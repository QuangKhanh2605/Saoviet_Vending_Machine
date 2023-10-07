

#ifndef USER_APP_DOOR_SENSOR_H_
#define USER_APP_DOOR_SENSER_H_

#define USING_APP_DOOR_SENSOR

#include "event_driven.h"
#include "user_util.h"

typedef enum
{
    _EVENT_DOOR_ENTRY,
    _EVENT_DOOR_SENSOR,
    _EVENT_DOOR_CTRL_RESPOND,
    _EVENT_DOOR_RESPOND_PC_BOX,
    
    _EVENT_DOOR_SENSOR_END,
}eKindEventDoorSensor;

typedef enum
{
    DOOR_CLOSE,
    DOOR_OPEN,
}eNumStatusDoor;

typedef enum
{
    DOOR1_CLOSE_DOOR2_CLOSE,
    DOOR1_OPEN_DOOR2_CLOSE,
    DOOR1_CLOSE_DOOR2_OPEN,
    DOOR1_OPEN_DOOR2_OPEN,
}eNumStatusTwoDoor;

typedef struct 
{
    uint8_t Sensor1;
    uint8_t Sensor2;
    uint8_t Handle_Respond;         //Xu ly gui ban tin Server
}StructStatusDoor;

extern sEvent_struct    sEventAppDoorSensor[];

/*=============== Function Hanlde ================*/
uint8_t     AppDoorSensor_Task(void);
void        Log_Data_Door_Respond_PcBox(void);
void        AppDoorSensor_Debug(void);


#endif


