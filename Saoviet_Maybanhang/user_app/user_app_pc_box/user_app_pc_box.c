#include "user_app_pc_box.h"

/*============== Function Static =========*/
static uint8_t fevent_pcbox_receive_handle(uint8_t event);
static uint8_t fevent_pcbox_complete_receive(uint8_t event);

/*=============== Struct =================*/
sEvent_struct               sEventAppPcBox[]=
{
  {_EVENT_PC_BOX_RECEIVE_HANDLE,        0, 0, 5,     fevent_pcbox_receive_handle},
  {_EVENT_PC_BOX_COMPLETE_RECEIVE,      0, 0, 5,     fevent_pcbox_complete_receive},
};

/*============== Function Static ===========*/
static uint8_t fevent_pcbox_receive_handle(uint8_t event)
{
    static uint16_t countBuffer_uart = 0;

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
    
    if(Crc_Check == Crc_Recv)
    {
        while((pos + 2) < sUartPcBox.Length_u16 - 2) //co 2 byte CRC cuoi cung
        {
            Obis_Recv   = sUartPcBox.Data_a8[pos++];
            switch(Obis_Recv)
            {
                case OBIS_PC_BOX_CTRL_MOTOR:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x02 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x07 && sUartPcBox.Data_a8[pos+1] <=0x0A)
                      {
                        sControlMotor.Status = 1;
                        sControlMotor.Layer  = sUartPcBox.Data_a8[2];
                        sControlMotor.Slot   = sUartPcBox.Data_a8[3];
                        pos += 2;
                      }
                  }
                  break;
                  
                case OBIS_ON_OFF_RELAY_PC:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x01)
                      {
                        sRelay_PC.Status_Recv = 1;
                        sRelay_PC.Status_Relay= sUartPcBox.Data_a8[pos];
                        pos += 1;
                      }
                  }
                  break;  
                  
                case OBIS_ON_OFF_RELAY_SCREEN:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x01)
                      {
                        sRelay_Screen.Status_Recv = 1;
                        sRelay_Screen.Status_Relay= sUartPcBox.Data_a8[pos];
                        pos += 1;
                      }
                  }
                  break;   
                  
                case OBIS_ON_OFF_RELAY_FRIDGE:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x01)
                      {
                        sRelay_Fridge.Status_Recv = 1;
                        sRelay_Fridge.Status_Relay= sUartPcBox.Data_a8[pos];
                        pos += 1;
                      }
                  }
                  break;   
                  
                case OBIS_ON_OFF_RELAY_ALARM:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x01)
                      {
                        sRelay_Alarm.Status_Recv = 1;
                        sRelay_Alarm.Status_Relay= sUartPcBox.Data_a8[pos];
                        pos += 1;
                      }
                  }
                  break;   
                
                case OBIS_ON_OFF_RELAY_5:
                  Length_Data = sUartPcBox.Data_a8[pos++];
                  if(Length_Data == 0x01 && (pos + Length_Data) <= (sUartPcBox.Length_u16 - 2))
                  {
                      if(sUartPcBox.Data_a8[pos] <= 0x01)
                      {
                        sRelay_Relay5.Status_Recv = 1;
                        sRelay_Relay5.Status_Relay= sUartPcBox.Data_a8[pos];
                        pos += 1;
                      }
                  }
                  break;   
                  
                default:
                  break;
            }
        }
    }
     
    UTIL_MEM_set(sUartPcBox.Data_a8 , 0x00, sUartPcBox.Length_u16);
    sUartPcBox.Length_u16 = 0;
    return 1;
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
