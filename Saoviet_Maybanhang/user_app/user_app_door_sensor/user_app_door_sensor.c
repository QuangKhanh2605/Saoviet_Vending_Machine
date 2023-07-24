
#include "user_app_door_sensor.h"

/*================= Function static ==============*/
static uint8_t fevent_door_sensor_1(uint8_t event);
static uint8_t fevent_door_sensor_2(uint8_t event);

/*================= Struct ======================*/
sEvent_struct           sEventAppDoorSensor[]=
{
  {_EVENT_DOOR_SENSOR_1,            0, 0, 5,      fevent_door_sensor_1},
  {_EVENT_DOOR_SENSOR_2,            0, 0, 5,      fevent_door_sensor_2},
};

StructStatusDoor        sStatusDoor = {0};
/*================== Function Handle =================*/

static uint8_t fevent_door_sensor_1(uint8_t event)
{
    if(HAL_GPIO_ReadPin(Door_Sensor_1_GPIO_Port, Door_Sensor_1_Pin) == INIT_STATUS_DOOR_SENSOR_INPUT)
    {
        sStatusDoor.Sensor1 = 1;
    }
    else
    {
        sStatusDoor.Sensor1 = 0;
    }

    fevent_enable(sEventAppDoorSensor, event);
    return 1;
}

static uint8_t fevent_door_sensor_2(uint8_t event)
{
    if(HAL_GPIO_ReadPin(Door_Sensor_2_GPIO_Port, Door_Sensor_2_Pin) == INIT_STATUS_DOOR_SENSOR_INPUT)
    {
        sStatusDoor.Sensor2 = 1;
    }
    else
    {
        sStatusDoor.Sensor2 = 0;
    }
    
    fevent_enable(sEventAppDoorSensor, event);
    return 1;
}

uint8_t AppDoorSensor_Task(void)
{
    uint8_t i = 0;
    uint8_t Result = false;
    
    for(i = 0; i < _EVENT_DOOR_SENSOR_END; i++)
    {
        if(sEventAppDoorSensor[i].e_status == 1)
        {
            Result = true;
            
            if((sEventAppDoorSensor[i].e_systick == 0) || 
               ((HAL_GetTick() - sEventAppDoorSensor[i].e_systick) >= sEventAppDoorSensor[i].e_period))
            {
                sEventAppDoorSensor[i].e_status = 0; //Disable event
                sEventAppDoorSensor[i].e_systick= HAL_GetTick();
                sEventAppDoorSensor[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}


