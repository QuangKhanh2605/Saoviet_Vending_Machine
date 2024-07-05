#include "user_app_pc_box.h"
#include "user_comm_vending_machine.h"
#include "user_inc_vending_machine.h"
#include "user_external_flash.h"
/*============== Function Static =========*/
static uint8_t fevent_pcbox_entry(uint8_t event);
static uint8_t fevent_pcbox_receive_handle(uint8_t event);
static uint8_t fevent_pcbox_complete_receive(uint8_t event);
static uint8_t fevent_pcbox_log_tsvh(uint8_t event);
static uint8_t fevent_pcbox_set_dcu_ID(uint8_t event);
static uint8_t fevent_pcbox_get_dcu_ID(uint8_t event);
static uint8_t fevent_wdg_stm32f4(uint8_t event);
static uint8_t fevent_reset_dcu(uint8_t event);
static uint8_t fevent_queue_respond_immediately(uint8_t event);
static uint8_t fevent_queue_respond_time(uint8_t event);
static uint8_t fevent_dcu_ping_pc_box(uint8_t event);
static uint8_t fevent_reset_pc_box(uint8_t event);
static uint8_t fevent_wait_reset_pc_box(uint8_t event);
static uint8_t fevent_refresh_dcu(uint8_t event);
/*=============== Struct =================*/
sEvent_struct               sEventAppPcBox[]=
{
  {_EVENT_PC_BOX_ENTRY,                 1, 5, TIME_ON_DCU,              fevent_pcbox_entry},
  {_EVENT_PC_BOX_RECEIVE_HANDLE,        0, 0, 5,                        fevent_pcbox_receive_handle},
  {_EVENT_PC_BOX_COMPLETE_RECEIVE,      0, 0, 5,                        fevent_pcbox_complete_receive},
  {_EVENT_PC_BOX_LOG_TSVH,              1, 5, TIME_ENTRY,               fevent_pcbox_log_tsvh},
  
  {_EVENT_PC_BOX_SET_DCU_ID,            0, 0, 5,                        fevent_pcbox_set_dcu_ID},
  {_EVENT_PC_BOX_GET_DCU_ID,            0, 0, 5,                        fevent_pcbox_get_dcu_ID},
  
  {_EVENT_WDG_STM32F4,                  1, 0, TIME_RESET_WDG,           fevent_wdg_stm32f4},
  {_EVENT_RESET_DCU,                    0, 5, 2000,                     fevent_reset_dcu},
  
  {_EVENT_QUEUE_RESPOND_IMMEDIATELY,    1, 0, 5,                        fevent_queue_respond_immediately},
  {_EVENT_QUEUE_RESPOND_TIME,           0, 0, TIME_RESPOND_PC_BOX,      fevent_queue_respond_time},
  
  {_EVENT_DCU_PING_PC_BOX,              0, 5, TIME_PING_PCBOX,          fevent_dcu_ping_pc_box},
  {_EVENT_RESET_PC_BOX,                 0, 5, TIME_RESET_PC_BOX,        fevent_reset_pc_box},
  {_EVENT_WAIT_RESET_PC_BOX,            0, 5, TIME_WAIT_RESET_PC_BOX,   fevent_wait_reset_pc_box},
  
  {_EVENT_REFRESH_DCU,                  1, 5, TIME_REFRESH_DCU,         fevent_refresh_dcu},
};

uint8_t aDCU_ID[MAX_LENGTH_DCU_ID] = DEVICE_ID;
sData   sDCU_ID=
{
    .Data_a8 = &aDCU_ID[0], 
    .Length_u16 = 14,
};
sDataQueueRespondPcBox          sQueueRespondPcBox[100];
sDataQueueRespondPcBox          sQueueReadData;
Struct_Queue_Type               qRespondPcBox;

StructStatusApp                 sStatusApp = {FREE};
StructParamPcBox                sParamPcBox=
{
    .CountResetPcBox = 0,
    .TimeResetPcBox  = TIME_WAIT_RESET_PC_BOX/TIME_ONE_MINUTES,
    .TimeTSVH        = TIME_SEND_TSVH/TIME_ONE_MINUTES,
    .ConnectPcBox    = _DISCONNECT_PCBOX,
    .UsingCrc        = _UNUSING_CRC,
};

