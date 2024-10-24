#include "user_app_pc_box.h"
#include "user_inc_vending_machine.h"
#include "user_external_flash.h"
#include "rtc.h"
#include "user_define.h"
#include "iwdg.h"
/*============== Function Static =========*/
static uint8_t fevent_pcbox_entry(uint8_t event);
static uint8_t fevent_pcbox_receive_handle(uint8_t event);
static uint8_t fevent_pcbox_complete_receive(uint8_t event);
static uint8_t fevent_pcbox_log_tsvh(uint8_t event);
static uint8_t fevent_wdg_stm32f4(uint8_t event);

static uint8_t fevent_queue_respond_immediately(uint8_t event);
static uint8_t fevent_queue_respond_time(uint8_t event);
static uint8_t fevent_handle_respond_error(uint8_t event);

static uint8_t fevent_dcu_ping_pc_box(uint8_t event);
static uint8_t fevent_reset_pc_box(uint8_t event);
static uint8_t fevent_wait_reset_pc_box(uint8_t event);

static uint8_t fevent_on_off_cycle_pc_box(uint8_t event);

static uint8_t fevent_refresh_dcu(uint8_t event);

static uint8_t fevent_get_real_time_dcu(uint8_t event);
/*=============== Struct =================*/
sEvent_struct               sEventAppPcBox[]=
{
  {_EVENT_PC_BOX_ENTRY,                 1, 5, TIME_ON_DCU,              fevent_pcbox_entry},
  {_EVENT_PC_BOX_RECEIVE_HANDLE,        0, 0, 5,                        fevent_pcbox_receive_handle},
  {_EVENT_PC_BOX_COMPLETE_RECEIVE,      0, 0, 5,                        fevent_pcbox_complete_receive},
  {_EVENT_PC_BOX_LOG_TSVH,              1, 5, TIME_ENTRY,               fevent_pcbox_log_tsvh},
  
  {_EVENT_WDG_STM32F4,                  1, 0, TIME_RESET_WDG,           fevent_wdg_stm32f4},
  
  {_EVENT_QUEUE_RESPOND_IMMEDIATELY,    0, 0, 5,                        fevent_queue_respond_immediately},
  {_EVENT_QUEUE_RESPOND_TIME,           0, 0, TIME_RESPOND_PC_BOX,      fevent_queue_respond_time},
  {_EVENT_HANDLE_RESPOND_ERROR,         0, 5, 30*60000,                 fevent_handle_respond_error},
  
  {_EVENT_DCU_PING_PC_BOX,              0, 5, TIME_PING_PCBOX,          fevent_dcu_ping_pc_box},
  {_EVENT_RESET_PC_BOX,                 0, 0, TIME_RESET_PC_BOX,        fevent_reset_pc_box},
  {_EVENT_WAIT_RESET_PC_BOX,            0, 0, TIME_WAIT_RESET_PC_BOX,   fevent_wait_reset_pc_box},
  
  {_EVENT_ON_OFF_CYCLE_PC_BOX,          1, 5, 15000,                    fevent_on_off_cycle_pc_box},
  
  {_EVENT_REFRESH_DCU,                  0, 5, TIME_REFRESH_DCU,         fevent_refresh_dcu},
  {_EVENT_GET_REAL_TIME_DCU,            0, 5, 60000,                    fevent_get_real_time_dcu},
};

sDataQueueRespondPcBox          sQueueRespondPcBox_Prio_0[NUMBER_ITEM_QUEUE];
Struct_Queue_Type               qRespondPcBox_Prio_0;

sDataQueueRespondPcBox          sQueueRespondPcBox_Prio_1[NUMBER_ITEM_QUEUE];
Struct_Queue_Type               qRespondPcBox_Prio_1;

StructStatusApp                 sStatusApp = {_APP_FREE};
StructOnOffCyclePcBox           sCycleOnOffPcBox;
StructParamPcBox                sParamPcBox=
{
    .CountPingPcBox = 0,
    .TimeTSVH        = TIME_SEND_TSVH/TIME_ONE_MINUTES,
    .StatePcBox      = _STT_PCBOX_DISCONNECT,
    .UsingCrc        = _UNUSING_CRC,
};

uint8_t aBufferRespondPcBox[NUMBER_MAX_BUFFER];
sData   sRespPcBox=
{
    .Data_a8    = aBufferRespondPcBox,
    .Length_u16 = 0,
};

static UTIL_TIMER_Object_t TimerOnOffPcBox;
/*============== Function Static =============*/
static uint8_t fevent_pcbox_entry(uint8_t event)
{
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_RESET_DCU;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = AFTER_RESET_DCU;
    
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
    
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"ON_MCU\r\n", sizeof("ON_MCU\r\n")-1); 
    
    UTIL_MEM_set(sUartPcBox.Data_a8 , 0x00, sUartPcBox.Length_u16);
    sUartPcBox.Length_u16 = 0;
    
    fevent_active(sEventAppPcBox, _EVENT_PC_BOX_RECEIVE_HANDLE);
    fevent_enable(sEventAppPcBox, _EVENT_REFRESH_DCU);
    fevent_active(sEventAppPcBox, _EVENT_QUEUE_RESPOND_IMMEDIATELY);
    fevent_active(sEventAppPcBox, _EVENT_GET_REAL_TIME_DCU);
    fevent_enable(sEventAppPcBox, _EVENT_HANDLE_RESPOND_ERROR);
    
    if(sParamPcBox.StatePcBox != _STT_PCBOX_SLEEP)
    {
      fevent_active(sEventAppPcBox, _EVENT_DCU_PING_PC_BOX);
    }
    return 1;
}

static uint8_t fevent_wdg_stm32f4(uint8_t event)
{
/*--------------Reset WDG-------------*/
    Reset_WDG_DCU();
    
    fevent_enable(sEventAppPcBox, event);
    return 1;
}

static uint8_t fevent_pcbox_receive_handle(uint8_t event)
{
    static uint16_t countBuffer_uart = 0;

    if(sUartPcBox.Length_u16 != 0)
    {
        if(countBuffer_uart == sUartPcBox.Length_u16)
        {
            countBuffer_uart = 0;
            fevent_active(sEventAppPcBox, _EVENT_PC_BOX_COMPLETE_RECEIVE);
            return 1;
        }
        else
            countBuffer_uart = sUartPcBox.Length_u16;
    }
    
    fevent_enable(sEventAppPcBox, event);
    return 1;
}

