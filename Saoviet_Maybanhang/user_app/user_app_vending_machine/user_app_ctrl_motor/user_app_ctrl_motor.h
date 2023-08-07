


#ifndef USER_APP_H_
#define USER_APP_H_

#define USING_APP_CTRL_MOTOR

#include "event_driven.h"
#include "user_util.h"

typedef enum
{
    _EVENT_CONTROL_MOTOR_PUSH,
    _EVENT_INPUT_MOTOR_PUSH,
    _EVENT_MOTOR_PUSH_OFF_ERROR,
    _EVENT_RESPOND_PCBOX,

    _EVENT_MOTOR_END,
}eKindEventMotor;

typedef enum
{
    PUSH_SUCCESS,
    PUSH_EARLY,
    PUSH_LATE,
}eStatePushMotor;

typedef struct
{
    uint8_t Status;
    uint8_t Pos;
}struct_ControlMotor;

extern sEvent_struct sEventAppMotor[];

extern uint8_t PulseCount;
extern uint8_t PosPushMotor;
extern uint8_t Response_Ir_Sensor;
/*============= Function =============*/
uint8_t     AppMotor_Task(void);
void        Off_Motor_Push(void);
void        On_Motor_Push(uint8_t Pos);

#endif
