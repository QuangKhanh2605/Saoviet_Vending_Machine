


#ifndef USER_APP_H_
#define USER_APP_H_

#define USING_APP_CTRL_MOTOR

#include "event_driven.h"
#include "user_util.h"

#define TIME_MOTOR_PUSH_EARLY           1000
#define TIME_MOTOR_PUSH_LATE            3000
#define TIME_MOTOR_RESPOND_PC_BOX       TIME_MOTOR_PUSH_LATE + 2000

typedef enum
{
    _EVENT_MOTOR_ENTRY,
    _EVENT_CONTROL_MOTOR_PUSH,
    _EVENT_INPUT_MOTOR_PUSH,
    _EVENT_MOTOR_PUSH_OFF_ERROR,
    _EVENT_RESPOND_PCBOX,
    _EVENT_WAIT_FREE_MOTOR,

    _EVENT_MOTOR_END,
}eKindEventMotor;

typedef enum
{
    PUSH_SUCCESS,
    PUSH_EARLY,
    PUSH_LATE,
}eStatePushMotor;

typedef enum 
{
    FIX_MOTOR,
    PUSH_MOTOR,
}eStateCtrlMotor;

typedef struct
{
    uint8_t State;              //Trang thai Push hoac Fix Motor
    uint8_t SumHandle;          //Tong so lan Push
    uint8_t Pos;                //Vi tri Push
    uint8_t NumHandle;          //So thu tu dang xu ly
    uint8_t StatePush;          //Trang thai chua hoac da hoan thanh Push 
    uint8_t PulseCount;         //Dem xung Motor
    uint8_t IrSensor;           //Dem Ir sensor    
}struct_ControlMotor;

typedef struct
{
    uint8_t NumEarly;           //So lan loi Push som (Error)
    uint8_t NumLate;            //So lan loi Push muon (Error)
    uint8_t IrSensor;           //Tong so lan dem Ir Sensor (Error)
}struct_InforMotor;

typedef enum
{
    COMPLETE_PUSH=0,
    ON_GOING_PUSH,
}eNumStatePush;

extern sEvent_struct            sEventAppMotor[];
extern struct_ControlMotor      sPushMotor;
/*============= Function =============*/
uint8_t     AppMotor_Task(void);
void        Off_Motor_Push(void);
void        On_Motor_Push(uint8_t Pos);
void        AppMotor_Debug(void);

void        Fix_Motor(uint8_t Pos);
void        Push_Motor(uint8_t Pos, uint8_t Number);

#endif
