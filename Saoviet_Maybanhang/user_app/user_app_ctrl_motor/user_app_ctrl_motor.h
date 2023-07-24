


#ifndef USER_APP_H_
#define USER_APP_H_

#define USING_APP_CTRL_MOTOR

#include "event_driver.h"
#include "user_util.h"
#include "main.h"
typedef enum
{
    _EVENT_CONTROL_MOTOR_PUSH,
    _EVENT_INPTU_MOTOR_PUSH,

    _EVENT_MOTOR_END,
}eKindEventMotor;

typedef struct
{
    uint8_t Status;
    uint8_t Layer;
    uint8_t Slot;
}struct_ControlMotor;

extern sEvent_struct sEventAppMotor[];
extern struct_ControlMotor sControlMotor;
/*============= Function =============*/
uint8_t     AppMotor_Task(void);
void        Off_Motor_Push(void);

#endif