uint8_t aBufferRespondPcBox[NUMBER_MAX_BUFFER];
sData   sRespPcBox=
{
    .Data_a8    = aBufferRespondPcBox,
    .Length_u16 = 0,
};
/*============== Function Static ===========*/
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
    fevent_active(sEventAppPcBox, _EVENT_DCU_PING_PC_BOX);
    fevent_enable(sEventAppPcBox, _EVENT_REFRESH_DCU);
    
    return 1;
}

static uint8_t fevent_wdg_stm32f4(uint8_t event)
{
/*--------------Reset WDG-------------*/
    HAL_GPIO_WritePin(Toggle_Reset_GPIO_Port, Toggle_Reset_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(Toggle_Reset_GPIO_Port, Toggle_Reset_Pin, GPIO_PIN_RESET);
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
        {
            countBuffer_uart = sUartPcBox.Length_u16;
        }
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
    
    AppPcBox_Debug(sUartPcBox.Data_a8, sUartPcBox.Length_u16 , _RECV_PCBOX);

    Crc_Recv = (sUartPcBox.Data_a8[sUartPcBox.Length_u16-1] << 8) |
               (sUartPcBox.Data_a8[sUartPcBox.Length_u16-2]);
    Calculator_Crc_U16(&Crc_Check, sUartPcBox.Data_a8, sUartPcBox.Length_u16 - 2);
    
    if(Crc_Check == Crc_Recv)   UsingCrc = 1;
    else if(sParamPcBox.UsingCrc == _UNUSING_CRC) UsingCrc = 1;
    
    if(UsingCrc == 1)
    {
        if(sParamPcBox.ConnectPcBox == _DISCONNECT_PCBOX)
        {
            fevent_active(sEventAppPcBox, _EVENT_WAIT_RESET_PC_BOX);
        }
        while((pos + 2) <= sUartPcBox.Length_u16 - 2) //co 2 byte CRC cuoi cung
        {
            switch(sUartPcBox.Data_a8[pos++])
            {
                case OBIS_PC_BOX_FIX_MOTOR:     //Obis Fix Motor
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
//                    if(sPushMotor.StatePush == COMPLETE_PUSH)
//                    {
//                        sStatusApp.Motor      = BUSY;
//                          
//                        sPushMotor.State      = FIX_MOTOR;
//                        sPushMotor.StatePush  = ON_GOING_PUSH;
//                        sPushMotor.Pos        = sUartPcBox.Data_a8[pos];
//                        sPushMotor.SumHandle  = 1;
//                        sPushMotor.PulseCount = 1;
//                        fevent_active(sEventAppMotor, _EVENT_MOTOR_ENTRY);
//                    }
                    Fix_Motor(sUartPcBox.Data_a8[pos]);
                    pos +=Length_Data;
                  }
                  break;
              
                case OBIS_PC_BOX_CTRL_MOTOR:    //Obis Push Motor
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x02 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
//                    if(sPushMotor.StatePush == COMPLETE_PUSH)
//                    {
//                        Respond = true;
//                        sStatusApp.Motor      = BUSY;
//                        
//                        sPushMotor.State      = PUSH_MOTOR;
//                        sPushMotor.StatePush  = ON_GOING_PUSH;
//                        sPushMotor.Pos        = sUartPcBox.Data_a8[pos];
//                        sPushMotor.SumHandle  = sUartPcBox.Data_a8[pos+1];
//                        sPushMotor.PulseCount = 0;
//                        fevent_active(sEventAppMotor, _EVENT_MOTOR_ENTRY);
//                    }
                    Push_Motor(sUartPcBox.Data_a8[pos], sUartPcBox.Data_a8[pos+1]);
                    pos +=Length_Data;
                  }
                  break;

                case OBIS_RESET_DCU:    //Obis reset Dcu
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {              
                        if(sUartPcBox.Data_a8[pos] == 0x01)
                        {
                            ResetDCU();
                        }
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
                        {
                            fevent_active(sEventAppRelay,_EVENT_RELAY_WARM_OFF);
                        }
                        else if(sUartPcBox.Data_a8[pos] == ON_RELAY)
                        {
                            if(sElectric.PowerPresent != POWER_OFF)
                            {
                                OnRelay_Warm(sTimeCycleWarm.Run);
                                fevent_active(sEventAppRelay,_EVENT_RELAY_WARM_ON);
                            }
                            else
                            {
                                fevent_active(sEventAppRelay,_EVENT_RELAY_WARM_OFF);
                            }
                        }
                        pos +=Length_Data;
                      }
                  }
                  break; 
                  
                case OBIS_SETUP_TEMP:   //Obis cai dat nhiet do
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x03 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                        sTemp_Thresh_Recv.Value  =  sUartPcBox.Data_a8[pos++];
                        sTemp_Thresh_Recv.Value  =  sTemp_Thresh_Recv.Value << 8;
                        sTemp_Thresh_Recv.Value |=  sUartPcBox.Data_a8[pos++];
                        sTemp_Thresh_Recv.Scale  =  sUartPcBox.Data_a8[pos++];
                        fevent_active(sEventAppTemperature , _EVENT_TEMP_SET_SETUPTEMP);
                        pos +=Length_Data;
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
                        pos +=Length_Data;
                  }
                  break;
                  
                case OBIS_GET_DCU_ID:   //Obis get seri DCU
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if((pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                        fevent_active(sEventAppPcBox , _EVENT_PC_BOX_GET_DCU_ID);
                        pos +=Length_Data;
                  }
                  break;         
                  
                  
                case OBIS_SET_DCU_ID:   //Obis cai dat seri DCU
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if((pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                        sDCU_ID.Length_u16 = 0;
                        while(Length_Data > 0)
                        {
                            if(sDCU_ID.Length_u16 < MAX_LENGTH_DCU_ID)
                            {
                              sDCU_ID.Data_a8[sDCU_ID.Length_u16++] = sUartPcBox.Data_a8[pos++];
                            }
                            Length_Data--;
                        }
                        fevent_active(sEventAppPcBox , _EVENT_PC_BOX_SET_DCU_ID);
                        pos +=Length_Data;
                  }
                  break;
                  
                case OBIS_DCU_PING_PC_BOX:  //Obis Ping PcBox
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x01)
                      {
                        sParamPcBox.CountResetPcBox = 0;
                      }
                      pos +=Length_Data;
                  }
                  
                case OBIS_TIME_LOG_TSVH:    //Obis set time TSVH
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      Set_TimeTSVH(sUartPcBox.Data_a8[pos]);
                      pos +=Length_Data;
                  }
                  
                case OBIS_TIME_WARM_RUN:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      PcBox_Setup_Time_Warm_Run(sUartPcBox.Data_a8[pos]);
                      pos +=Length_Data;
                  }
                  
                case OBIS_TIME_WARM_WAIT:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      PcBox_Setup_Time_Warm_Wait(sUartPcBox.Data_a8[pos]);
                      pos +=Length_Data;
                  }
                  
                default:
                  break;
            }
        }
        if(Respond == true)
        {
            Write_Queue_Repond_PcBox((uint8_t*)"OK",2);
        }
    }
    else
    {
        Write_Queue_Repond_PcBox((uint8_t*)"ERROR",5);
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

static uint8_t fevent_pcbox_set_dcu_ID(uint8_t event)
{
/*---------------Cai dat ID DCU------------------*/
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++]= DEFAULT_READ_EXFLASH;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++]= sDCU_ID.Length_u16;
   
    for(uint8_t i=0; i<sDCU_ID.Length_u16; i++)
    {
        sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sDCU_ID.Data_a8[i];
    }
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_MAIN_ID);
    eFlash_S25FL_BufferWrite(sRespPcBox.Data_a8, EX_FLASH_ADDR_MAIN_ID, sDCU_ID.Length_u16+2);
    
    sRespPcBox.Data_a8[0] = OBIS_SET_DCU_ID;
    
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
    return 1;
}

