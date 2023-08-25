#include "user_app_pc_box.h"
#include "user_comm_vending_machine.h"
#include "user_inc_vending_machine.h"
#include "user_external_flash.h"
/*============== Function Static =========*/
static uint8_t fevent_pcbox_entry(uint8_t event);
static uint8_t fevent_pcbox_receive_handle(uint8_t event);
static uint8_t fevent_pcbox_complete_receive(uint8_t event);
static uint8_t fevent_pcbox_log_tsvh(uint8_t event);
static uint8_t fevent_pcbox_ping(uint8_t event);
static uint8_t fevent_pcbox_set_dcu_ID(uint8_t event);
static uint8_t fevent_pcbox_get_dcu_ID(uint8_t event);
static uint8_t fevent_wdg_stm32f4(uint8_t event);
static uint8_t fevent_reset_dcu(uint8_t event);
static uint8_t fevent_queue_respond_immediately(uint8_t event);
static uint8_t fevent_queue_respond_time(uint8_t event);
static uint8_t fevent_refresh_dcu(uint8_t event);
/*=============== Struct =================*/
sEvent_struct               sEventAppPcBox[]=
{
  {_EVENT_PC_BOX_ENTRY,                 1, 5, TIME_ENTRY,           fevent_pcbox_entry},
  {_EVENT_PC_BOX_RECEIVE_HANDLE,        1, 0, 5,                    fevent_pcbox_receive_handle},
  {_EVENT_PC_BOX_COMPLETE_RECEIVE,      0, 0, 5,                    fevent_pcbox_complete_receive},
  {_EVENT_PC_BOX_LOG_TSVH,              0, 0, TIME_SEND_TSVH,       fevent_pcbox_log_tsvh},
  {_EVENT_PC_BOX_PING,                  0, 0, 5,                    fevent_pcbox_ping},
  {_EVENT_PC_BOX_SET_DCU_ID,            0, 0, 5,                    fevent_pcbox_set_dcu_ID},
  {_EVENT_PC_BOX_GET_DCU_ID,            0, 0, 5,                    fevent_pcbox_get_dcu_ID},
  {_EVENT_WDG_STM32F4,                  0, 0, TIME_RESET_WDG,       fevent_wdg_stm32f4},
  {_EVENT_RESET_DCU,                    0, 0, 2000,                 fevent_reset_dcu},
  
  {_EVENT_QUEUE_RESPOND_IMMEDIATELY,    1, 0, 5,                    fevent_queue_respond_immediately},
  {_EVENT_QUEUE_RESPOND_TIME,           0, 0, TIME_RESPOND_PC_BOX,  fevent_queue_respond_time},
  
  {_EVENT_REFRESH_DCU,                  0, 5, TIME_REFRESH_DCU,     fevent_refresh_dcu},
};

uint8_t     aBuffer_Ping[20];
sData       sBuffer_Ping=
{
    .Data_a8    = aBuffer_Ping,
    .Length_u16 = 0,
};
uint8_t aDCU_ID[MAX_LENGTH_DCU_ID] = DEVICE_ID;
sData   sDCU_ID=
{
    .Data_a8 = &aDCU_ID[0], 
    .Length_u16 = 14,
};
sDataQueueRespondPcBox          sQueueRespondPcBox[20];
sDataQueueRespondPcBox          sQueueReadData;
Struct_Queue_Type               qRespondPcBox;

uint32_t Flag_Refresh_Dcu = 0;
/*============== Function Static ===========*/
static uint8_t fevent_pcbox_entry(uint8_t event)
{
    fevent_active(sEventAppPcBox, _EVENT_PC_BOX_LOG_TSVH);
    fevent_enable(sEventAppPcBox, _EVENT_REFRESH_DCU);
    return 1;
}

