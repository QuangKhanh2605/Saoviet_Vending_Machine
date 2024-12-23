
/*
 * user_app.c
 *
 *  Created on: Dec 14, 2021
 *      Author: Chien
 */

#include "user_app_comm.h"
#include "user_define.h"

#include "user_uart.h"

#include "user_timer.h"
#include "user_lpm.h"
#include "gpio.h"
//#include "i2c.h"
//#include "adc.h"
#include "user_comm_vending_machine.h"
#include "user_inc_vending_machine.h"
/*==========Static func====================*/
static uint8_t _Cb_Timer_IRQ(uint8_t event);
static uint8_t _Cb_Uart_Rx_Debug(uint8_t event);
static uint8_t _Cb_Uart_Error(uint8_t event);

static uint8_t _Cb_HandLer_IDLE(uint8_t event);
static uint8_t _Cb_Tx_Timer (uint8_t event);
static uint8_t _Cb_Set_RTC (uint8_t event);
static void Cb_TX_Timer_Event(void *context);
static uint8_t _Cb_Save_Box (uint8_t event);

static uint32_t  AppCom_Calcu_Period_To_RoudTime (uint32_t FreqWakeup);


/*=================Var struct======================*/
sEvent_struct sEventAppComm[] =
{
	{ _EVENT_TIMMER_IRQ, 		    0, 0, 0, 	    _Cb_Timer_IRQ },
	{ _EVENT_UART_RX_DEBUG, 	    0, 0, 20, 	    _Cb_Uart_Rx_Debug },
    { _EVENT_UART_ERROR, 	        0, 0, 20, 	    _Cb_Uart_Error },
    { _EVENT_SET_RTC, 				0, 0, 0, 	    _Cb_Set_RTC },
	{ _EVENT_IDLE, 					1, 0, 1000,     _Cb_HandLer_IDLE },
    { _EVENT_TX_TIMER,		        0, 0, 0, 	    _Cb_Tx_Timer },
    //Event Log
    { _EVENT_SAVE_BOX,		        0, 0, 0, 	    _Cb_Save_Box },  
};

/*===============================================*/
char aSaoVietCom[15][71] =
{
    {"       =======================================================       \r\n"},
    {"                                                                     \r\n"},
    {"* * *      * * *         *                 *         * * *      * * *\r\n"},
    {"*        *       *        *               *        *       *        *\r\n"},
    {"*        *                 *             *        *                 *\r\n"},
    {"          *                 *           *        *                   \r\n"},
    {"            *                *         *         *                   \r\n"},
    {"              *               *       *          *                   \r\n"},
    {"                *              *     *           *                   \r\n"},
    {"        *        *              *   *             *                  \r\n"},
    {"         *       *               * *               *       *         \r\n"},
    {"           * * *                  *           *      * * *           \r\n"},
    {"                                                                     \r\n"},
    {"                                                                     \r\n"},
    {"         =====================================================       \r\n"},
};


sData   sFirmVersion = {(uint8_t *) "SVTH_SVM_OXY_V1_1_1", 19};

static UTIL_TIMER_Object_t TimerTx;


/*======================Function========================*/
/*====== Func Call Event ==========*/
static uint8_t _Cb_Timer_IRQ(uint8_t event)
{
	UTIL_TIMER_IRQ_Handler();

	return 1;
}

/*
    Khi co su kien:
        + Start: lay moc thoi gian systick
                 Lay moc length hien tai
        + cu 5ms vao check:
                ++ Neu length bang voi length truoc: Nhan xong
                ++ Neu length khac length truoc: thuc hien nhu buoc start
                                                 tiep tuc enable cho check tiep
*/
static uint8_t _Cb_Uart_Rx_Debug(uint8_t event)
{
    static uint8_t  MarkFirstRecvUart = 0;
    static uint16_t LastLengthRecv = 0; 
        
    if (MarkFirstRecvUart == 0)
    {
        MarkFirstRecvUart = 1;
        LastLengthRecv = sUartDebug.Length_u16;
        fevent_enable(sEventAppComm, event);
    } else
    {
        if (sUartDebug.Length_u16 == LastLengthRecv)
        {
            MarkFirstRecvUart = 0;
            UTIL_Printf( DBLEVEL_L, sUartDebug.Data_a8, sUartDebug.Length_u16 );
            UTIL_Printf_Str( DBLEVEL_L,"\r\n");

            Check_AT_User(&sUartDebug, _AT_REQUEST_SERIAL);

            Reset_Buff(&sUartDebug);
        } else
        {
            LastLengthRecv = sUartDebug.Length_u16;
            fevent_enable(sEventAppComm, event);
        }
    }
    
    sModem.CountEventWaitConfg_u8 = 0;  //Cho phep giu 20s config

	return 1;
}

