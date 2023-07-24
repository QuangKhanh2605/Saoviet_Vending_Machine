

#ifndef USER_APP_ELECTRIC_H_
#define USER_APP_ELECTRIC_H_

#include "event_driver.h"
#include "user_util.h"

typedef enum
{
    _EVENT_ELECTRIC_1,
    _EVENT_ELECTRIC_2,
    
    _EVENT_ELECTRIC_END,
}eKindEventElectric;

extern sEvent_struct       sEventAppElectric[];

/*============= Function handle ==============*/

uint8_t     AppElectric_Task(void);

#endif