static uint8_t fevent_wdg_stm32f4(uint8_t event)
{
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
    uint16_t pos = 0;
    uint8_t  Obis_Recv = 0;
    uint8_t Length_Data = 0;
    uint16_t Crc_Check = 0;
    uint16_t Crc_Recv  = 0;

    Crc_Recv = (sUartPcBox.Data_a8[sUartPcBox.Length_u16-1] << 8) |
               (sUartPcBox.Data_a8[sUartPcBox.Length_u16-2]);
    Calculator_Crc_U16(&Crc_Check, sUartPcBox.Data_a8, sUartPcBox.Length_u16 - 2);
    
    if(Crc_Check != Crc_Recv)
    {
        while((pos + 2) <= sUartPcBox.Length_u16 - 2) //co 2 byte CRC cuoi cung
        {
            Obis_Recv   = sUartPcBox.Data_a8[pos++];
            switch(Obis_Recv)
            {
                case OBIS_PC_BOX_FIX_MOTOR:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                    if(sPushMotor.StatePush == COMPLETE_PUSH)
                    {
                        sPushMotor.State      = FIX_MOTOR;
                        sPushMotor.StatePush  = ON_GOING_PUSH;
                        sPushMotor.Pos        = sUartPcBox.Data_a8[pos];
                        sPushMotor.Num        = 1;
                        sPushMotor.PulseCount = 1;
                        fevent_active(sEventAppMotor, _EVENT_MOTOR_ENTRY);
                    }
                    pos += 1;
                  }
                  break;
              
                case OBIS_PC_BOX_CTRL_MOTOR:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x02 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                    if(sPushMotor.StatePush == COMPLETE_PUSH)
                    {
                        sPushMotor.State      = PUSH_MOTOR;
                        sPushMotor.StatePush  = ON_GOING_PUSH;
                        sPushMotor.Pos        = sUartPcBox.Data_a8[pos];
                        sPushMotor.Num        = sUartPcBox.Data_a8[pos+1];
                        sPushMotor.PulseCount = 0;
                        fevent_active(sEventAppMotor, _EVENT_MOTOR_ENTRY);
                    }
                    pos += 2;
                  }
                  break;
                  
                case OBIS_RESET_DCU:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {              
                        fevent_active(sEventAppPcBox, _EVENT_RESET_DCU);
                  }
                  break; 
                  
                case OBIS_ON_OFF_RELAY_SCREEN:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x01)
                      {
                        sStatusRelay.Screen= sUartPcBox.Data_a8[pos];
                        fevent_active(sEventAppRelay , _EVENT_ON_OFF_RELAY_SCREEN);
                        pos += 1;
                      }
                  }
                  break;   
                  
                case OBIS_ON_OFF_RELAY_LAMP:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x01)
                      {
                        sStatusRelay.Lamp = sUartPcBox.Data_a8[pos];
                        fevent_active(sEventAppRelay , _EVENT_ON_OFF_RELAY_LAMP);
                        pos += 1;
                      }
                  }
                  break; 
         
                case OBIS_ON_OFF_RELAY_WARM:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x01)
                      {
                        sStatusRelay.Warm = sUartPcBox.Data_a8[pos];
                        fevent_active(sEventAppRelay , _EVENT_ON_OFF_RELAY_WARM);
                        pos += 1;
                      }
                  }
                  break; 
                  
                case OBIS_TEMP_THRESHOLD:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x03 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                        sTemp_Thresh_Recv.Value  =  sUartPcBox.Data_a8[pos++];
                        sTemp_Thresh_Recv.Value  =  sTemp_Thresh_Recv.Value << 8;
                        sTemp_Thresh_Recv.Value |=  sUartPcBox.Data_a8[pos++];
                        sTemp_Thresh_Recv.Scale  =  sUartPcBox.Data_a8[pos++];
                        fevent_active(sEventAppTemperature , _EVENT_TEMP_SET_THRESHOLD);
                  }
                  break;
                  
                case OBIS_PING_PC_BOX:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if((pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                        sBuffer_Ping.Length_u16 = 0;
                        sBuffer_Ping.Data_a8[sBuffer_Ping.Length_u16++] = OBIS_PING_PC_BOX;
                        sBuffer_Ping.Data_a8[sBuffer_Ping.Length_u16++] = Length_Data;
                        while(Length_Data > 0)
                        {
                            sBuffer_Ping.Data_a8[sBuffer_Ping.Length_u16++] = sUartPcBox.Data_a8[pos++];
                            Length_Data--;
                        }
                        fevent_active(sEventAppPcBox , _EVENT_PC_BOX_PING);
                  }
                  break;
                  
                case OBIS_GET_DCU_ID:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if((pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                        fevent_active(sEventAppPcBox , _EVENT_PC_BOX_GET_DCU_ID);
                  }
                  break;         
                  
                  
                case OBIS_SET_DCU_ID:
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
                  }
                  break;
                  
                default:
                  break;
            }
        }
        Write_Queue_Repond_PcBox((uint8_t*)"OK",2);
    }
    else
    {
        Init_Uart_Module(); 
        Write_Queue_Repond_PcBox((uint8_t*)"ERROR",5);
    }
    AppPcBox_Debug();
        
    UTIL_MEM_set(sUartPcBox.Data_a8 , 0x00, sUartPcBox.Length_u16);
    sUartPcBox.Length_u16 = 0;
    
    fevent_active(sEventAppPcBox, _EVENT_PC_BOX_RECEIVE_HANDLE);
    return 1;
}

