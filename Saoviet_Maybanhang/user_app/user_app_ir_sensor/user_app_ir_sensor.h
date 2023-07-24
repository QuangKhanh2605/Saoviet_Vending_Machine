

#ifndef USER_APP_IR_SENSOR_H_
#define USER_APP_IR_SENSOR_H_

#include "event_driver.h"
#include "user_util.h"

typedef enum
{
    _EVENT_IR_SENSOR_1,
    _EVENT_IR_SENSOR_2,
    
    _EVENT_IR_SENSOR_END,
}eKindEventIrSensor;

extern sEvent_struct    sEventAppIrSensor[];

/*============== Function ===============*/
uint8_t AppIrSensor_Task(void);

#endif

