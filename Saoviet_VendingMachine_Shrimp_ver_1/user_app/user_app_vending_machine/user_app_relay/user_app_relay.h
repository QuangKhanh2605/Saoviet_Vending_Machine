

#ifndef USER_APP_RELAY_H_
#define USER_APP_RELAY_H_

#define USING_APP_RELAY

#include "event_driven.h"
#include "user_util.h"

#define  TIME_LED_STATUS        2000
#define  TIME_LED_PCBOX         80
#define  TIME_LED_SLAVE         1000

#define GPIO_PIN_ON_RELAY       GPIO_PIN_SET
#define GPIO_PIN_OFF_RELAY      GPIO_PIN_RESET

#define NUMBER_RELAY            6

#define  TIME_RL_WARM           5   //Min

#define  TIME_RL_WARM_REFRESH   15  //Min

#define  RELAY_SCREEN_PORT          ON_OFF_SCREEN_GPIO_Port
//#define  RELAY_FRIDGE_COOL_PORT     ON_OFF_FRIDGE_COOL_GPIO_Port
#define  RELAY_ALARM_PORT           ON_OFF_ALARM_GPIO_Port
//#define  RELAY_FRIDGE_HEAT_PORT     ON_OFF_FRIDGE_HOT_GPIO_Port
#define  RELAY_LAMP_PORT            ON_OFF_LED_GPIO_Port
//#define  RELAY_WARM_PORT            GLASS_GPIO_Port
#define  RELAY_PC_PORT              ON_OFF_PC_GPIO_Port
#define  RELAY_V1_PORT              ON_OFF_V1_GPIO_Port
#define  RELAY_V2_PORT              ON_OFF_V2_GPIO_Port

#define  RELAY_SCREEN_PIN           ON_OFF_SCREEN_Pin 
//#define  RELAY_FRIDGE_COOL_PIN      ON_OFF_FRIDGE_COOL_Pin
#define  RELAY_ALARM_PIN            ON_OFF_ALARM_Pin
//#define  RELAY_FRIDGE_HEAT_PIN      ON_OFF_FRIDGE_HOT_Pin
#define  RELAY_LAMP_PIN             ON_OFF_LED_Pin
//#define  RELAY_WARM_PIN             GLASS_Pin
#define  RELAY_PC_PIN               ON_OFF_PC_Pin
#define  RELAY_V1_PIN               ON_OFF_V1_Pin
#define  RELAY_V2_PIN               ON_OFF_V2_Pin

typedef enum
{
    _EVENT_RELAY_ENTRY,
    
    _EVENT_RELAY_WARM_REFRESH,
    _EVENT_RELAY_WARM_ON,
    _EVENT_RELAY_WARM_OFF,
    
    _EVENT_CONTROL_LED_STATUS,
    _EVENT_CONTROL_LED_PCBOX,
    _EVENT_CONTROL_LED_SLAVE,
    
    _EVENT_RELAY_END,
}eKindEventRelay;

typedef enum
{
    _LED_STATUS = 0,
    _LED_PCBOX,
    _LED_SLAVE,
} Led_TypeDef;
    
typedef enum
{
    _RL_UNRESPOND = 0,
    _RL_RESPOND,
}eNumStateRespondPcBox;

typedef enum
{
    _RL_UNDEBUG=0,
    _RL_DEBUG,
}eNumStateDebug;

typedef enum
{
    _RL_CTRL,
    _RL_UNCTRL,
}eNumStateCtrlRelay;

typedef struct
{
    uint8_t Screen;
//    uint8_t FridgeCool;
    uint8_t Alarm;
//    uint8_t FridgeHeat;
    uint8_t Lamp;
//    uint8_t Warm;
    uint8_t Pc;
    
    uint8_t Screen_Ctrl;        //Trang thai relay dieu khien qua pcbox hoac lenh at+
//    uint8_t FridgeCool_Ctrl;    //Trang thai relay dieu khien qua pcbox hoac lenh at+
    uint8_t Alarm_Ctrl;         //Trang thai relay dieu khien qua pcbox hoac lenh at+
//    uint8_t FridgeHeat_Ctrl;    //Trang thai relay dieu khien qua pcbox hoac lenh at+
    uint8_t Lamp_Ctrl;          //Trang thai relay dieu khien qua pcbox hoac lenh at+
//    uint8_t Warm_Ctrl;          //Trang thai relay dieu khien qua pcbox hoac lenh at+
    uint8_t Pc_Ctrl;            //Trang thai relay dieu khien qua pcbox hoac lenh at+
}Struct_StatusRelay;

typedef enum
{
    RELAY_SCREEN=0,
    RELAY_ALARM,
    RELAY_LAMP,
    RELAY_PC,
    
    RELAY_V1,
    RELAY_V2,
}Relay_TypeDef;

typedef struct
{
    uint8_t Run;    //Thoi gian nghi relay warm
    uint8_t Wait;   //Thoi gian chay relay warm
}Struct_TimeCycleWarm;

typedef enum
{
    OFF_RELAY=0,
    ON_RELAY,
}eNumStateRelay;
extern sEvent_struct        sEventAppRelay[];

extern Struct_StatusRelay         sStatusRelay;
extern uint8_t                    LedRecvPcBox;
extern Struct_TimeCycleWarm       sTimeCycleWarm;
/*=============== Function handle ================*/
uint8_t     AppRelay_Task(void);
void        Init_AppRelay(void);

void        Save_StatusRelay(void);
void        Init_StatusRelay(void);
void        Setup_StatusRelay(void);

void        LED_Toggle (Led_TypeDef Led);
void        LED_On (Led_TypeDef Led);
void        LED_Off (Led_TypeDef Led);

void        OnOff_Relay(Relay_TypeDef Relay, uint8_t State);
void        Relay_Respond_Pc_Box(uint8_t State, uint8_t KindRelay, uint8_t Status);
void        Relay_Debug(uint8_t State_Debug, uint8_t KindRelay, uint8_t Status);
void        ControlRelay(uint8_t Relay, uint8_t State, uint8_t StateRespond, uint8_t RelayDebug, uint8_t RelayCtrl);

void        OnRelay_Warm(uint32_t time_min);
uint8_t     Save_TimeRelayWarm(uint8_t Run, uint8_t Wait);
void        Init_TimeRelayWarm(void);
#endif
