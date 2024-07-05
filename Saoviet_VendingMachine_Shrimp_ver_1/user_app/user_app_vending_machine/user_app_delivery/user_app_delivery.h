


#ifndef USER_APP_H_
#define USER_APP_H_

#define USING_APP_DELIVERY

#include "event_driven.h"
#include "user_util.h"


#define DEFAULT_LEVEL_PWM_ELEVATOR      100
#define TIME_MOTOR_PUSH_EARLY           1000
#define TIME_MOTOR_PUSH_LATE            4500
#define TIME_MOTOR_RESPOND_PC_BOX       TIME_MOTOR_PUSH_LATE + 2000

#define NUMBER_MOTOR                    10
#define NUMBER_PUSH_MOTOR               10

#define NUMBER_MAX_FLOOR                5

#define LEVEL_PWM_ELEVATOR_UP           100
#define LEVEL_PWM_ELEVATOR_DOWN         65
#define LEVEL_PWM_ELEVATOR_OFF          0

#define LEVEL_PWM_ELEVATOR_UP_REDUCE    100
#define LEVEL_PWM_ELEVATOR_DOWN_REDUCE  60

#define STATE_GPIO_ON                   GPIO_PIN_SET
#define STATE_GPIO_OFF                  GPIO_PIN_RESET

#define PW_LOCK_PORT                    PW_LOCK_GPIO_Port
#define PW_LOCK_PIN                     PW_LOCK_Pin
#define SW_LOCK_PORT                    LOCK_GPIO_Port
#define SW_LOCK_PIN                     LOCK_Pin
#define FB_LOCK_PORT                    FB_LOCK_GPIO_Port
#define FB_LOCK_PIN                     FB_LOCK_Pin

//#define ON_OFF_MOTOR_PWM_1_PORT         ON_OFF_PWM_1_GPIO_Port
//#define ON_OFF_MOTOR_PWM_2_PORT         ON_OFF_PWM_2_GPIO_Port

#define ON_OFF_MOTOR_PWM_1_PIN          ON_OFF_PWM_1_Pin
#define ON_OFF_MOTOR_PWM_2_PIN          ON_OFF_PWM_2_Pin

#define CTRL_MOTOR_1_PORT               CTRL_Motor_1_GPIO_Port
#define CTRL_MOTOR_2_PORT               CTRL_Motor_2_GPIO_Port
#define CTRL_MOTOR_3_PORT               CTRL_Motor_3_GPIO_Port
#define CTRL_MOTOR_4_PORT               CTRL_Motor_4_GPIO_Port
#define CTRL_MOTOR_5_PORT               CTRL_Motor_5_GPIO_Port
#define CTRL_MOTOR_6_PORT               CTRL_Motor_6_GPIO_Port
#define CTRL_MOTOR_7_PORT               CTRL_Motor_7_GPIO_Port
#define CTRL_MOTOR_8_PORT               CTRL_Motor_8_GPIO_Port
#define CTRL_MOTOR_9_PORT               CTRL_Motor_9_GPIO_Port
#define CTRL_MOTOR_10_PORT              CTRL_Motor_10_GPIO_Port

#define CTRL_MOTOR_1_PIN                CTRL_Motor_1_Pin
#define CTRL_MOTOR_2_PIN                CTRL_Motor_2_Pin
#define CTRL_MOTOR_3_PIN                CTRL_Motor_3_Pin
#define CTRL_MOTOR_4_PIN                CTRL_Motor_4_Pin
#define CTRL_MOTOR_5_PIN                CTRL_Motor_5_Pin
#define CTRL_MOTOR_6_PIN                CTRL_Motor_6_Pin
#define CTRL_MOTOR_7_PIN                CTRL_Motor_7_Pin
#define CTRL_MOTOR_8_PIN                CTRL_Motor_8_Pin
#define CTRL_MOTOR_9_PIN                CTRL_Motor_9_Pin
#define CTRL_MOTOR_10_PIN               CTRL_Motor_10_Pin



typedef enum
{
    _EVENT_MOTOR_ENTRY,
    _EVENT_WAIT_FREE_MOTOR,
    
    _EVENT_LOCK_MAGNETIS_BEGIN,
    _EVENT_ELEVATOR_LOWER_BEGIN,
    _EVENT_DELIVERY_ENTRY,
    _EVENT_CALCULATOR_COORDINATES,
    _EVENT_PUSH_MOTOR,
    _EVENT_ELEVATOR_LOWER_END,
    _EVENT_LOCK_MAGNETIS_END,
    _EVENT_FINISH_DELIVERY,

    _EVENT_HANDLE_ERROR_DELIVERY,
    _EVENT_FREE_ELEVATOR_LOWER,
    
    _EVENT_FIX_MOTOR,
    
    _EVENT_CONTROL_PWM_ELEVATOR,
    _EVENT_ALTER_PWM_ELEVATOR,
    _EVENT_CTRL_ELEVATOR_UPPER,
    _EVENT_CTRL_ELEVATOR_LOWER,
    
    _EVENT_TEST_ELEVATOR,
    _EVENT_ELEVATOR_FLOOR,

    _EVENT_MOTOR_END,
}eKindEventDelivery;