static uint8_t fevent_pcbox_get_dcu_ID(uint8_t event)
{
/*-----------------Phan hoi seri DCU cho PcBox--------------*/
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++]= OBIS_GET_DCU_ID;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++]= sDCU_ID.Length_u16;
   
    for(uint8_t i=0; i<sDCU_ID.Length_u16; i++)
    {
        sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sDCU_ID.Data_a8[i];
    }

    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
    return 1;
}

static uint8_t fevent_reset_dcu(uint8_t event)
{
/*-----------------Kiem tra dieu kien va reset DCU---------------*/
    if(qGet_Number_Items(&qRespondPcBox) == 0)
    {
        ResetDCU();
    }
    fevent_active(sEventAppPcBox, event);
    return 1;
}

static uint8_t fevent_queue_respond_immediately(uint8_t event)
{
/*-------------Phan hoi neu trong Queue con 1 ban tin-------------*/
    if(qGet_Number_Items(&qRespondPcBox) > 0)
    {
        if(sParamPcBox.ConnectPcBox == _CONNECT_PCBOX)
        {
            qQueue_Receive(&qRespondPcBox ,&sQueueReadData, 1);
            Transmit_PCBOX(sQueueReadData.aData_u8,sQueueReadData.Length);
        }
        
        fevent_enable(sEventAppPcBox, _EVENT_QUEUE_RESPOND_TIME);
        return 1;
    }
    fevent_enable(sEventAppPcBox, event);
    return 1;
}

