

#ifndef USER_APP_RELAY_H_
#define USER_APP_RELAY_H_

#define USING_APP_RELAY

#include "event_driven.h"
#include "user_util.h"

#define  TIME_LED_STATUS    2000
#define  TIME_LED_PCBOX     80
#define  TIME_LED_SLAVE     1000

typedef enum
{
    _EVENT_RELAY_ENTRY,
    _EVENT_ON_OFF_RELAY_ELEVATOR,
    _EVENT_ON_OFF_RELAY_SCREEN,
    _EVENT_ON_OFF_RELAY_FRIDGE_COOL,
    _EVENT_ON_OFF_RELAY_ALARM,
    _EVENT_ON_OFF_RELAY_FRIDGE_HEAT,
    _EVENT_ON_OFF_RELAY_LAMP,
    _EVENT_ON_OFF_RELAY_WARM,
    
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
    DISCONNECT,
    CONNECT,
}eNumConnectSlave;

typedef struct
{
    uint8_t Respond_PcBox;
    uint8_t Elevator;
    uint8_t Screen;
    uint8_t FridgeCool;
    uint8_t Alarm;
    uint8_t FridgeHeat;
    uint8_t Lamp;
    uint8_t Warm;
}Struct_StatusRelay;

typedef enum
{
    RELAY_ELEVATOR,
    RELAY_SCREEN,
    RELAY_FRIDGE_COOL,
    RELAY_ALARM,
    RELAY_FRIDGE_HEAT,
    RELAY_LAMP,
    RELAY_WARM,
}Relay_TypeDef;
extern sEvent_struct        sEventAppRelay[];

extern Struct_StatusRelay         sStatusRelay;
extern uint8_t                    LedRecvPcBox;
/*=============== Function handle ================*/

uint8_t     AppRelay_Task(void);
void        Init_AppRelay(void);
void        On_Relay(Relay_TypeDef Relay);
void        Off_Relay(Relay_TypeDef Relay);

void        AppRelay_Debug(uint8_t Status, uint8_t Relay);
void        Relay_Respond_Pc_Box_Control(uint8_t Obis, uint8_t Data);

void        Write_Status_Relay_ExFlash(void);
void        Read_Status_Relay_ExFlash(void);
void        Init_StatusRelay(void);

void        LED_Toggle (Led_TypeDef Led);
void        LED_On (Led_TypeDef Led);
void        LED_Off (Led_TypeDef Led);

#endif
