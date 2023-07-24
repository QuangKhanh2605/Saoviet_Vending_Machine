

#ifndef USER_APP_RELAY_H_
#define USER_APP_RELAY_H_

#include "event_driver.h"
#include "user_util.h"

typedef enum
{
    _EVENT_RELAY_1,
    _EVENT_RELAY_2,
    
    _EVENT_RELAY_END,
}eKindEventRelay;


extern sEvent_struct        sEventAppRelay[];

/*=============== Function handle ================*/

uint8_t     AppRelay_Task(void);


#endif