static uint8_t fevent_queue_respond_time(uint8_t event)
{
/*---------------Phan hoi neu trong Queue co nhieu ban tin------------*/
    if(qGet_Number_Items(&qRespondPcBox) == 0)
    {
        fevent_active(sEventAppPcBox, _EVENT_QUEUE_RESPOND_IMMEDIATELY);
        return 1;
    }
    else
    {
        if(sParamPcBox.ConnectPcBox == _CONNECT_PCBOX)
        {
            qQueue_Receive(&qRespondPcBox ,&sQueueReadData, 1);
            Transmit_PCBOX(sQueueReadData.aData_u8,sQueueReadData.Length);
        }
        
        fevent_enable(sEventAppPcBox, _EVENT_QUEUE_RESPOND_TIME);
    }
    return 1;
}

static uint8_t fevent_dcu_ping_pc_box(uint8_t event)
{
/*--------------------DCU Ping PcBox---------------------*/
  if(sParamPcBox.CountResetPcBox >= NUMBER_MAX_PING_PCBOX)  //Sau n lan khong ping thanh cong reset pc box
  {
    fevent_active(sEventAppPcBox, _EVENT_RESET_PC_BOX);
  }
  else
  {
    sParamPcBox.CountResetPcBox++;
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_DCU_PING_PC_BOX;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamPcBox.CountResetPcBox;
    
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
    
    fevent_enable(sEventAppPcBox, event);
  }
    return 1;
}

static uint8_t fevent_reset_pc_box(uint8_t event)
{
/*-----------------Reset PcBox------------------*/
    static uint8_t state_reset = 0;
    if(state_reset == 0)
    {
        sParamPcBox.CountResetPcBox = 0;
        sStatusApp.Pcbox = BUSY;
        state_reset = 1;
        HAL_GPIO_WritePin(ON_OFF_IR_GPIO_Port, ON_OFF_IR_Pin, GPIO_PIN_SET);
        ControlRelay(RELAY_SCREEN, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_UNCTRL);
        
        fevent_enable(sEventAppPcBox, event);
        fevent_disable(sEventAppPcBox, _EVENT_DCU_PING_PC_BOX);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"app_pc_box: Reset Pc Box", sizeof("app_pc_box: Reset Pc Box")-1);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
        
        MX_USART6_UART_Init();
        Init_Uart_PcBox_Rx_IT();
    }
    else
    {   
        sParamPcBox.ConnectPcBox = _DISCONNECT_PCBOX;
        state_reset = 0;
        HAL_GPIO_WritePin(ON_OFF_IR_GPIO_Port, ON_OFF_IR_Pin, GPIO_PIN_RESET);
        fevent_enable(sEventAppPcBox, _EVENT_WAIT_RESET_PC_BOX);
    }
    return 1;
}

