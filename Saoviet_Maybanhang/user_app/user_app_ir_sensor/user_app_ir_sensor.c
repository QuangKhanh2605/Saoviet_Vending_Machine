#include "user_app_ir_sensor.h"

/*============== Function Static ==============*/
static uint8_t fevent_ir_sensor_1(uint8_t event);
static uint8_t fevent_ir_sensor_2(uint8_t event);

/*================= Struct ===================*/
sEvent_struct               sEventAppIrSensor[]=
{
  {_EVENT_IR_SENSOR_1,          0, 0, 5,    fevent_ir_sensor_1},
  {_EVENT_IR_SENSOR_2,          0, 0, 5,    fevent_ir_sensor_2},
};

/*================= Function Handle ==============*/
static uint8_t fevent_ir_sensor_1(uint8_t event)
{
    return 1;
}

static uint8_t fevent_ir_sensor_2(uint8_t event)
{
    return 1;
}

uint8_t AppIrSensor_Task(void)
{
    uint8_t i = 0;
    uint8_t Result = false;
    for(i = 0; i < _EVENT_IR_SENSOR_END; i++)
    {
        if(sEventAppIrSensor[i].e_status == 1)
        {
            Result = true;
            
            if((sEventAppIrSensor[i].e_systick == 0) ||
               ((HAL_GetTick() - sEventAppIrSensor[i].e_systick) >= sEventAppIrSensor[i].e_period))
            {
                sEventAppIrSensor[i].e_status = 0;  //Disable event
                sEventAppIrSensor[i].e_systick = 0;
                sEventAppIrSensor[i].e_function_handler(i);
            }
        }
    }
    
    
    return Result;
}