static uint8_t fevent_pcbox_log_tsvh(uint8_t event)
{
    uint8_t length=0;
    uint8_t aData[50]={0};
    length = Log_TSVH(aData);
    Write_Queue_Repond_PcBox(aData, length);
    fevent_enable(sEventAppPcBox, _EVENT_PC_BOX_LOG_TSVH);
    return 1;
}

static uint8_t fevent_pcbox_ping(uint8_t event)
{
    uint16_t TempCrc = 0;
    Calculator_Crc_U16(&TempCrc, sBuffer_Ping.Data_a8, sBuffer_Ping.Length_u16);
    
    sBuffer_Ping.Data_a8[sBuffer_Ping.Length_u16++] = TempCrc;
    sBuffer_Ping.Data_a8[sBuffer_Ping.Length_u16++] = TempCrc << 8;

    Write_Queue_Repond_PcBox(sBuffer_Ping.Data_a8, sBuffer_Ping.Length_u16);
    return 1;
}

static uint8_t fevent_pcbox_set_dcu_ID(uint8_t event)
{
    uint8_t write[MAX_LENGTH_DCU_ID+2] = {0};
    write[0]= DEFAULT_READ_EXFLASH;
    write[1]= sDCU_ID.Length_u16;
   
    for(uint8_t i=0; i<sDCU_ID.Length_u16; i++)
    {
        write[i+2]=sDCU_ID.Data_a8[i];
    }
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_MAIN_ID);
    eFlash_S25FL_BufferWrite(write, EX_FLASH_ADDR_MAIN_ID, sDCU_ID.Length_u16+2);
    
    Write_Queue_Repond_PcBox(sDCU_ID.Data_a8,sDCU_ID.Length_u16);
    return 1;
}

static uint8_t fevent_pcbox_get_dcu_ID(uint8_t event)
{
    Write_Queue_Repond_PcBox(sDCU_ID.Data_a8,sDCU_ID.Length_u16);
    return 1;
}

static uint8_t fevent_reset_dcu(uint8_t event)
{
    if(qGet_Number_Items(&qRespondPcBox) == 0)
    {
        Write_Status_Relay_ExFlash();
        Reset_Chip();
    }
    fevent_active(sEventAppPcBox, event);
    return 1;
}

static uint8_t fevent_queue_respond_immediately(uint8_t event)
{
    if(qGet_Number_Items(&qRespondPcBox) > 0)
    {
        qQueue_Receive(&qRespondPcBox ,&sQueueReadData, 1);
        Respond_PcBox(sQueueReadData.aData_u8,sQueueReadData.Length);

        fevent_enable(sEventAppPcBox, _EVENT_QUEUE_RESPOND_TIME);
        return 1;
    }
    fevent_enable(sEventAppPcBox, event);
    return 1;
}