/*
    Func: Event Uart Error: 
        + DMA khi error mcu se disable IRQ, nen can init lai
        + Int: Khong bi disable, nen k can init la
*/
static uint8_t _Cb_Uart_Error(uint8_t event)
{
    UTIL_Printf_Str (DBLEVEL_M, "u_app_com: event uart error!\r\n" );
    
    ReInit_Rx_Mode_Uart_Error();
    
    return 1;
}



static uint8_t _Cb_Set_RTC(uint8_t event)
{
    UTIL_Set_RTC(sRTCSet);

    fevent_enable(sEventAppComm, _EVENT_IDLE);

    return 1;
}


static uint8_t _Cb_HandLer_IDLE(uint8_t event)
{
    static uint8_t MarkGetStime_u8 = 0;
    static uint8_t MarkResetDaily = 0;
    static uint32_t LandMarkWaitGPSEmpty = 0;
    uint8_t IsAllowResetMcu = true;
    
	Get_RTC();

#ifdef USING_APP_WM
    AppWm_Get_VBat();
    AppWm_Get_VOut();
#endif 
   
#ifdef USING_APP_SIM
    //Check True Update firmware -> wait Pulse increase to reset.
    if ((sModem.IsOverFivePulse_u8 == TRUE) && (sSimVar.IsUpdateFinish_u8 == 1))
        Reset_Chip();
    
    //Check Request Stime To Server
    if ((sRTC.hour == 0) && (MarkGetStime_u8 == 0))
    {
        MarkGetStime_u8 = 1;
        sMQTT.aMARK_MESS_PENDING[SEND_SERVER_TIME_PENDING] = TRUE;
    } else if (sRTC.hour != 0)
    {
        MarkGetStime_u8 = 0;
    }
    
#ifdef USING_APP_MEM
    if (AppMem_Queue_Write_Empty() == true) 
    {
        if (sAppSimVar.sGPS.Index_u8 > 1)
        {
            LandMarkWaitGPSEmpty = RtCountSystick_u32;
            if (Check_Time_Out(LandMarkWaitGPSEmpty, 600000) == false)
            {
                IsAllowResetMcu = false;
            }
        }
    }   
#endif
    
    //Check reset daily: neu dang lay do gps-> doi xong ms reset
    if (sRTC.hour != 14)
    {
        MarkResetDaily = 1;
    }
    
    if ( (sRTC.hour == 14) && (IsAllowResetMcu == true) && (MarkResetDaily == 1) )
    {
        //Reset thiet bi
//        Reset_Chip();
    }  
#endif

	return 1;
}


