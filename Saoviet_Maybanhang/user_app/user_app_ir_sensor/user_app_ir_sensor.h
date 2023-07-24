

#ifndef USER_APP_IR_SENSOR_H_
#define USER_APP_IR_SENSOR_H_

#define USING_APP_IR_SENSOR

#include "event_driver.h"
#include "user_util.h"
#include "main.h"

typedef enum
{
    _EVENT_IR_SENSOR_OUTPUT,
    _EVENT_IR_SENSOR_INPUT,
    
    _EVENT_IR_SENSOR_END,
}eKindEventIrSensor;

extern sEvent_struct    sEventAppIrSensor[];

/*============== Function ===============*/
uint8_t AppIrSensor_Task(void);

#endif