static uint8_t fevent_wait_reset_pc_box(uint8_t event)
{
/*-----------------Wait Reset PcBox----------------*/
    sParamPcBox.ConnectPcBox = _CONNECT_PCBOX;
    fevent_active(sEventAppPcBox, _EVENT_DCU_PING_PC_BOX);
    sStatusApp.Pcbox = FREE;

    return 1;
}

static uint8_t fevent_refresh_dcu(uint8_t event)
{
/*----------------Kiem tra dieu kien va refresh DCU----------------*/
    if(sStatusApp.Motor == FREE && sStatusApp.Door == FREE 
       && sStatusApp.Temperature == FREE && sStatusApp.RL_Warm == FREE 
       && sStatusApp.Pcbox == FREE)
    {
      if(HAL_GetTick() - sEventAppPcBox[_EVENT_PC_BOX_LOG_TSVH].e_systick >=  sEventAppPcBox[_EVENT_PC_BOX_LOG_TSVH].e_period/2)
      {
        fevent_enable(sEventAppPcBox, _EVENT_RESET_DCU);
        fevent_disable(sEventAppPcBox, _EVENT_PC_BOX_RECEIVE_HANDLE);
        fevent_disable(sEventAppPcBox, _EVENT_PC_BOX_COMPLETE_RECEIVE);
        return 1;
      }
    }
    
    fevent_active(sEventAppPcBox, event);
    return 1;
}
/*================= Function Handle ================*/

void Packing_Respond_PcBox(uint8_t aData[], uint16_t Length)
{
    uint16_t TempCrc = 0;
    
    Calculator_Crc_U16(&TempCrc, aData, Length);
    aData[Length++] = TempCrc;
    aData[Length++] = TempCrc << 8;
    if(aData[0] == OBIS_DCU_PING_PC_BOX || aData[0] == OBIS_RESET_DCU)    
    Transmit_PCBOX(aData, Length);
    else    Write_Queue_Repond_PcBox(aData, Length);
}

/*
    @brief  Truyen Data len PcBox co Debug
*/
void Transmit_PCBOX(uint8_t aData[],uint8_t length)
{
    Respond_PcBox(aData, length);
    AppPcBox_Debug(aData, length , _TRANS_PCBOX);
};

/*
    @brief  Viet vao Queue doi gui lenh len PcBox
*/
void Write_Queue_Repond_PcBox(uint8_t aData[], uint8_t Length)
{
    sDataQueueRespondPcBox  sQueueWrite;
    sQueueWrite.Length   = Length;
    for(uint8_t i=0; i<Length; i++)
    {
        sQueueWrite.aData_u8[i] = aData[i];
    }
    
    qQueue_Send(&qRespondPcBox, (sDataQueueRespondPcBox*)&sQueueWrite, _TYPE_SEND_TO_END);
}

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
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.PowerPresent;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sStatusRelay.Screen;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sStatusRelay.Lamp;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sStatusRelay.Warm;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sStatusRelay.FridgeHeat;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sStatusRelay.FridgeCool;
    
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
    
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sTimeCycleWarm.Run;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sTimeCycleWarm.Wait;
    
    sRespPcBox.Data_a8[1] = sRespPcBox.Length_u16 - 2;
    
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
}

void Pc_Box_Init(void)
{
    qQueue_Create(&qRespondPcBox, 100, sizeof(sDataQueueRespondPcBox),(sDataQueueRespondPcBox*)&sQueueRespondPcBox);
}

