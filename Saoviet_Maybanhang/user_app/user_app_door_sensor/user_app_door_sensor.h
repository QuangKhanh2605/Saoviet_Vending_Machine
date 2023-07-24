

#ifndef USER_APP_DOOR_SENSOR_H_
#define USER_APP_DOOR_SENSER_H_

#include "event_driver.h"
#include "user_util.h"

typedef enum
{
    _EVENT_DOOR_SENSOR_1,
    _EVENT_DOOR_SENSOR_2,
    
    _EVENT_DOOR_SENSOR_END,
}eKindEventDoorSensor;


extern sEvent_struct    sEventAppDoorSensor[];

/*=============== Function Hanlde ================*/
uint8_t     AppDoorSensor_Task(void);


#endif