static uint8_t fevent_pcbox_complete_receive(uint8_t event)
{
    uint16_t pos        = 0;
    uint8_t Length_Data = 0;
    uint8_t UsingCrc    = 0;
    uint16_t Crc_Check  = 0;
    uint16_t Crc_Recv   = 0;
    uint8_t Respond     = false;
    
    uint8_t Pos_Motor = 0;
    uint8_t Number = 0;
    
    uint16_t TimeOffPcBox = 0;
    
    uint8_t Recv_Result = 0;
    
    AppPcBox_Debug(sUartPcBox.Data_a8, sUartPcBox.Length_u16 , _RECV_PCBOX);

    Crc_Recv = (sUartPcBox.Data_a8[sUartPcBox.Length_u16-1] << 8) |
               (sUartPcBox.Data_a8[sUartPcBox.Length_u16-2]);
    Calculator_Crc_U16(&Crc_Check, sUartPcBox.Data_a8, sUartPcBox.Length_u16 - 2);
    
    if(Crc_Check == Crc_Recv)   UsingCrc = 1;
    else if(sParamPcBox.UsingCrc == _UNUSING_CRC) UsingCrc = 1;
    
    if(UsingCrc == 1 && sParamPcBox.StatePcBox != _STT_PCBOX_SLEEP)
    {
        while((pos + 2) <= sUartPcBox.Length_u16 - 2) //co 2 byte CRC cuoi cung
        {
            switch(sUartPcBox.Data_a8[pos++])
            {
                case OBIS_PC_BOX_FIX_MOTOR:     //Obis Fix Motor
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                    Fix_Motor(sUartPcBox.Data_a8[pos]);
                    pos +=Length_Data;
                    Recv_Result=1;
                  }
                  break;
              
                case OBIS_PC_BOX_CTRL_MOTOR:    //Obis Push Motor
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data <= NUMBER_MOTOR * 2 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                    if(sParamDelivery.StateHanlde == DELIVERY_FREE)
                    {
                        Reset_StateDelivery();
                        
                        for(uint8_t i = 0; i < Length_Data; i+=2)
                        {
                            Pos_Motor = sUartPcBox.Data_a8[pos+i];
                            Number = sUartPcBox.Data_a8[pos+i+1];
                            if(Pos_Motor <= NUMBER_MOTOR )
                            {
                                if((sParamDelivery.aDataPush[Pos_Motor - 1] + Number) <= NUMBER_PUSH_MOTOR)
                                {
                                    sParamDelivery.aDataPush[Pos_Motor - 1] += Number;
                                }
                                else
                                {
                                    sParamDelivery.aDataPush[Pos_Motor - 1] = NUMBER_PUSH_MOTOR;
                                }
                            }
                        }
                        Delivery_Entry();
                        pos +=Length_Data;
                        Recv_Result=1;
                    }
                    else
                    {
                        UTIL_Printf(DBLEVEL_L, (uint8_t*)"Handle: DELIVERY_BUSY", sizeof("Handle: DELIVERY_BUSY")-1);
                        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
                    }
                  }
                  break;

                case OBIS_RESET_DCU:    //Obis reset Dcu
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {              
                      if(sUartPcBox.Data_a8[pos] == 0x01)
                          ResetDCU();
                  }
                  break; 
                  
                case OBIS_ON_OFF_RELAY_SCREEN:  //Obis Ctrl relay Screen
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x01)
                      {
                        ControlRelay(RELAY_SCREEN, sUartPcBox.Data_a8[pos], _RL_RESPOND, _RL_DEBUG, _RL_CTRL);
                        pos +=Length_Data;
                        Recv_Result=1;
                      }
                  }
                  break;   
                  
                case OBIS_ON_OFF_RELAY_LAMP:    //Obis Ctrl relay Lamp
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x01)
                      {
                        if(sElectric.PowerPresent != POWER_OFF)
                            ControlRelay(RELAY_LAMP, sUartPcBox.Data_a8[pos], _RL_RESPOND, _RL_DEBUG, _RL_CTRL);
                        else
                        {
                            sStatusRelay.Lamp_Ctrl = sUartPcBox.Data_a8[pos];
                            ControlRelay(RELAY_LAMP, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_UNCTRL);
                        }
                        pos +=Length_Data;
                        Recv_Result=1;
                      }
                  }
                  break; 
         
                case OBIS_ON_OFF_RELAY_WARM:    //Obis Ctrl relay Warm
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x01)
                      {
                        if(sUartPcBox.Data_a8[pos] == OFF_RELAY)
                            fevent_active(sEventAppRelay, _EVENT_RELAY_WARM_OFF);
                        else if(sUartPcBox.Data_a8[pos] == ON_RELAY)
                        {
                            if(sElectric.PowerPresent != POWER_OFF)
                            {
                                OnRelay_Warm(sTimeCycleWarm.Run);
                                fevent_active(sEventAppRelay, _EVENT_RELAY_WARM_ON);
                            }
                            else
                                fevent_active(sEventAppRelay, _EVENT_RELAY_WARM_OFF);
                        }
                        pos +=Length_Data;
                        Recv_Result=1;
                      }
                  }
                  break; 
                  
                case OBIS_SETUP_TEMP:   //Obis cai dat nhiet do
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x03 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                        sTemp_Thresh_Recv.Value  =  sUartPcBox.Data_a8[pos];
                        sTemp_Thresh_Recv.Value  =  sTemp_Thresh_Recv.Value << 8;
                        sTemp_Thresh_Recv.Value |=  sUartPcBox.Data_a8[pos+1];
                        sTemp_Thresh_Recv.Scale  =  sUartPcBox.Data_a8[pos+2];
                        fevent_active(sEventAppTemperature, _EVENT_TEMP_SET_SETUPTEMP);
                        pos +=Length_Data;
                        Recv_Result=1;
                  }
                  break;
                  
                case OBIS_PC_BOX_PING_DCU:  //Obis Ping DCU
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if((pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                        sRespPcBox.Length_u16 = 0;
                        sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_PC_BOX_PING_DCU;
                        sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = Length_Data;
                        while(Length_Data > 0)
                        {
                            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sUartPcBox.Data_a8[pos++];
                            Length_Data--;
                        }
                        Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
//                        pos +=Length_Data;
                        Recv_Result=1;
                  }
                  break;
                  
                case OBIS_GET_DCU_ID:   //Obis get seri DCU
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if((pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                        uint8_t aData[20]={0};
                        uint8_t length = 0;
                        aData[length++] = OBIS_GET_DCU_ID;
                        aData[length++] = sModem.sDCU_id.Length_u16;
                        for(uint8_t i = 0; i< sModem.sDCU_id.Length_u16; i++)
                            aData[length++] = sModem.sDCU_id.Data_a8[i];
                        
                        RespondPcBox_String(aData, length);
                        pos +=Length_Data;
                        Recv_Result=1;
                  }
                  break;         
                  
                  
                case OBIS_SET_DCU_ID:   //Obis cai dat seri DCU
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if((pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                        Save_DcuID(sUartPcBox.Data_a8 + pos, Length_Data);
                        RespondPcBox_String(sUartPcBox.Data_a8 +pos -2, Length_Data +2);
                        pos +=Length_Data;
                        Recv_Result=1;
                  }
                  break;
                  
                case OBIS_DCU_PING_PC_BOX:  //Obis Ping PcBox
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x01)
                      {
                          sParamPcBox.CountPingPcBox = 0;
                          sParamPcBox.CountResetPcBox = 0;
                      }

                      pos +=Length_Data;
                      Recv_Result=1;
                  }
                  break;
                  
                case OBIS_DCU_GET_REAL_TIME:  //Obis Get Real Time PcBox
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  RTC_TimeTypeDef sTime;
                  RTC_DateTypeDef sDate;
                  ST_TIME_FORMAT sTimeRTC;
                  if(Length_Data == 0x06 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      sDate.Year = sUartPcBox.Data_a8[pos];
                      sDate.Month = sUartPcBox.Data_a8[pos+1];
                      sDate.Date   = sUartPcBox.Data_a8[pos+2];
                      
                      sTime.Hours = sUartPcBox.Data_a8[pos+3];
                      sTime.Minutes = sUartPcBox.Data_a8[pos+4];
                      sTime.Seconds = sUartPcBox.Data_a8[pos+5];
                      
                      sTimeRTC.year   = sDate.Year % 100;
                      sTimeRTC.month  = sDate.Month;
                      sTimeRTC.date   = sDate.Date;
                      sTimeRTC.hour   = sTime.Hours; 
                      sTimeRTC.min    = sTime.Minutes;
                      sTimeRTC.sec    = sTime.Seconds;
                      
                      Recv_Result=1;
                      sDate.WeekDay = ((HW_RTC_GetCalendarValue_Second (sTimeRTC, 1) / SECONDS_IN_1DAY) + 6) % 7 + 1;
                  }
                    if(sDate.Year >= 20 && sDate.Month >= 1 && sDate.Month <= 12 && sDate.Date >=1 && sDate.Date <=31)
                    {
                        if(sTime.Hours < 24 && sTime.Minutes < 60 && sTime.Seconds <60)
                        {
                            if(sDate.WeekDay >=2 && sDate.WeekDay <=8)
                            {
                                HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
                                HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
                                RespondPcBox_String(sUartPcBox.Data_a8 + pos -2, Length_Data +2);
                                pos +=Length_Data;
                            }
                            else
                            {
                                Packing_Respond_PcBox((uint8_t*)"FAIL", 4);
                            }
                        }
                        else
                            Packing_Respond_PcBox((uint8_t*)"FAIL", 4);
                    }
                    else
                        Packing_Respond_PcBox((uint8_t*)"FAIL", 4);
                  break;
                  
                case OBIS_TIME_LOG_TSVH:    //Obis set time TSVH
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      Save_TimeTSVH(sUartPcBox.Data_a8[pos]);
                      pos +=Length_Data;
                      Recv_Result=1;
                  }
                  break;
                  
