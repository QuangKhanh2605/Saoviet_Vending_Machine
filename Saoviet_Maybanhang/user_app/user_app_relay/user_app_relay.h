

#ifndef USER_APP_RELAY_H_
#define USER_APP_RELAY_H_

#define USING_APP_RELAY

#include "event_driver.h"
#include "user_util.h"
#include "main.h"
typedef enum
{
    _EVENT_RELAY_PC,
    _EVENT_RELAY_SCREEN,
    _EVENT_RELAY_FRIDGE,
    _EVENT_RELAY_ALARM,
    _EVENT_RELAY_5,
    
    _EVENT_RELAY_END,
}eKindEventRelay;

typedef struct
{
    uint8_t Status_Recv;
    uint16_t Status_Relay;
}Struct_Relay;

typedef enum
{
    RELAY_PC,
    RELAY_SCREEN,
    RELAY_FRIDGE,
    RELAY_ALARM,
    RELAY_5,
}Relay_TypeDef;
extern sEvent_struct        sEventAppRelay[];

extern Struct_Relay         sRelay_PC;
extern Struct_Relay         sRelay_Screen;
extern Struct_Relay         sRelay_Fridge;
extern Struct_Relay         sRelay_Alarm;
extern Struct_Relay         sRelay_Relay5;
/*=============== Function handle ================*/

uint8_t     AppRelay_Task(void);

void        On_Relay(Relay_TypeDef Relay);
void        Off_Relay(Relay_TypeDef Relay);


#endif