typedef enum
{
    PUSH_SUCCESS,
    PUSH_EARLY,
    PUSH_LATE,
}eStatePushMotor;

typedef enum
{
    ELEVATOR_STOP,
    ELEVATOR_UP,
    ELEVATOR_DOWN,
}eDirectionElevator;

typedef enum
{
    ELEVATOR_OFF,
    ELEVATOR_ON,
}eStateElevator;

typedef enum
{
    DELIVERY_FREE=0,
    DELIVERY_PURCHASE,
    DELIVERY_TEST_ELEVATOR,
    DELIVERY_FIX_MOTOR,
}eNumStatePush;

typedef enum
{
    _ELEVATOR_FLOOR,
    _ELEVATOR_TOP,
    _ELEVATOR_BOT,
    _ELEVATOR_ERROR,
}eNumKindStateElevator;

typedef enum
{
    MOTOR_PUSH_OFF,
    MOTOR_PUSH_ON,
}eNumStateMotorPush;

typedef enum
{
    _WEIGHT_485_DISCONNECT,
    _WEIGHT_485_CONNECT,
}eNumStateWeight485;

typedef struct
{
    uint8_t NumEarly;           //So lan loi Push som (Error)
    uint8_t NumLate;            //So lan loi Push muon (Error)
}struct_InforMotor;

typedef struct
{
    uint8_t StateHanlde;                //Trang thai Delivery (Free, On_Going, Test_Elevator)
    uint8_t aDataPush[NUMBER_MOTOR];    //Data tra hang
    uint8_t SumHandle;                  //Tong so luong phai xu ly
    uint8_t PosHandle;                  //Vi tri dang xu ly
    uint8_t StateConnectWeight;         //Trang thai ket noi slave weight 485
    uint16_t Weighing;                  //Can nang
    uint32_t LockMagnetis;
}Struct_ParamDelivery;

typedef struct
{
    uint8_t State;                      //Trang thai
    uint8_t Status;
    uint8_t FloorCurrent;               //Vi tri tang hien tai
    uint8_t FloorHandle;                //Tang tra hang
    uint8_t Level_PWM;
    uint8_t RunLevel_PWM;
    uint32_t Encoder_C1;
    uint32_t Encoder_C2;
    uint32_t TimeOnMotor;
}Struct_Elevator;

typedef struct
{
    uint8_t State;
    uint8_t aPulse[NUMBER_MOTOR];       //Data kiem tra xung
    uint32_t TimeOnMotor;           //Kiem tra thoi gian tra hang
}Struct_MotorPush;

typedef enum
{
    RESPOND_PUSH_MOTOR,
    RESPOND_FIX_MOTOR,
    RESPOND_COMPLETE_DELIVERY,
    RESPOND_WARNING_DELIVERY,
}eStateRespondPcBox;

extern sEvent_struct            sEventAppDelivery[];
extern Struct_ParamDelivery     sParamDelivery;
extern Struct_Elevator          sElevator;
/*============= Function =============*/
uint8_t     AppDelivery_Task(void);
void        Init_AppDelivery(void);

void        Off_Motor_Push(void);
void        On_Motor_Push(uint8_t Pos);
void        IRQ_Pluse_OffMotorPush(uint8_t Pos);

void        AppDelivery_Debug(uint8_t KindDelivery);

uint8_t     Fix_Motor(uint8_t Pos);
void        Reset_StateDelivery(void);

void        Delivery_Entry(void);
void        Control_Elevator(uint8_t Direction, uint8_t Level);
void        Respond_Delivery_PcBox(eStateRespondPcBox StateRes);
uint8_t     Check_Top_Bot_Elevator(void);

void        Enable_Handle_Idle_Delivery(uint8_t State, uint32_t timeout);
void        Disable_Handle_Idle_Delivery(void);

void        Debug_Encoder(void);
void        Control_LockMagnetis(GPIO_PinState StateCtrl);
void        Debug_Floor_Elevator(uint8_t Floor);

void        Enable_Test_Elevator(void);
void        Delivery_Handle_State(uint8_t State);
void        GPIO_Init_Pos_Elevator(uint8_t Kind);
#endif
