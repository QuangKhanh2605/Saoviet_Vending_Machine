
#ifndef USER_APP_TEMPERATURE_H_
#define USER_APP_TEMPERATURE_H_

#include "event_driver.h"
#include "user_util.h"

typedef enum
{
    _EVENT_TEMPERATURE_1,
    _EVENT_TEMPERATURE_2,
    
    _EVENT_TEMPERATURE_END,
}eKindEventTemperature;

extern  sEvent_struct       sEventAppTemperature[];

/*=============== Function ================*/
uint8_t     AppTemperature_Task(void);


#endif

