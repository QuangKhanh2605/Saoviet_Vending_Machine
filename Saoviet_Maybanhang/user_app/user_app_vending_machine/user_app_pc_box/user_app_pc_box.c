#include "user_app_pc_box.h"
#include "user_comm_vending_machine.h"
#include "user_inc_vending_machine.h"
/*============== Function Static =========*/
static uint8_t fevent_pcbox_receive_handle(uint8_t event);
static uint8_t fevent_pcbox_complete_receive(uint8_t event);
static uint8_t fevent_pcbox_log_tsvh(uint8_t event);
static uint8_t fevent_wdg_stm32f4(uint8_t event);
static uint8_t fevent_pcbox_entry(uint8_t event);
static uint8_t fevent_pcbox_ping(uint8_t event);
/*=============== Struct =================*/
sEvent_struct               sEventAppPcBox[]=
{
  {_EVENT_PC_BOX_ENTRY,                 1, 5, TIME_ENTRY,   fevent_pcbox_entry},
  {_EVENT_PC_BOX_RECEIVE_HANDLE,        1, 0, 5,            fevent_pcbox_receive_handle},
  {_EVENT_PC_BOX_COMPLETE_RECEIVE,      0, 0, 5,            fevent_pcbox_complete_receive},
  {_EVENT_PC_BOX_LOG_TSVH,              0, 0, 60000,        fevent_pcbox_log_tsvh},
  {_EVENT_PC_BOX_PING,                  0, 0, 5,            fevent_pcbox_ping},
  {_EVENT_WDG_STM32F4,                  0, 0, 1000,         fevent_wdg_stm32f4},
};

uint8_t     aBuffer_Ping[20];
sData       sBuffer_Ping=
{
    .Data_a8    = aBuffer_Ping,
    .Length_u16 = 0,
};

/*============== Function Static ===========*/
static uint8_t fevent_pcbox_entry(uint8_t event)
{
    fevent_active(sEventAppPcBox, _EVENT_PC_BOX_LOG_TSVH);
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
    Crc_Recv = (sUartPcBox.Data_a8[sUartPcBox.Length_u16-2] << 8) |
               (sUartPcBox.Data_a8[sUartPcBox.Length_u16-1]);
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
                    PosPushMotor = sUartPcBox.Data_a8[pos];
                    pos += 1;
                    PulseCount = 1;
                    fevent_active(sEventAppMotor, _EVENT_CONTROL_MOTOR_PUSH);
                  }
                  break;
              
                case OBIS_PC_BOX_CTRL_MOTOR:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                    PosPushMotor = sUartPcBox.Data_a8[pos];
                    pos += 1;
                    PulseCount = 0;
                    fevent_active(sEventAppMotor, _EVENT_CONTROL_MOTOR_PUSH);
                  }
                  break;
                  
                case OBIS_ON_OFF_RELAY_PC:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x01)
                      {
                        sStatusRelay.PC= sUartPcBox.Data_a8[pos];
                        pos += 1;
                        fevent_active(sEventAppRelay, _EVENT_ON_OFF_RELAY_PC);
                      }
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
                        sStatusRelay.Lamp= sUartPcBox.Data_a8[pos];
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
                        sStatusRelay.Warm= sUartPcBox.Data_a8[pos];
                        fevent_active(sEventAppRelay , _EVENT_ON_OFF_RELAY_WARM);
                        pos += 1;
                      }
                  }
                  break; 
                  
                case OBIS_TEMP_THRESHOLD:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x03 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                        sTemp_Thresh_Recv.Value  =  sUartPcBox.Data_a8[pos++] << 8;
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
                        while(Length_Data > 0)
                        {
                            sBuffer_Ping.Data_a8[sBuffer_Ping.Length_u16++] = sUartPcBox.Data_a8[pos++];
                            Length_Data--;
                        }
                        fevent_active(sEventAppPcBox , _EVENT_PC_BOX_PING);
                  }
                  break;
               
                  
                default:
                  break;
            }
        }
        Respond_PcBox((uint8_t*)"OK",2);
    }
    else
    {
        Init_Uart_Module(); 
        Respond_PcBox((uint8_t*)"ERROR",2);
    }
     
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
    Respond_PcBox(aData, length);
    fevent_enable(sEventAppPcBox, _EVENT_PC_BOX_LOG_TSVH);
    return 1;
}

static uint8_t fevent_pcbox_ping(uint8_t event)
{
    uint16_t TempCrc = 0;
    Calculator_Crc_U16(&TempCrc, sBuffer_Ping.Data_a8, sBuffer_Ping.Length_u16);
    
    sBuffer_Ping.Data_a8[sBuffer_Ping.Length_u16++] = TempCrc << 8;
    sBuffer_Ping.Data_a8[sBuffer_Ping.Length_u16++] = TempCrc;
    Respond_PcBox(sBuffer_Ping.Data_a8, sBuffer_Ping.Length_u16);
    return 1;
}

uint8_t Log_TSVH(uint8_t *aData)
{
    uint8_t length = 0;
    uint16_t TempCrc = 0;
    
/*=============== Log ===============*/
    
    aData[length++] = OBIS_TSVH_PC_BOX;
    aData[length++] = 0x00;
    aData[length++] = sStatusRelay.Screen;
    aData[length++] = sStatusRelay.Lamp;
    aData[length++] = sStatusRelay.Warm;
    aData[length++] = sStatusRelay.Relay_5;
    aData[length++] = sStatusRelay.Fridge;
    
    aData[length++] = Threshold_Ctrl << 8;
    aData[length++] = Threshold_Ctrl;
    aData[length++] = sTemperature.Value << 8;
    aData[length++] = sTemperature.Value;
    aData[length++] = sTemperature.Scale;
    
    aData[length++] = sElectric.Value << 8;
    aData[length++] = sElectric.Value;
    aData[length++] = sElectric.Scale;
    
    aData[1] = length;
    
    Calculator_Crc_U16(&TempCrc, aData, length);
    
    aData[length++] = TempCrc << 8;
    aData[length++] = TempCrc;
      
    return length;
}

/*================= Function Handle ================*/
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
