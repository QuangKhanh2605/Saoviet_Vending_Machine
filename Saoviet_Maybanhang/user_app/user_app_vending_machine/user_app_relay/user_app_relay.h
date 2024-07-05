

#ifndef USER_APP_RELAY_H_
#define USER_APP_RELAY_H_

#define USING_APP_RELAY

#include "event_driven.h"
#include "user_util.h"

#define  TIME_LED_STATUS        2000
#define  TIME_LED_PCBOX         80
#define  TIME_LED_SLAVE         1000

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
    DISCONNECT_SLAVE,
    CONNECT_SLAVE,
}eNumConnectSlave;
    
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
    uint8_t Elevator;
    uint8_t Screen;
    uint8_t FridgeCool;
    uint8_t Alarm;
    uint8_t FridgeHeat;
    uint8_t Lamp;
    uint8_t Warm;
    
    uint8_t Elevator_Ctrl;      //Trang thai relay dieu khien qua pcbox hoac lenh at+
    uint8_t Screen_Ctrl;        //Trang thai relay dieu khien qua pcbox hoac lenh at+
    uint8_t FridgeCool_Ctrl;    //Trang thai relay dieu khien qua pcbox hoac lenh at+
    uint8_t Alarm_Ctrl;         //Trang thai relay dieu khien qua pcbox hoac lenh at+
    uint8_t FridgeHeat_Ctrl;    //Trang thai relay dieu khien qua pcbox hoac lenh at+
    uint8_t Lamp_Ctrl;          //Trang thai relay dieu khien qua pcbox hoac lenh at+
    uint8_t Warm_Ctrl;          //Trang thai relay dieu khien qua pcbox hoac lenh at+
}Struct_StatusRelay;

typedef enum
{
    RELAY_ELEVATOR = 0,
    RELAY_SCREEN,
    RELAY_FRIDGE_COOL,
    RELAY_ALARM,
    RELAY_FRIDGE_HEAT,
    RELAY_LAMP,
    RELAY_WARM,
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
extern uint8_t                    ConnectSlave;
extern Struct_TimeCycleWarm       sTimeCycleWarm;
/*=============== Function handle ================*/

uint8_t     AppRelay_Task(void);
void        Init_AppRelay(void);

void        Write_Status_Relay_ExFlash(void);
void        Read_Status_Relay_ExFlash(void);
void        Init_StatusRelay(void);

void        LED_Toggle (Led_TypeDef Led);
void        LED_On (Led_TypeDef Led);
void        LED_Off (Led_TypeDef Led);

void        OnOff_Relay(Relay_TypeDef Relay, uint8_t State);
void        Relay_Respond_Pc_Box(uint8_t State, uint8_t KindRelay, uint8_t Status);
void        Relay_Debug(uint8_t State_Debug, uint8_t KindRelay, uint8_t Status);
void        ControlRelay(uint8_t Relay, uint8_t State, uint8_t StateRespond, uint8_t RelayDebug, uint8_t RelayCtrl);

void        OnRelay_Warm(uint32_t time_min);
void        Init_Time_Relay_Warm(void);
void        Write_Flash_Time_Relay_Warm(void);
void        Setup_TimeCycle_Relay_Warm(uint8_t TimeRunMin, uint8_t TimeWaitMin);

void        PcBox_Setup_Time_Warm_Run(uint8_t TimeRunWarm);
void        PcBox_Setup_Time_Warm_Wait(uint8_t TimeWaitWarm);
#endif
