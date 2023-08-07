

#ifndef USER_APP_DOOR_SENSOR_H_
#define USER_APP_DOOR_SENSER_H_

#define USING_APP_DOOR_SENSOR

#include "event_driven.h"
#include "user_util.h"

typedef enum
{
    _EVENT_DOOR_ENTRY,
    _EVENT_DOOR_SENSOR_1,
    _EVENT_DOOR_SENSOR_2,
    _EVENT_DOOR_RESPOND_PC_BOX,
    
    _EVENT_DOOR_SENSOR_END,
}eKindEventDoorSensor;

typedef struct 
{
    uint8_t Sensor1;
    uint8_t Sensor2;
}StructStatusDoor;

extern sEvent_struct    sEventAppDoorSensor[];

/*=============== Function Hanlde ================*/
uint8_t     AppDoorSensor_Task(void);
uint8_t     Log_Data_Door(uint8_t *aData);


#endif