//                case OBIS_TIME_WARM_RUN:
//                  Length_Data = sUartPcBox.Data_a8[pos++];
//                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
//                  {
//                      if(Save_TimeRelayWarm(sUartPcBox.Data_a8[pos], sTimeCycleWarm.Wait) == 1)
//                         RespondPcBox_String(sUartPcBox.Data_a8 +pos -2, Length_Data +2);
//                      else
//                         Packing_Respond_PcBox((uint8_t*)"FAIL", 4);
//                      
//                      pos +=Length_Data;
//                  }
//                  break;
                  
//                case OBIS_TIME_WARM_WAIT:
//                  Length_Data = sUartPcBox.Data_a8[pos++];
//                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
//                  {
//                      if(Save_TimeRelayWarm(sTimeCycleWarm.Run, sUartPcBox.Data_a8[pos]) == 1)
//                         RespondPcBox_String(sUartPcBox.Data_a8 +pos -2, Length_Data +2);
//                      else
//                         Packing_Respond_PcBox((uint8_t*)"FAIL", 4);
//                      
//                      pos +=Length_Data;
//                  }
//                  break;
                  
                case OBIS_ON_OFF_CYCLE_PC_BOX:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(Save_CycleOnOffPcBox(sUartPcBox.Data_a8[pos],sCycleOnOffPcBox.HoursOFF, sCycleOnOffPcBox.MinutesOFF,
                                              sCycleOnOffPcBox.HoursON, sCycleOnOffPcBox.MinutesON) == 1)
                         RespondPcBox_String(sUartPcBox.Data_a8 +pos -2, Length_Data +2);
                      else
                         Packing_Respond_PcBox((uint8_t*)"FAIL", 4);
                      
                      pos +=Length_Data;
                      Recv_Result=1;
                  }
                  break;
                  
                case OBIS_TIME_OFF_ON_PC_BOX:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x04 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(Save_CycleOnOffPcBox(sCycleOnOffPcBox.State,sUartPcBox.Data_a8[pos], sUartPcBox.Data_a8[pos+1],sUartPcBox.Data_a8[pos+2], sUartPcBox.Data_a8[pos+3]) == 1)
                      {
                        RespondPcBox_String(sUartPcBox.Data_a8 + pos -2, Length_Data +2);
                      }
                      else
                         Packing_Respond_PcBox((uint8_t*)"FAIL", 4);
                      
                      pos +=Length_Data;
                      Recv_Result=1;
                  }
                  break;
                  
                case OBIS_ACCEPT_OFF_PCBOX:   
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x02 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                        TimeOffPcBox  =  sUartPcBox.Data_a8[pos];
                        TimeOffPcBox  =  TimeOffPcBox << 8;
                        TimeOffPcBox |=  sUartPcBox.Data_a8[pos+1];
                        
                        if(sCycleOnOffPcBox.State == _ON_CYCLE && sParamPcBox.StatePcBox != _STT_PCBOX_SLEEP)
                        {
                            if(Check_RealTime_OFF_PcBox(sRTC.hour, sRTC.min) == 1)
                            {
                              OFF_PcBox(TimeOffPcBox);
                            }
                            else
                              Packing_Respond_PcBox((uint8_t*)"FAIL", 4);
                        }
                        else
                          Packing_Respond_PcBox((uint8_t*)"FAIL", 4);
                        pos +=Length_Data;
                        Recv_Result=0;
                  }
                  break;
                  
                default:
                  break;
            }
        }
        if(Respond == true)
            RespondPcBox_String((uint8_t*)"OK", 2);
    }
    else
    {
        RespondPcBox_String((uint8_t*)"ERROR", 5);
        if(sParamPcBox.StatePcBox == _STT_PCBOX_SLEEP)
        {
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"PcBox: SLEEP", sizeof("PcBox: SLEEP")-1);
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
        }
        
        if(UsingCrc == 0)
        {
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"PcBox: CRC FAIL", sizeof("PcBox: CRC FAIL")-1);
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
        }
    }
    
    if(Recv_Result == 1)
    {
        if(sParamPcBox.StatePcBox == _STT_PCBOX_DISCONNECT)       //Nhan duoc giu lieu bat ki tu Pc Xac nhan la da ket noi
        {
            Save_StatePcBox(_STT_PCBOX_CONNECT);
            fevent_active(sEventAppPcBox, _EVENT_WAIT_RESET_PC_BOX);
        }
        
        sParamPcBox.CountPingPcBox = 0;
        sParamPcBox.CountResetPcBox = 0;
    }
    
    LedRecvPcBox = sUartPcBox.Length_u16;    
    
    UTIL_MEM_set(sUartPcBox.Data_a8 , 0x00, sUartPcBox.Length_u16);
    sUartPcBox.Length_u16 = 0;
    
    fevent_active(sEventAppPcBox, _EVENT_PC_BOX_RECEIVE_HANDLE);
    return 1;
}