static uint8_t fevent_queue_respond_time(uint8_t event)
{
    if(qGet_Number_Items(&qRespondPcBox) == 0)
    {
        fevent_active(sEventAppPcBox, _EVENT_QUEUE_RESPOND_IMMEDIATELY);
        return 1;
    }
    else
    {
        qQueue_Receive(&qRespondPcBox ,&sQueueReadData, 1);
        Respond_PcBox(sQueueReadData.aData_u8,sQueueReadData.Length);

        fevent_enable(sEventAppPcBox, _EVENT_QUEUE_RESPOND_TIME);
    }
    return 1;
}

static uint8_t fevent_refresh_dcu(uint8_t event)
{
    if(Flag_Refresh_Dcu == 0)
    {
        fevent_active(sEventAppPcBox, _EVENT_RESET_DCU);
        fevent_disable(sEventAppPcBox, _EVENT_PC_BOX_RECEIVE_HANDLE);
        fevent_disable(sEventAppPcBox, _EVENT_PC_BOX_COMPLETE_RECEIVE);
    }
    else
    {
        fevent_enable(sEventAppPcBox, event);
    }
    return 1;
}
/*================= Function Handle ================*/

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

uint8_t Log_TSVH(uint8_t *aData)
{
    uint8_t length = 0;
    uint16_t TempCrc = 0;
    
/*=============== Log ===============*/
    
    aData[length++] = OBIS_TSVH_PC_BOX;
    aData[length++] = 0x00;
    aData[length++] = Status_Supply_Power;
    aData[length++] = sStatusRelay.Screen;
    aData[length++] = sStatusRelay.Lamp;
    aData[length++] = sStatusRelay.Warm;
    aData[length++] = sStatusRelay.FridgeHeat;
    aData[length++] = sStatusRelay.FridgeCool;
    
    aData[length++] = Threshold_Ctrl >> 8;
    aData[length++] = Threshold_Ctrl;
    aData[length++] = sTemperature.Value >> 8;
    aData[length++] = sTemperature.Value;
    aData[length++] = DEFAULT_TEMP_SCALE;

    aData[length++] = sElectric.Current >> 8;
    aData[length++] = sElectric.Current;
    aData[length++] = sElectric.Voltage >> 8;
    aData[length++] = sElectric.Voltage;
    aData[length++] = sElectric.Scale;
    
    aData[1] = length - 2;
    
    Calculator_Crc_U16(&TempCrc, aData, length);
    
    aData[length++] = TempCrc;
    aData[length++] = TempCrc >> 8;
      
    return length;
}

void Pc_Box_Init(void)
{
    qQueue_Create(&qRespondPcBox, 20, sizeof(sDataQueueRespondPcBox),(sDataQueueRespondPcBox*)&sQueueRespondPcBox);
}

void Init_DCU_ID(void)
{
    uint8_t read[MAX_LENGTH_DCU_ID+2] = {0};
    eFlash_S25FL_BufferRead(read, EX_FLASH_ADDR_MAIN_ID , MAX_LENGTH_DCU_ID+2);
    
    if(read[0] == DEFAULT_READ_EXFLASH)
    {
        uint8_t length = read[1];
        for(uint8_t i=0; i<length; i++)
        {
            sDCU_ID.Data_a8[i]=read[i+2];
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

void AppPcBox_Debug(void)
{
#ifdef USING_APP_PC_BOX_DEBUG

    UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_pc_box: Recv: ", sizeof("app_pc_box: Recv: "));
    uint8_t array[50]={0};
    sData sSource=
    {
        .Data_a8 = array,
    };
    if(sUartPcBox.Length_u16 < 25)
    {
        Convert_Hex_To_String_Hex(&sSource, &sUartPcBox);
        UTIL_Printf(DBLEVEL_M, (uint8_t*)sSource.Data_a8, sSource.Length_u16);
    }
    else
    {
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"Data Large", sizeof("Data Large"));
    }

    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n"));
    HAL_Delay(1);
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
