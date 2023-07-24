#include "user_app_electric.h"

/*============== Function static =============*/
static uint8_t fevent_electric_1(uint8_t event);
static uint8_t fevent_electric_2(uint8_t event);

/*============== Struct ===================*/
sEvent_struct               sEventAppElectric[] = 
{
  {_EVENT_ELECTRIC_1,            0, 0, 5,        fevent_electric_1},
  {_EVENT_ELECTRIC_2,            0, 0, 5,        fevent_electric_2},
};

/*============= Function Handle =============*/

static uint8_t fevent_electric_1(uint8_t event)
{
    return 1;
}

static uint8_t fevent_electric_2(uint8_t event)
{
    return 1;
}

uint8_t  AppElectric_Task(void)
{
  
    uint8_t i = 0;
    uint8_t Result = false;
    for(i = 0; i < _EVENT_ELECTRIC_END; i++)
    {
        if(sEventAppElectric[i].e_status == 1)
        {
            Result = true;
            
            if((sEventAppElectric[i].e_systick == 0) ||
               ((HAL_GetTick() - sEventAppElectric[i].e_systick) >= sEventAppElectric[i].e_period))
            {
                sEventAppElectric[i].e_status = 0;  //Disable event
                sEventAppElectric[i].e_systick= HAL_GetTick();
                sEventAppElectric[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}
