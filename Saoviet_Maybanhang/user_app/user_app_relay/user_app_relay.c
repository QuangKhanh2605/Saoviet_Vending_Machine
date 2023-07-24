#include "user_app_relay.h"

/*============== Function static ===============*/
static uint8_t fevent_relay_1(uint8_t event);
static uint8_t fevent_relay_2(uint8_t event);

/*=================== struct ==================*/
sEvent_struct               sEventAppFridgeLamp[] = 
{
  {_EVENT_RELAY_1,      0, 0, 5,    fevent_relay_1},
  {_EVENT_RELAY_2,      0, 0, 5,    fevent_relay_2},
};

/*================= Function Handle ==============*/
static uint8_t fevent_relay_1(uint8_t event)
{
    return 1;
}

static uint8_t fevent_relay_2(uint8_t event)
{
    return 1;
}


uint8_t AppRelay_Task(void)
{
    uint8_t i = 0;
    uint8_t Result = false;
    for( i = 0; i < _EVENT_RELAY_END; i++)
    {
        if(sEventAppFridgeLamp[i].e_status == 1)
        {
            Result = true;
            if((sEventAppFridgeLamp[i].e_systick == 0) ||
                ((HAL_GetTick() - sEventAppFridgeLamp[i].e_systick) >= sEventAppFridgeLamp[i].e_period))
            {
                sEventAppFridgeLamp[i].e_status = 0; //Disable event
                sEventAppFridgeLamp[i].e_systick= 0;
                sEventAppFridgeLamp[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}

