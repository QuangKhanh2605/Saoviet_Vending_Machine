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
    uint16_t Crc_Check = 0;
    uint16_t Crc_Recv  = 0;
    uint8_t  Obis_Recv = 0;
    
    Obis_Recv   = sUartPcBox.Data_a8[0];
    Crc_Recv = (sUartPcBox.Data_a8[sUartPcBox.Length_u16-2] << 8) |
               (sUartPcBox.Data_a8[sUartPcBox.Length_u16-1]);
    
    Calculator_Crc_U16(&Crc_Check, sUartPcBox.Data_a8, sUartPcBox.Length_u16 - 2);
    
    if(Crc_Check == Crc_Recv)
    {
        switch(Obis_Recv)
        {
            case OBIS_PC_BOX_CTRL_MOTOR:
              if(sUartPcBox.Data_a8[2] <= 0x07 && sUartPcBox.Data_a8[3] <=0x0A && sUartPcBox.Data_a8[1] == 0x02)
              {
                sControlMotor.Status = 1;
                sControlMotor.Layer  = sUartPcBox.Data_a8[2];
                sControlMotor.Slot   = sUartPcBox.Data_a8[3];
              }
              break;
            
            default:
              break;
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