static uint8_t _Cb_Tx_Timer(uint8_t event)
{
#ifdef USING_APP_WM
    //Active Event Check Alarm
    fevent_active(sEventAppWM, _EVENT_ENTRY_WM);
#endif
    
#ifdef USING_APP_EMET
    //Active Event Check Alarm
    fevent_active(sEventAppEmet, _EVENT_TEMH_LOG_TSVH);
    fevent_active(sEventAppEmet, _EVENT_SCAN_ALARM);
#endif

#ifdef USING_APP_TEMH
    //Active Check Mode
    sTempHumi.IRQMainPowerDetect_u8 = pending;             //avoid sleep
    fevent_active(sEventAppTempH, _EVENT_CHECK_AC_POWER);
#endif
    
#ifdef USING_APP_CTRL_OXY
    if(sStateCtrlOxy.StateDCU == 1)
    {
        fevent_active(sEventAppCtrlOxy, _EVENT_CTRL_OXY_LOG_TSVH);  
    }
#endif 
 
    if (sModem.ModeSimPower_u8 == _POWER_MODE_SAVE)
    {
        //Increase Count To Send Mess
        sModem.CountSleepToSend_u8++;
        if (sModem.CountSleepToSend_u8 >= sFreqInfor.NumWakeup_u8)
        {
            sModem.CountSleepToSend_u8 = 0;
        #ifdef USING_APP_SIM
            fevent_active(sEventAppSim, _EVENT_SIM_REQ_GPS);
            AppSim_Start_Module_Sim();
            sModem.IsDutyCycle_u8  = true;
        #endif
            
        #ifdef USING_APP_TEMH
            fevent_active(sEventAppTempH, _EVENT_TEMH_ENTRY);
        #endif
        }
    } else
    {
        sModem.CountSleepToSend_u8 = 0;
        
    #ifdef USING_APP_SIM
        fevent_active(sEventAppSim, _EVENT_SIM_REQ_GPS);
        AppSim_Start_Module_Sim();
        sModem.IsDutyCycle_u8  = true;
    #endif
        
    #ifdef USING_APP_TEMH
        fevent_active(sEventAppTempH, _EVENT_TEMH_ENTRY);
    #endif 
    }
         

    return 1;
}


static uint8_t _Cb_Save_Box (uint8_t event)
{
    
   return 1;
}


/*=========================== Func App Main ========================*/
void SysApp_Init (void)
{       
  
#ifdef USING_APP_EMET
    __HAL_IWDG_START(&hiwdg);
#endif

#ifdef USING_APP_WM
    LPTIM_Init_Feature();
#endif
       
#ifdef USING_APP_LORA
    AppLora_Init ();
#else
    //Init timer: Init RTC...cau h�nh alarm profile
	UTIL_TIMER_Init();
#endif
    
	//Get RTC
	Get_RTC();
    
    /*Init low power manager*/
    UTIL_LPM_Init();
}
 

void SysApp_Setting (void)
{
    AppComm_Init();
#ifdef USING_APP_SIM
    AppSim_Init(); 
#endif
   
#ifdef USING_APP_WM
    AppWm_Init();
#endif
    
#ifdef USING_APP_TEMH
    AppTemH_Init();
#endif
    
#ifdef USING_APP_MEM
    AppMem_Init();
#endif
    //Dang ki cac event
	UTIL_TIMER_Create(&TimerTx,  0xFFFFFFFFU, UTIL_TIMER_ONESHOT, Cb_TX_Timer_Event, NULL);
	UTIL_TIMER_SetPeriod (&TimerTx, 20000);   //Set sFreqInfor.FreqSendUnitMin_u32 * 60000
	UTIL_TIMER_Start (&TimerTx);
    
#ifdef USING_APP_VENDING_MACHINE
    Init_AppVendingMachine();
#endif
}