static uint8_t fevent_pcbox_log_tsvh(uint8_t event)
{
/*----------------Log TSVH----------------*/
    sEventAppPcBox[_EVENT_PC_BOX_LOG_TSVH].e_period = sParamPcBox.TimeTSVH * TIME_ONE_MINUTES;
    Log_TSVH();
    fevent_enable(sEventAppPcBox, event);
    return 1;
}

static uint8_t fevent_queue_respond_immediately(uint8_t event)
{
/*-------------Phan hoi neu trong Queue con 1 ban tin-------------*/
    Struct_Queue_Type               *PtrQueueRespond;
    sDataQueueRespondPcBox          sQueueReadData;
    uint8_t KindSend_Prio = 0;
    
    if(qGet_Number_Items(&qRespondPcBox_Prio_1) == 0)
    {
        PtrQueueRespond = &qRespondPcBox_Prio_0;
        KindSend_Prio = 0;
    }
    else
    {
        PtrQueueRespond = &qRespondPcBox_Prio_1;
        KindSend_Prio = 1;
    }

    if(qGet_Number_Items(PtrQueueRespond) > 0)
    {   
        if(KindSend_Prio == 1)   
        {
            qQueue_Receive(PtrQueueRespond ,&sQueueReadData, 1);
            Transmit_PCBOX(sQueueReadData.aData_u8,sQueueReadData.Length);
        }
        else
        {
            if(sParamPcBox.StatePcBox == _STT_PCBOX_CONNECT)
            {
                qQueue_Receive(PtrQueueRespond ,&sQueueReadData, 1);
                Transmit_PCBOX(sQueueReadData.aData_u8,sQueueReadData.Length);
            }
        }
        
        fevent_enable(sEventAppPcBox, _EVENT_QUEUE_RESPOND_TIME);
        return 1;
    }
    fevent_enable(sEventAppPcBox, event);
    return 1;
}

static uint8_t fevent_queue_respond_time(uint8_t event)
{
/*---------------Phan hoi neu trong Queue co nhieu ban tin---------------*/
    Struct_Queue_Type               *PtrQueueRespond;
    sDataQueueRespondPcBox          sQueueReadData;
    uint8_t KindSend_Prio = 0;
    
    if(qGet_Number_Items(&qRespondPcBox_Prio_1) == 0)
    {
        PtrQueueRespond = &qRespondPcBox_Prio_0;
        KindSend_Prio = 0;
    }
    else
    {
        PtrQueueRespond = &qRespondPcBox_Prio_1;
        KindSend_Prio = 1;
    }
  
    if(qGet_Number_Items(PtrQueueRespond) == 0)
    {
        fevent_active(sEventAppPcBox, _EVENT_QUEUE_RESPOND_IMMEDIATELY);
        return 1;
    }
    else
    {
        if(KindSend_Prio == 1)   
        {
            qQueue_Receive(PtrQueueRespond ,&sQueueReadData, 1);
            Transmit_PCBOX(sQueueReadData.aData_u8,sQueueReadData.Length);
        }
        else
        {
            if(sParamPcBox.StatePcBox == _STT_PCBOX_CONNECT)
            {
                qQueue_Receive(PtrQueueRespond ,&sQueueReadData, 1);
                Transmit_PCBOX(sQueueReadData.aData_u8,sQueueReadData.Length);
            }
        }
        
        fevent_enable(sEventAppPcBox, _EVENT_QUEUE_RESPOND_TIME);
    }
    return 1;
}

static uint8_t fevent_handle_respond_error(uint8_t event)
{
    ResetDCU();
    
    return 1;
}

static uint8_t fevent_dcu_ping_pc_box(uint8_t event)
{
/*--------------------DCU Ping PcBox---------------------*/
  if(sParamPcBox.CountPingPcBox >= NUMBER_MAX_PING_PCBOX)  //Sau n lan khong ping thanh cong reset pc box
  {
    fevent_active(sEventAppPcBox, _EVENT_RESET_PC_BOX);
  }
  else
  {
    sParamPcBox.CountPingPcBox++;
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_DCU_PING_PC_BOX;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamPcBox.CountPingPcBox;
    
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
    
    fevent_enable(sEventAppPcBox, event);
  }
    return 1;
}

static uint8_t fevent_reset_pc_box(uint8_t event)
{
/*-----------------Reset PcBox------------------*/
    static uint8_t state_reset = 0;
    
    if(sParamPcBox.CountResetPcBox <= 1)    //Neu sau 2 lan reset pcbox khong co ket noi thi se ON pcbox o lan thu 3
    {
        if(state_reset == 0)
        {
            sStatusApp.Pcbox = _APP_BUSY;
            state_reset = 1;
            
            Save_StatePcBox(_STT_PCBOX_RESET);
            HAL_GPIO_WritePin(RESET_SW_PC_GPIO_Port, RESET_SW_PC_Pin, GPIO_PIN_SET);
            ControlRelay(RELAY_SCREEN, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_UNCTRL);
            
            fevent_enable(sEventAppPcBox, event);
            fevent_disable(sEventAppPcBox, _EVENT_DCU_PING_PC_BOX);
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"app_pc_box: Reset Pc Box", sizeof("app_pc_box: Reset Pc Box")-1);
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
            
            MX_USART2_UART_Init();
            Init_RX_Mode_Uart_PcBox();
        }
        else
        {   
            sParamPcBox.CountPingPcBox = 0;
            sParamPcBox.CountResetPcBox++;
            Save_StatePcBox(_STT_PCBOX_DISCONNECT);
            state_reset = 0;
            HAL_GPIO_WritePin(RESET_SW_PC_GPIO_Port, RESET_SW_PC_Pin, GPIO_PIN_RESET);
            fevent_enable(sEventAppPcBox, _EVENT_WAIT_RESET_PC_BOX);
        }
    }
    else
    {
            sParamPcBox.CountPingPcBox =0;
            sParamPcBox.CountResetPcBox=0;
            Save_StatePcBox(_STT_PCBOX_DISCONNECT);
            state_reset = 0;
            HAL_GPIO_WritePin(RESET_SW_PC_GPIO_Port, RESET_SW_PC_Pin, GPIO_PIN_RESET);
            ON_PcBox(3000);
            
            fevent_enable(sEventAppPcBox, _EVENT_WAIT_RESET_PC_BOX);
    }
    return 1;
}

