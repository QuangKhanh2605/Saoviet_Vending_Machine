

#ifndef USER_APP_RELAY_H_
#define USER_APP_RELAY_H_

#define USING_APP_RELAY

#include "event_driven.h"
#include "user_util.h"

typedef enum
{
    _EVENT_RELAY_ENTRY,
    _EVENT_ON_OFF_RELAY_PC,
    _EVENT_ON_OFF_RELAY_SCREEN,
    _EVENT_ON_OFF_RELAY_FRIDGE_COOL,
    _EVENT_ON_OFF_RELAY_ALARM,
    _EVENT_ON_OFF_RELAY_FRIDGE_HEAT,
    _EVENT_ON_OFF_RELAY_LAMP,
    _EVENT_ON_OFF_RELAY_WARM,
    
    _EVENT_RELAY_END,
}eKindEventRelay;

typedef struct
{
    uint8_t FridgeHeat;
    uint8_t FridgeCool;
    uint8_t Alarm;
    uint8_t PC;
    uint8_t Screen;
    uint8_t Lamp;
    uint8_t Warm;
}Struct_StatusRelay;

typedef enum
{
    RELAY_PC,
    RELAY_SCREEN,
    RELAY_FRIDGE_COOL,
    RELAY_ALARM,
    RELAY_FRIDGE_HEAT,
    RELAY_LAMP,
    RELAY_WARM,
}Relay_TypeDef;
extern sEvent_struct        sEventAppRelay[];

extern Struct_StatusRelay         sStatusRelay;
/*=============== Function handle ================*/

uint8_t     AppRelay_Task(void);

void        On_Relay(Relay_TypeDef Relay);
void        Off_Relay(Relay_TypeDef Relay);

void        AppRelay_Debug(uint8_t Status, uint8_t Relay);


#endif