/*
    Note:
    - khi gen lai
        + Phai sua lai chan uart 2 thanh Pull up o trong code
        + Comment fun init in rtc

*/
//uint8_t read_e[4];
//uint8_t write_e[4];
/*
    Func: Init Queue trong main
*/
uint8_t TaskStatus_u8 = 0;
void Main_Task (void)
{
//    uint8_t TaskStatus_u8 = 0;
    SysApp_Init();
    SysApp_Setting();
    
    UTIL_Printf_Str (DBLEVEL_M, "\r\n=========================\r\n" );
    UTIL_Printf_Str (DBLEVEL_M,     "==== MAIN PROGRAMING ====\r\n" );
    UTIL_Printf_Str (DBLEVEL_M,     "=========================\r\n" );
    
//    UTIL_Log_Str (DBLEVEL_L, "DAY LA VERSION: ");
//	UTIL_Log(DBLEVEL_L, sFirmVersion.Data_a8, sFirmVersion.Length_u16);
//    UTIL_Log_Str (DBLEVEL_L, "\r\n");
    
#ifdef USING_APP_EMET
    Screen_Init();
    Button_Init();
    EMeasure_Init();
    Relay_Init();
    LED_Init();
    Temp_Humid_Init();
#endif
    
    HAL_Delay(50);
	for (;;)
	{
    #ifdef USING_APP_VENDING_MACHINE
        AppVendingMachine_Task();
    #endif

		TaskStatus_u8 = 0;

		TaskStatus_u8 |= AppComm_Task();
        
//    #ifdef USING_APP_MEM
////        if ( sSimCommon.PowerStatus_u8 == _POWER_CONN_MQTT) 
////        {
////            if (Check_Time_Out(sSimVar.LandMarkSendAT_u32, 800) == true)
////                TaskStatus_u8 |= AppMem_Task();
////        } else
////            TaskStatus_u8 |= AppMem_Task();
//        
//        TaskStatus_u8 |= AppMem_Task();
//    #endif
        
//    #ifdef USING_APP_SIM
//        TaskStatus_u8 |= AppSim_Task();
//        TaskStatus_u8 |= Sim_Task();
//    #endif
//      
//    #ifdef USING_APP_TEMH
//        TaskStatus_u8 |= AppTemH_Task();
//    #endif
//            
//    #ifdef USING_APP_WM
//        TaskStatus_u8 |= AppWm_Task(); 
//    #endif
        
    #ifdef USING_APP_EMET
        Button_Task();
        Screen_Task();
        EMeasure_Task();
        Relay_Task();
        LED_Task();
        Temp_Humid_Task();
    #endif
      
    #ifdef USING_APP_LORA
        TaskStatus_u8 |= AppLora_Process();
    #endif
//
//        if ((TaskStatus_u8 == 0) && \
//            (sModem.ModeSimPower_u8 == _POWER_MODE_SAVE) && \
//            (sTempHumi.IRQMainPowerDetect_u8 == FALSE) )
//        {
//        #ifdef USING_APP_SIM
//            //Neu thuc hien xong tranfer qua module sim (hoac sim deep sleep | Poweroff) -> set che do ngu sau
//            if (sSimCommon.PowerStatus_u8 == _POWER_POWER_OFF)
//            {
//                UTIL_LPM_SetStopMode((UTIL_LPM_State_t) LPM_FALSE);
//            } else
//                UTIL_LPM_SetStopMode((UTIL_LPM_State_t) LPM_TRUE);
//        #else
//            UTIL_LPM_SetStopMode((UTIL_LPM_State_t) LPM_FALSE);
//        #endif
//            //Func Lowpower
//            UTIL_LPM_EnterLowPower();
//        }
	}
}


/*=================Func handler=====================*/

void AppComm_Init (void)
{
    //Init Modem
	Init_Uart_Module();
    //Init information in Memory
	Init_Memory_Infor();
    //Loai DCU
    sModem.TypeModem_u8 = _CONTROL_OXY;   
    //Set Con tro mode Power: Save_Mode. OnlineMode
    sModem.ModeSimPower_u8 =  _POWER_MODE_ONLINE; // _POWER_MODE_SAVE; // _POWER_MODE_ONLINE;
    //Func Pointer Lib Timer
    pModemProcessIRQTimer = AppComm_IRQ_Timer_CallBack;
    //Func Pointer Lib LPM
    sLPMFeatureHandler = &sModemLPMHandler; 
    //Init Func Pointer Log to mem
    pFunc_Log_To_Mem = &LOG_Save_Record; 
}

/*
    Func: tao gia tri delay theo ID
*/
uint32_t AppComm_Get_Random_Follow_ID (void)
{
    uint32_t Result = 0;
    
#ifdef USING_APP_LORA
    uint16_t Index = sModem.sDCU_id.Length_u16 - 1;
    
    Result = (*(sModem.sDCU_id.Data_a8 + Index) - 0x30) + (*(sModem.sDCU_id.Data_a8 + Index - 1) - 0x30) * 10 ;

    if (sFreqInfor.FreqWakeup_u32 != 0)
    {
        Result = (Result * 2) % (sFreqInfor.FreqWakeup_u32 * 60);
        Result *= 1000;
    } else
    {
        Result = 10000;
    }
#endif
    
    return Result;
}

