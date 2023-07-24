

#ifndef USER_APP_VIB_SENSOR_H_
#define USER_APP_VIB_SENSOR_H_

#include "event_driver.h"
#include "user_util.h"

typedef enum
{
    _EVENT_VIB_SENSOR_1,
    _EVENT_VIB_SENSOR_2,
    
    _EVENT_VIB_SENSOR_END,
}eKindEventVibSensor;

extern sEvent_struct       sEventAppVibSensor[];

/*================= Function Handle ================*/
uint8_t     AppVibSensor_Task(void);



#endif

