#include "user_app_at_debug.h"
#include "user_at_serial.h"
#include "user_comm_vending_machine.h"
/*============== Function Static =========*/
static uint8_t fevent_at_debug_entry(uint8_t event);
static uint8_t fevent_at_debug_receive_handle(uint8_t event);
static uint8_t fevent_at_debug_complete_receive(uint8_t event);
/*=============== Struct =================*/
sEvent_struct               sEventAppAtDebug[]=
{
  {_EVENT_AT_DEBUG_ENTRY,                 1, 5, TIME_ON_DCU, fevent_at_debug_entry},
  {_EVENT_AT_DEBUG_RECEIVE_HANDLE,        0, 0, 5,           fevent_at_debug_receive_handle},
  {_EVENT_AT_DEBUG_COMPLETE_RECEIVE,      0, 0, 5,           fevent_at_debug_complete_receive},
};

/*========================================*/

static uint8_t fevent_at_debug_entry(uint8_t event)
{
    fevent_active(sEventAppAtDebug, _EVENT_AT_DEBUG_RECEIVE_HANDLE);
    UTIL_MEM_set(sUartDebug.Data_a8 , 0x00, sUartDebug.Length_u16);
    sUartDebug.Length_u16 = 0;
    return 1;
}

static uint8_t fevent_at_debug_receive_handle(uint8_t event)
{
    static uint16_t countBuffer_uart = 0;

    if(sUartDebug.Length_u16 != 0)
    {
        if(countBuffer_uart == sUartDebug.Length_u16)
        {
            countBuffer_uart = 0;
            fevent_active(sEventAppAtDebug, _EVENT_AT_DEBUG_COMPLETE_RECEIVE);
            return 1;
        }
        else
        {
            countBuffer_uart = sUartDebug.Length_u16;
        }
    }
    
    fevent_enable(sEventAppAtDebug, event);
    return 1;
}

static uint8_t fevent_at_debug_complete_receive(uint8_t event)
{
    Check_AT_User(&sUartDebug, _AT_REQUEST_SERIAL);
    
    UTIL_MEM_set(sUartDebug.Data_a8 , 0x00, sUartDebug.Length_u16);
    sUartDebug.Length_u16 = 0;
    fevent_active(sEventAppAtDebug, _EVENT_AT_DEBUG_RECEIVE_HANDLE);
    return 1;
}

/*================= Function Handle ================*/

uint8_t AppAtDebug_Task(void)
{
    uint8_t i = 0;
    uint8_t Result = false;
    
    for(i = 0; i < _EVENT_AT_DEBUG_END ; i++)
    {
        if(sEventAppAtDebug[i].e_status == 1)
        {
            Result = true;
            if(sEventAppAtDebug[i].e_systick == 0 || 
               ((HAL_GetTick() - sEventAppAtDebug[i].e_systick) >= sEventAppAtDebug[i].e_period))
            {
                sEventAppAtDebug[i].e_status = 0;  //Disable event
				sEventAppAtDebug[i].e_systick = HAL_GetTick();
				sEventAppAtDebug[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}