static uint8_t fevent_wait_reset_pc_box(uint8_t event)
{
/*-----------------Wait Reset PcBox----------------*/
    fevent_active(sEventAppPcBox, _EVENT_DCU_PING_PC_BOX);
    sStatusApp.Pcbox = _APP_FREE;
    sParamPcBox.CountPingPcBox = 0;
    return 1;
}

static uint8_t fevent_on_off_cycle_pc_box(uint8_t event)
{
/*------------------On Off Cycle Pc Box-----------------*/
    if(sRTC.year > 20)
    {
        if(sParamPcBox.StatePcBox != _STT_PCBOX_SLEEP && sCycleOnOffPcBox.State == _ON_CYCLE)
        {
            if(Check_RealTime_OFF_PcBox(sRTC.hour, sRTC.min) == 1)
            {
//                OFF_PcBox(2000);
                if(sParamPcBox.CountPingPcBox <= NUMBER_MAX_PING_PCBOX/2 && sParamPcBox.StatePcBox == _STT_PCBOX_CONNECT)
                {
                    sRespPcBox.Length_u16 = 0;
                    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_QUESTION_OFF_PCBOX;
                    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
                    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
                    
                    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
                }
            }
        }
    
        if(sParamPcBox.StatePcBox == _STT_PCBOX_SLEEP && sCycleOnOffPcBox.State == _ON_CYCLE)
        {
            if(Check_RealTime_ON_PcBox(sRTC.hour, sRTC.min) == 1)
            {
                ON_PcBox(1000);
            }
        }
    }
    else
    {
        if(sParamPcBox.StatePcBox == _STT_PCBOX_SLEEP)
        {
            ON_PcBox(1000);
        }
    }
    
    if(sParamPcBox.StatePcBox == _STT_PCBOX_SLEEP)
    {
        fevent_enable(sEventAppPcBox, _EVENT_HANDLE_RESPOND_ERROR);
        if(sCycleOnOffPcBox.State == _OFF_CYCLE)
        {
//            ON_PcBox(1000);
        }
    }
    
    fevent_enable(sEventAppPcBox, event);
    return 1;
}

static uint8_t fevent_refresh_dcu(uint8_t event)
{
/*----------------Kiem tra dieu kien va refresh DCU----------------*/
    if(sStatusApp.Delivery == _APP_FREE && sStatusApp.Door == _APP_FREE 
       && sStatusApp.Temperature == _APP_FREE && sStatusApp.RL_Warm == _APP_FREE 
       && sStatusApp.Pcbox == _APP_FREE)
    {
        if(qGet_Number_Items(&qRespondPcBox_Prio_0) == 0)
            ResetDCU();
    }
    
    fevent_active(sEventAppPcBox, event);
    return 1;
}

static uint8_t fevent_get_real_time_dcu(uint8_t event)
{
    static uint8_t state_get_time = 0;
    static uint8_t get_time_cycle = 0;
    
    if(sRTC.year < 20)
    {
        state_get_time = 0;
    }
    
    if(get_time_cycle == 0)
    {
        if(sRTC.hour == 9 || sRTC.hour == 21)
        {
            state_get_time = 0;
            get_time_cycle = 1;
        }
    }
    else
    {
        if(sRTC.hour == 11 || sRTC.hour == 23)
            get_time_cycle = 0;
    }
    
    if(state_get_time == 0)
    {
        sRespPcBox.Length_u16 = 0;
        sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_DCU_GET_REAL_TIME;
        sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
        sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
        
        Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
        state_get_time = 1;
    }
    
    fevent_enable(sEventAppPcBox, event);
    return 1;
}
/*====================== Handle PcBox =====================*/
void  OFF_PcBox(uint32_t TimeOnRelay_ms)
{
    sParamPcBox.CountPingPcBox = 0;
    sParamPcBox.CountResetPcBox = 0;
    
    sStatusApp.Pcbox = _APP_BUSY;
    ControlRelay(RELAY_SCREEN, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_CTRL);
    ControlRelay(RELAY_LAMP, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_CTRL);
    
    MX_USART2_UART_Init();
    Init_RX_Mode_Uart_PcBox();
    OnOffPcBox(TimeOnRelay_ms);
    Save_StatePcBox(_STT_PCBOX_SLEEP);
    
    HAL_GPIO_WritePin(RESET_SW_PC_GPIO_Port, RESET_SW_PC_Pin, GPIO_PIN_RESET);
    
    fevent_disable(sEventAppPcBox, _EVENT_DCU_PING_PC_BOX);
    fevent_disable(sEventAppPcBox, _EVENT_RESET_PC_BOX);
    fevent_disable(sEventAppPcBox, _EVENT_WAIT_RESET_PC_BOX);
    
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"app_pc_box: Sleep PcBox", sizeof("app_pc_box: Sleep PcBox")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
}

void ON_PcBox(uint32_t TimeOnRelay_ms)
{
    MX_USART2_UART_Init();
    Init_RX_Mode_Uart_PcBox();
    OnOffPcBox(TimeOnRelay_ms);
    
    Save_StatePcBox(_STT_PCBOX_DISCONNECT);
    
//    ControlRelay(RELAY_SCREEN, ON_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_CTRL);
    
    fevent_enable(sEventAppPcBox, _EVENT_WAIT_RESET_PC_BOX);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"app_pc_box: On PcBox", sizeof("app_pc_box: On PcBox")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
}

/*
    @brief  Truyen Data len PcBox co Debug
*/
void Transmit_PCBOX(uint8_t aData[],uint8_t length)
{
    Respond_PcBox(aData, length);
    AppPcBox_Debug(aData, length , _TRANS_PCBOX);
    fevent_enable(sEventAppPcBox, _EVENT_HANDLE_RESPOND_ERROR);
}

