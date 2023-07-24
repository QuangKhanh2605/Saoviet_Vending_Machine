#include "user_app_vib_sensor.h"

/*=============== Function static ================*/
static uint8_t fevent_vib_sensor_1(uint8_t event);
static uint8_t fevent_vib_sensor_2(uint8_t event);

/*================== Struct ===================*/
sEvent_struct       sEventAppVibSensor[]=
{
  {_EVENT_VIB_SENSOR_1,     0, 0, 5,    fevent_vib_sensor_1},
  {_EVENT_VIB_SENSOR_2,     0, 0, 5,    fevent_vib_sensor_2},
};

/*================== Function Handle ==============*/
static uint8_t fevent_vib_sensor_1(uint8_t event)
{
    return 1;
}

static uint8_t fevent_vib_sensor_2(uint8_t event)
{
    return 1;
}

uint8_t AppVibSensor_Task(void)
{
    uint8_t i = 0;
    uint8_t Result =  false;
    
    for(i = 0; i < _EVENT_VIB_SENSOR_END; i++)
    {
        if(sEventAppVibSensor[i].e_status == 1)
        {
            Result = true; 
            
            if((sEventAppVibSensor[i].e_systick == 0) ||
               ((HAL_GetTick() - sEventAppVibSensor[i].e_systick) >= sEventAppVibSensor[i].e_period))
            {
                sEventAppVibSensor[i].e_status = 0; //Disable event
                sEventAppVibSensor[i].e_systick= 0;
                sEventAppVibSensor[i].e_function_handler(i);
            }
        }
    }
    
    
    return Result;
}