void Init_DCU_ID(void)
{
    uint8_t read[MAX_LENGTH_DCU_ID+2] = {0};
    eFlash_S25FL_BufferRead(read, EX_FLASH_ADDR_MAIN_ID , MAX_LENGTH_DCU_ID+2);
    
    if(read[0] == DEFAULT_READ_EXFLASH)
    {
        uint8_t length = read[1];
        sDCU_ID.Length_u16 = 0;
        for(uint8_t i=0; i<length; i++)
        {
            sDCU_ID.Data_a8[sDCU_ID.Length_u16++]=read[i+2];
        }
    }
    else
    {
        read[0] = DEFAULT_READ_EXFLASH;
        read[1] = sDCU_ID.Length_u16;
        for(uint8_t i=0; i<sDCU_ID.Length_u16; i++)
        {
            read[i+2]=sDCU_ID.Data_a8[i];
        }
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_MAIN_ID);
        HAL_Delay(1);
        eFlash_S25FL_BufferWrite(read, EX_FLASH_ADDR_MAIN_ID, sDCU_ID.Length_u16 + 2);
    }
}

/*
    @brief  Cai dat thoi gian gui thong so van hanh (min)
*/
void Set_TimeTSVH(uint8_t Time)
{
    if(Time == 0) Time = 1;
    sParamPcBox.TimeTSVH = Time;
    
    uint8_t write[2]={0x00};
    
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_TIME_LOG_TSVH;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamPcBox.TimeTSVH ;

    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
    
    write[0] = DEFAULT_READ_EXFLASH;
    write[1] = sParamPcBox.TimeTSVH ;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_FREQ_TSVH);
    eFlash_S25FL_BufferWrite(write, EX_FLASH_ADDR_FREQ_TSVH, 2);
    sEventAppPcBox[_EVENT_PC_BOX_LOG_TSVH].e_period = sParamPcBox.TimeTSVH * TIME_ONE_MINUTES;
    fevent_enable(sEventAppPcBox, _EVENT_PC_BOX_LOG_TSVH);
}

void Init_Using_Crc()
{
    uint8_t read[2] = {0};
    eFlash_S25FL_BufferRead(read, EX_FLASH_ADDR_PCBOX_USING_CRC , 2);
    if(read[0] == DEFAULT_READ_EXFLASH)
    {
        if(read[1] == 0x01) sParamPcBox.UsingCrc = _USING_CRC;
        else if(read[1] == 0x00) sParamPcBox.UsingCrc = _UNUSING_CRC;
    }
}

void Write_Flash_Using_Crc(void)
{
  uint8_t write[2]={0};
    write[0] = DEFAULT_READ_EXFLASH;
    write[1] = sParamPcBox.UsingCrc ;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_PCBOX_USING_CRC);
    eFlash_S25FL_BufferWrite(write, EX_FLASH_ADDR_PCBOX_USING_CRC, 2);
}

/*
    @brief  Cai dat thoi gian reset PcBox
*/
void Set_TimeResetPcBox(uint8_t Time)
{
    if(Time == 0) Time = 1;
    sParamPcBox.TimeResetPcBox = Time;
    uint8_t write[3]={0x00};
    write[0] = DEFAULT_READ_EXFLASH;
    write[1] = sParamPcBox.TimeResetPcBox ;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_TIME_RESET_PCBOX);
    eFlash_S25FL_BufferWrite(write, EX_FLASH_ADDR_TIME_RESET_PCBOX, 3);
    sEventAppPcBox[_EVENT_WAIT_RESET_PC_BOX].e_period = sParamPcBox.TimeResetPcBox * TIME_ONE_MINUTES;
}

void Init_PcBox(void)
{
    uint8_t read[2] = {0};
    eFlash_S25FL_BufferRead(read, EX_FLASH_ADDR_FREQ_TSVH , 2);
    if( read[0] == DEFAULT_READ_EXFLASH)
    {
        sParamPcBox.TimeTSVH  = read[1];
    }
    
    eFlash_S25FL_BufferRead(read, EX_FLASH_ADDR_TIME_RESET_PCBOX , 2);
    if( read[0] == DEFAULT_READ_EXFLASH)
    {
        sParamPcBox.TimeResetPcBox  = read[1];
    }
    sEventAppPcBox[_EVENT_WAIT_RESET_PC_BOX].e_period = sParamPcBox.TimeResetPcBox * TIME_ONE_MINUTES;
}

void ResetDCU(void)
{
    HAL_Delay(1000);
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
    HAL_Delay(500);
    Reset_Chip();
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