void Packing_Respond_PcBox(uint8_t aData[], uint16_t Length)
{
    uint16_t TempCrc = 0;
    Calculator_Crc_U16(&TempCrc, aData, Length);
    aData[Length++] = TempCrc;
    aData[Length++] = TempCrc << 8;
//    if(aData[0] == OBIS_DCU_PING_PC_BOX || aData[0] == OBIS_RESET_DCU || \
//       aData[0] == OBIS_ON_GOING_PUSH   || aData[0] == OBIS_COMPLETE_PUSH || \
//       aData[0] == OBIS_DCU_GET_REAL_TIME)   
//    {
//        if(aData[0] == OBIS_ON_GOING_PUSH   || aData[0] == OBIS_COMPLETE_PUSH)
//        Write_Queue_Repond_PcBox(&qRespondPcBox_Prio_1, aData, Length, _TYPE_SEND_TO_END);
//    }
//    else    
//        Write_Queue_Repond_PcBox(&qRespondPcBox_Prio_0, aData, Length, _TYPE_SEND_TO_END);
    
    switch(aData[0])
    {
        case OBIS_ON_GOING_PUSH:
        case OBIS_COMPLETE_PUSH:
          Write_Queue_Repond_PcBox(&qRespondPcBox_Prio_1, aData, Length, _TYPE_SEND_TO_HEAD);
          break;
          
        case OBIS_TSVH_PC_BOX:
        case OBIS_WARNING_VIB_SENSOR:
        case OBIS_WARNING_DOOR_SENSOR:
        case OBIS_WARNING_ERROR_TEMP:
          Write_Queue_Repond_PcBox(&qRespondPcBox_Prio_0, aData, Length, _TYPE_SEND_TO_END);
          break;
          
        default:
          Write_Queue_Repond_PcBox(&qRespondPcBox_Prio_1, aData, Length, _TYPE_SEND_TO_END);
          break;
    }
}

/*
    @brief  Viet vao Queue doi gui lenh len PcBox
*/
void Write_Queue_Repond_PcBox(Struct_Queue_Type *qQueueSend, uint8_t aData[], uint8_t Length, uint8_t KindSend)
{
    sDataQueueRespondPcBox  sQueueWrite;
    sDataQueueRespondPcBox  sQueueRead;

    sQueueWrite.Length   = Length;
    for(uint8_t i=0; i<Length; i++)
        sQueueWrite.aData_u8[i] = aData[i];
    
    if(qGet_Number_Items(qQueueSend) >= NUMBER_ITEM_QUEUE - 1)
        qQueue_Receive(qQueueSend ,&sQueueRead, 1);
    
    qQueue_Send(qQueueSend, (sDataQueueRespondPcBox*)&sQueueWrite, KindSend);
}

void RespondPcBox_String(uint8_t *aData, uint16_t Length)
{
    sRespPcBox.Length_u16 = 0;
    if(Length < NUMBER_MAX_BUFFER)
    {
        while(sRespPcBox.Length_u16 < Length)
        {
            sRespPcBox.Data_a8[sRespPcBox.Length_u16] = *(aData + sRespPcBox.Length_u16);
            sRespPcBox.Length_u16++;
        }
        Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
    }
}

uint8_t  Check_RealTime_OFF_PcBox(uint8_t Hours, uint8_t Minutes)
{
    uint16_t countTimeOff = 0;
    uint16_t countTimeOn = 0;
    uint16_t countTimeRTC = 0;
    
    countTimeOff = sCycleOnOffPcBox.HoursOFF*MINUTES_OF_HOURS + sCycleOnOffPcBox.MinutesOFF;
    countTimeOn = sCycleOnOffPcBox.HoursON*MINUTES_OF_HOURS + sCycleOnOffPcBox.MinutesON;
    countTimeRTC = Hours*MINUTES_OF_HOURS + Minutes;
    
    if(countTimeOn >= 1) 
        countTimeOn -=1;
    else 
        countTimeOn = MINUTES_OF_DAY - (1 - countTimeOn);
    
    if(countTimeOff >= countTimeOn)
    {
        if(countTimeRTC >= countTimeOff || countTimeRTC < countTimeOn)
          return 1;
    }
    else
    {
        if(countTimeRTC >= countTimeOff && countTimeRTC < countTimeOn)
          return 1;
    }
      
    return 0;
}

uint8_t     Check_RealTime_ON_PcBox(uint8_t Hours, uint8_t Minutes)
{
    uint16_t countTimeOff = 0;
    uint16_t countTimeOn = 0;
    uint16_t countTimeRTC = 0;
    
    countTimeOff = sCycleOnOffPcBox.HoursOFF*MINUTES_OF_HOURS + sCycleOnOffPcBox.MinutesOFF;
    countTimeOn = sCycleOnOffPcBox.HoursON*MINUTES_OF_HOURS + sCycleOnOffPcBox.MinutesON;
    countTimeRTC = Hours*MINUTES_OF_HOURS + Minutes;
    
    if(countTimeOff > countTimeOn)
    {
        if(countTimeRTC < countTimeOff && countTimeRTC >= countTimeOn)
          return 1;
    }
    else
    {
        if(countTimeRTC < countTimeOff || countTimeRTC >= countTimeOn)
          return 1;
    }
      
    return 0;
}
/*================= Function Handle ================*/
/*
    @brief  Log TSVH
*/
void Log_TSVH(void)
{
//    uint8_t length = 0;
//    uint16_t TempCrc = 0;
    
/*=============== Log ===============*/
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_TSVH_PC_BOX;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x00;
    
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sRTC.year;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sRTC.month;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sRTC.date;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sRTC.hour;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sRTC.min;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sRTC.sec;
    
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.PowerPresent;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sStatusRelay.Screen;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sStatusRelay.Lamp;
//    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sStatusRelay.Warm;
//    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sStatusRelay.FridgeHeat;
//    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sStatusRelay.FridgeCool;
    
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sTemp_Crtl_Fridge.TempSetup >> 8;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sTemp_Crtl_Fridge.TempSetup;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sTemperature.Value >> 8;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sTemperature.Value;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = DEFAULT_TEMP_SCALE;

    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.Voltage >> 8;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.Voltage;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.Current >> 8;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.Current;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.ScaleVolCur;
    
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.Power >> 8;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.Power;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.Energy >> 24;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.Energy >> 16;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.Energy >> 8;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.Energy;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.ScalePowEne;
    
//    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sTimeCycleWarm.Run;
//    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sTimeCycleWarm.Wait;
    
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sCycleOnOffPcBox.State;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sCycleOnOffPcBox.HoursOFF;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sCycleOnOffPcBox.MinutesOFF;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sCycleOnOffPcBox.HoursON;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sCycleOnOffPcBox.MinutesON;
    
    sRespPcBox.Data_a8[1] = sRespPcBox.Length_u16 - 2;
    
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
}