uint8_t AppComm_Task(void)
{
	uint8_t i = 0;
	uint8_t Result = 0;

	for (i = 0; i < _EVENT_END_COMM; i++)
	{
		if (sEventAppComm[i].e_status == 1)
		{
            if (i != _EVENT_IDLE)
                Result = 1;

			if ((sEventAppComm[i].e_systick == 0) ||
					((HAL_GetTick() - sEventAppComm[i].e_systick)  >=  sEventAppComm[i].e_period))
			{
                sEventAppComm[i].e_status = 0;  //Disable event
				sEventAppComm[i].e_systick = HAL_GetTick();
				sEventAppComm[i].e_function_handler(i);
			}
		}
	}
    
    //Enable Event Period again
    if (sEventAppComm[_EVENT_IDLE].e_status == 0)
        fevent_enable(sEventAppComm, _EVENT_IDLE);

	return Result;
}


 
void AppComm_IRQ_Timer_CallBack (void)
{
    fevent_active(sEventAppComm, _EVENT_TIMMER_IRQ);
}




/*
    - Func: Tinh lai thoi gian sai lệch de hẹn gio tròn
*/
static uint32_t AppCom_Calcu_Period_To_RoudTime (uint32_t FreqWakeup)
{
    uint32_t FreqCacul = 0;

    Get_RTC();

    FreqCacul = FreqWakeup - (sRTC.min % FreqWakeup);   //Phep tinh nay tinh ra gia tri 1->Stimer.min. Khong co gia tri 0
    //Sub second current, and add to 2 (dư phong)
    if (sRTC.sec < (FreqCacul * 60))
    {
        FreqCacul  = FreqCacul * 60 - sRTC.sec + 2;
        return (FreqCacul * 1000);
    }

    return (2 * 60000); //default
}


static void Cb_TX_Timer_Event(void *context)
{
//    UTIL_Printf_Str (DBLEVEL_M, "Timer TX Event callback\r\n" );

#ifdef USING_APP_MEM
    if (AppMem_Status() == ERROR)
    {
        Modem_Alarm_Emergency((uint8_t*) "u_app_comm: memory error\r\n", 26);
    }
#endif
    fevent_active(sEventAppComm, _EVENT_TX_TIMER);

    AppComm_Set_Next_TxTimer();
}


void AppComm_Set_Next_TxTimer (void)
{
    static uint8_t CountMessInit = 0;
    uint32_t FreqCacul = 10000;

    //Get chu ki gui tiep theo
	if (CountMessInit < MAX_NUNBER_SEND_INIT)
    {
		CountMessInit ++;
        sModem.CountSleepToSend_u8 = sFreqInfor.NumWakeup_u8;  //cho send luon ban tin
	} else
    {
        if (sModem.ModeSimPower_u8 == _POWER_MODE_ONLINE)
        {
            sFreqInfor.FreqSendUnitMin_u32 = sFreqInfor.FreqSendOnline_u32;  //Doi sang minute
            Get_RTC();
            FreqCacul = sFreqInfor.FreqSendUnitMin_u32 - (sRTC.min % sFreqInfor.FreqSendUnitMin_u32);
            FreqCacul = FreqCacul * 60 - sRTC.sec + 2;
            FreqCacul *= 1000;
        } else
        {
            sFreqInfor.FreqSendUnitMin_u32 = sFreqInfor.NumWakeup_u8 * sFreqInfor.FreqWakeup_u32;
            //cacul period again to send at rounding stime:
            FreqCacul = AppCom_Calcu_Period_To_RoudTime (sFreqInfor.FreqWakeup_u32);
        }

        //set Period again.
        UTIL_TIMER_SetPeriod(&TimerTx, FreqCacul);
    }
    //Set tiep timer Tx vơi Duty = Thơi gian tròn cua chu ki
	UTIL_TIMER_Start(&TimerTx);
}




