

#ifndef USER_APP_IR_SENSOR_H_
#define USER_APP_IR_SENSOR_H_

#define USING_APP_IR_SENSOR

#include "event_driven.h"
#include "user_util.h"
#include "user_comm_vending_machine.h"

typedef enum
{
    _EVENT_IR_SENSOR_INPUT,
    _EVENT_IR_SENSOR_OUTPUT,
    
    _EVENT_IR_SENSOR_END,
}eKindEventIrSensor;

extern sEvent_struct    sEventAppIrSensor[];

extern uint8_t          Response_Ir_Sensor;
/*============== Function ===============*/
uint8_t AppIrSensor_Task(void);

#endif