void ResetDCU(void)
{
    uint8_t aData[5]={0};
    uint8_t length = 0;
    uint16_t TempCrc = 0;
    aData[length++] = OBIS_RESET_DCU;
    aData[length++] = 0x01;
    aData[length++] = BEFORE_RESET_DCU;
    
    Calculator_Crc_U16(&TempCrc, aData, length);
    aData[length++] = TempCrc;
    aData[length++] = TempCrc << 8;
    Transmit_PCBOX(aData, length);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"OFF_MCU\r\n", sizeof("OFF_MCU\r\n")-1); 
//    HAL_Delay(500);
    Reset_Chip();
}

void Reset_WDG_DCU(void)
{
    //Reset WDG Hardware
    HAL_GPIO_WritePin(Toggle_Reset_GPIO_Port, Toggle_Reset_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(Toggle_Reset_GPIO_Port, Toggle_Reset_Pin, GPIO_PIN_RESET);
    
    //Reset WDG Firmware
    HAL_IWDG_Refresh(&hiwdg); 
}

/*---------------------------Save and Init ID DCU-----------------------*/
uint8_t Save_DcuID(uint8_t *aData, uint16_t Length)
{
    if(Length <= MAX_LENGTH_DCU_ID)
    {
        uint8_t write[MAX_LENGTH_DCU_ID+2] = {0};
        
        write[0]= BYTE_TEMP_FIRST_EXFLASH;
        write[1]= Length;
        
        sModem.sDCU_id.Length_u16 = 0;
        UTIL_MEM_set(sModem.sDCU_id.Data_a8, 0x00, MAX_LENGTH_DCU_ID);
        for(uint8_t i = 0; i < Length; i++)
        {
            if(i < MAX_LENGTH_DCU_ID)
            {
                write[i+2] = aData[i];
                sModem.sDCU_id.Data_a8[sModem.sDCU_id.Length_u16++] = aData[i];
            }
            else break;
        }
        
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_MAIN_ID);
        eFlash_S25FL_BufferWrite(write, EX_FLASH_ADDR_MAIN_ID, Length+2);
        return 1;
    }
    return 0;
}
/*----------------------Save and Init thoi gian gui TSVH--------------------*/
/*
    @brief  Cai dat thoi gian gui thong so van hanh (min)
*/
uint8_t Save_TimeTSVH(uint8_t Time)
{
    if(Time == 0) Time = 1;
    sParamPcBox.TimeTSVH = Time;
    
    uint8_t write[2]={0x00};
    
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_TIME_LOG_TSVH;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamPcBox.TimeTSVH ;

    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
    
    write[0] = BYTE_TEMP_FIRST_EXFLASH;
    write[1] = sParamPcBox.TimeTSVH ;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_FREQ_TSVH);
    eFlash_S25FL_BufferWrite(write, EX_FLASH_ADDR_FREQ_TSVH, 2);
    sEventAppPcBox[_EVENT_PC_BOX_LOG_TSVH].e_period = sParamPcBox.TimeTSVH * TIME_ONE_MINUTES;
    fevent_enable(sEventAppPcBox, _EVENT_PC_BOX_LOG_TSVH);
    return 1;
}

void Init_TimePcBox(void)
{
    uint8_t read[2] = {0};
    eFlash_S25FL_BufferRead(read, EX_FLASH_ADDR_FREQ_TSVH , 2);
    if( read[0] == BYTE_TEMP_FIRST_EXFLASH)
        sParamPcBox.TimeTSVH  = read[1];
    else
        sParamPcBox.TimeTSVH  = 1;
}

/*---------------------Save and Init Using Crc------------------*/
uint8_t Save_UsingCrc(uint8_t UsingCrc)
{
    if(UsingCrc <= 1)
    {
        uint8_t write[2]={0};
        sParamPcBox.UsingCrc = UsingCrc;
        write[0] = BYTE_TEMP_FIRST_EXFLASH;
        write[1] = sParamPcBox.UsingCrc;
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_PCBOX_USING_CRC);
        eFlash_S25FL_BufferWrite(write, EX_FLASH_ADDR_PCBOX_USING_CRC, 2);
        return 1;
    }
    return 0;
}

void Init_UsingCrc()
{
    uint8_t read[2] = {0};
    eFlash_S25FL_BufferRead(read, EX_FLASH_ADDR_PCBOX_USING_CRC , 2);
    if(read[0] == BYTE_TEMP_FIRST_EXFLASH)
    {
        if(read[1] == 0x01) sParamPcBox.UsingCrc = _USING_CRC;
        else if(read[1] == 0x00) sParamPcBox.UsingCrc = _UNUSING_CRC;
    }
    else 
        sParamPcBox.UsingCrc = _UNUSING_CRC;
}
/*-------------------Save and Init Cycle On Off PcBox-----------------*/
uint8_t Save_CycleOnOffPcBox(uint8_t State, uint8_t HoursOFF, uint8_t MinutesOFF, uint8_t HoursON, uint8_t MinutesON)
{
    uint16_t countTimeOff = 0;
    uint16_t countTimeOn = 0;
    uint8_t ResultCal = 0;
    
    countTimeOff = HoursOFF*MINUTES_OF_HOURS + MinutesOFF;
    countTimeOn  = HoursON*MINUTES_OF_HOURS + MinutesON;

    if(State<=1 && HoursOFF<24 && MinutesOFF<60 && HoursON<24 && MinutesON<60)
    {
        if(countTimeOff > countTimeOn)
        {
            if(countTimeOff - countTimeOn >= 3 && (MINUTES_OF_DAY - countTimeOff) + countTimeOn >= 3) 
              ResultCal = 1;
        }
        else
        {
            if(countTimeOn - countTimeOff >= 3 && (MINUTES_OF_DAY - countTimeOn) + countTimeOff >= 3) 
              ResultCal = 1;
        }
      
        if(ResultCal == 1)
        {
            sCycleOnOffPcBox.State      = State;
            sCycleOnOffPcBox.HoursOFF   = HoursOFF;
            sCycleOnOffPcBox.MinutesOFF = MinutesOFF;
            sCycleOnOffPcBox.HoursON    = HoursON;
            sCycleOnOffPcBox.MinutesON  = MinutesON; 
          
            uint8_t write[6]={0};
            write[0] = BYTE_TEMP_FIRST_EXFLASH;
            write[1] = sCycleOnOffPcBox.State;
            write[2] = sCycleOnOffPcBox.HoursOFF;
            write[3] = sCycleOnOffPcBox.MinutesOFF;
            write[4] = sCycleOnOffPcBox.HoursON;
            write[5] = sCycleOnOffPcBox.MinutesON;
            eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_TIME_ON_OFF_PCBOX);
            eFlash_S25FL_BufferWrite(write, EX_FLASH_ADDR_TIME_ON_OFF_PCBOX, 6);
            return 1;
        }
    }
    return 0;
}