/*
    Func: Callback IRQ gpio:
        + PIN 6: DTC
        + PIN 14: CONFIG Button
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {
       case GPIO_PIN_6:
            
            break;
        case GPIO_PIN_11:
//            fevent_active(sEventAppTempH, _EVENT_MAGNETIC_WAKE);
            break;
        case GPIO_PIN_8:
//            sTempHumi.IRQMainPowerDetect_u8 = pending;
//            fevent_active(sEventAppTempH, _EVENT_CHECK_AC_POWER);
            break;
        default:
            break;           
    }      
    
     if (GPIO_Pin == IR_Sensor_Irq_Pin)
     {
        sPushMotor.IrSensor++;
     }
     
     if(GPIO_Pin == Count_Pin)
     {
        sPushMotor.PulseCount++;
     }
     
     if (GPIO_Pin == Vib_Sensor_Irq_3_Pin)
     {
        sStatusVib_Scan.Sensor3++;
     }
     
     if (GPIO_Pin == Vib_Sensor_Irq_2_Pin)
     {
        sStatusVib_Scan.Sensor2++;
     }
     
     if (GPIO_Pin == Vib_Sensor_Irq_1_Pin)
     {
        sStatusVib_Scan.Sensor1++;
     }
}




/*
    Func:            
*/
void AppComm_SV_Protocol_Packet_Data (uint8_t *pTarget, uint16_t *LenTarget, uint8_t Obis, 
                                        void *pData, uint8_t LenData, uint8_t Scale)
{
    uint16_t Pos = *LenTarget, i= 0;
    ST_TIME_FORMAT *sRTCTemp;
    uint8_t *TempU8;
    
    if (LenData == 0)
        return;
    
    pTarget[Pos++] = Obis;
    pTarget[Pos++] = LenData;
            
    switch (Obis)
    {
        case OBIS_TIME_DEVICE:
            sRTCTemp = pData;

            pTarget[Pos++] = sRTCTemp->year;
            pTarget[Pos++] = sRTCTemp->month;
            pTarget[Pos++] = sRTCTemp->date;
            pTarget[Pos++] = sRTCTemp->hour;
            pTarget[Pos++] = sRTCTemp->min;
            pTarget[Pos++] = sRTCTemp->sec;
            break;
        case OBIS_ENVI_TEMP_1:
        case OBIS_ENVI_HUMI_1: 
        case OBIS_ENVI_TEMP_2:
        case OBIS_ENVI_HUMI_2:
        case OBIS_EMET_CUR:
        case OBIS_DEV_VOL1:
        case OBIS_DEV_VOL2:
        case OBIS_RSSI_1:
        case OBIS_FREQ_SEND:
            AppComm_Sub_Packet_Integer(pTarget, &Pos, pData, LenData);
            //
            if (Scale != 0xAA)
                pTarget[Pos++] = Scale;
            
            break; 
        case OBIS_GPS_LOC:
            TempU8 = (uint8_t *) pData;
            for (i = 0; i < LenData; i++)
                pTarget[Pos++] = *(TempU8 + i);
            break; 
        default:
            break;
    }
    
    *LenTarget = Pos ;
}


void AppComm_Sub_Packet_Integer (uint8_t *pTarget, uint16_t *LenTarget, void *Data, uint8_t LenData)
{
    uint16_t Pos = *LenTarget;
    
    uint8_t  *TempU8;
    uint16_t *TempU16;
    uint32_t *TempU32;
    uint64_t *TempU64;
    
    switch (LenData)
    {
        case 1:    
            TempU8 = (uint8_t *) Data;
            pTarget[Pos++] = *TempU8;
            break;
        case 2:
            TempU16 = (uint16_t *) Data;
            pTarget[Pos++] = ((*TempU16) >> 8) & 0xFF;
            pTarget[Pos++] =  (*TempU16) & 0xFF;
            break;
        case 4:
            TempU32 = (uint32_t *) Data;
            pTarget[Pos++] = ((*TempU32) >> 24) & 0xFF;
            pTarget[Pos++] = ((*TempU32) >> 16) & 0xFF;
            pTarget[Pos++] = ((*TempU32) >> 8) & 0xFF;
            pTarget[Pos++] =  (*TempU32) & 0xFF;
            break;
        case 8:
            TempU64 = (uint64_t *) Data;
            pTarget[Pos++] = ((*TempU64) >> 56) & 0xFF;
            pTarget[Pos++] = ((*TempU64) >> 48) & 0xFF;
            pTarget[Pos++] = ((*TempU64) >> 40) & 0xFF;
            pTarget[Pos++] = ((*TempU64) >> 32) & 0xFF;
            pTarget[Pos++] = ((*TempU64) >> 24) & 0xFF;
            pTarget[Pos++] = ((*TempU64) >> 16) & 0xFF;
            pTarget[Pos++] = ((*TempU64) >> 8) & 0xFF;
            pTarget[Pos++] =  (*TempU64) & 0xFF;
            break;
        default:
            break;
    }
    
    *LenTarget = Pos ;
}