void Init_CycleOnOffPcBox(void)
{
    uint8_t read[6] = {0};
    eFlash_S25FL_BufferRead(read, EX_FLASH_ADDR_TIME_ON_OFF_PCBOX , 6);
    if(read[0] == BYTE_TEMP_FIRST_EXFLASH)
    {
        if(read[1]<= 1)
        {
            if(read[2]<24 && read[3]<60)
            {
                if(read[4]<24 && read[5]<60)
                {
                    sCycleOnOffPcBox.State      = read[1];
                    sCycleOnOffPcBox.HoursOFF   = read[2];
                    sCycleOnOffPcBox.MinutesOFF = read[3];
                    sCycleOnOffPcBox.HoursON    = read[4];
                    sCycleOnOffPcBox.MinutesON  = read[5];
                }
            }
        }
    }
    else
    {
        sCycleOnOffPcBox.State      = 0;
        sCycleOnOffPcBox.HoursOFF   = DEFAULT_OFF_PCBOX_HOURS;
        sCycleOnOffPcBox.MinutesOFF = DEFAULT_OFF_PCBOX_MINUTES;
        sCycleOnOffPcBox.HoursON    = DEFAULT_ON_PCBOX_HOURS;
        sCycleOnOffPcBox.MinutesON  = DEFAULT_ON_PCBOX_MINUTES;
    }
}
/*-------------------- Save and Init State PcBox ---------------------*/
uint8_t Save_StatePcBox(uint8_t State)
{
    if(sParamPcBox.StatePcBox != State)
    {
        if(State < _STT_PCBOX_END)
        {
            uint8_t write[2]={0};
            sParamPcBox.StatePcBox = State;
            write[0] = BYTE_TEMP_FIRST_EXFLASH;
            write[1] = sParamPcBox.StatePcBox;
            eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_STATE_PCBOX);
            eFlash_S25FL_BufferWrite(write, EX_FLASH_ADDR_STATE_PCBOX, 2);
            return 1;
        }
    }
    return 1;
}

void Init_StatePcBox()
{
    uint8_t read[2] = {0};
    eFlash_S25FL_BufferRead(read, EX_FLASH_ADDR_STATE_PCBOX , 2);
    if(read[0] == BYTE_TEMP_FIRST_EXFLASH)
    {
        if(read[1] < _STT_PCBOX_END) 
            sParamPcBox.StatePcBox = read[1];
    }
    
//    if(sParamPcBox.StatePcBox == _STT_PCBOX_CONNECT)
//    {
//        ControlRelay(RELAY_SCREEN, ON_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_CTRL);
//    }
//    else
//    {
//        ControlRelay(RELAY_SCREEN, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_CTRL);
//    }
}

/*---------------------------Handle Queue---------------------------*/
void Init_QueuePcBox(void)
{
    qQueue_Create(&qRespondPcBox_Prio_0, NUMBER_ITEM_QUEUE, sizeof(sDataQueueRespondPcBox),(sDataQueueRespondPcBox*)&sQueueRespondPcBox_Prio_0);
    qQueue_Create(&qRespondPcBox_Prio_1, NUMBER_ITEM_QUEUE, sizeof(sDataQueueRespondPcBox),(sDataQueueRespondPcBox*)&sQueueRespondPcBox_Prio_1);
}

/*----------------------------Handle Timer--------------------------*/
void Init_Timer(void)
{
    UTIL_TIMER_Create (&TimerOnOffPcBox,  0xFFFFFFFFU, UTIL_TIMER_ONESHOT, Cb_Timer_Event_OnOffPcBox, NULL);
}

static void Cb_Timer_Event_OnOffPcBox(void *context)
{
    HAL_GPIO_WritePin(ON_OFF_SW_PC_GPIO_Port, ON_OFF_SW_PC_Pin, GPIO_PIN_RESET);
//    UTIL_TIMER_Start (&TimerOnOffPcBox);
}

void OnOffPcBox(uint16_t time)
{
    HAL_GPIO_WritePin(ON_OFF_SW_PC_GPIO_Port, ON_OFF_SW_PC_Pin, GPIO_PIN_SET);
    UTIL_TIMER_SetPeriod (&TimerOnOffPcBox, time);   //Set sFreqInfor.FreqSendUnitMin_u32 * 60000
	UTIL_TIMER_Start (&TimerOnOffPcBox);
}

/*===========================Handle Task==========================*/
void Init_AppPcBox(void)
{
    Init_StatePcBox();
    Init_Timer();
    Init_TimePcBox();
    Init_UsingCrc();
    Init_QueuePcBox();
    Init_CycleOnOffPcBox();
}

uint8_t AppPcBox_Task(void)
{
    uint8_t i = 0;
    uint8_t Result = false;
    
    for(i = 0; i < _EVENT_PC_BOX_END ; i++)
    {
        if(sEventAppPcBox[i].e_status == 1)
        {
            Result = true;
            if(sEventAppPcBox[i].e_systick == 0 || 
               ((HAL_GetTick() - sEventAppPcBox[i].e_systick) >= sEventAppPcBox[i].e_period))
            {
                sEventAppPcBox[i].e_status = 0;  //Disable event
				sEventAppPcBox[i].e_systick = HAL_GetTick();
				sEventAppPcBox[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}

/*
    @brief  Debug
*/
void AppPcBox_Debug(uint8_t aData[], uint8_t length, uint8_t TransRecv)
{
#ifdef USING_APP_PC_BOX_DEBUG
    uint8_t array[NUMBER_MAX_BUFFER*2]={0};
    sData   sArray=
    {
        .Data_a8    = aData,
        .Length_u16 = length,
    };
    uint8_t check[]="app_pc_box: Recv: ";
    if(TransRecv == _RECV_PCBOX)
    {
        UTIL_Printf(DBLEVEL_L, check, sizeof("app_pc_box: Recv: ")-1);
    }
    else
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"app_pc_box: Trans: ", sizeof("app_pc_box: Trans: ")-1);
    }
    sData sSource=
    {
        .Data_a8 = array,
    };
    
    if(sArray.Length_u16 < NUMBER_MAX_BUFFER*2)
    {
        Convert_Hex_To_String_Hex(&sSource, &sArray);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)sSource.Data_a8, sSource.Length_u16);
    }
    else
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"Data Large", sizeof("Data Large")-1);
    }

    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
#endif
}
