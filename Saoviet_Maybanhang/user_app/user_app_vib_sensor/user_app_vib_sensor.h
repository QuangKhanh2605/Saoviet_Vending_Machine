

#ifndef USER_APP_VIB_SENSOR_H_
#define USER_APP_VIB_SENSOR_H_

#define USING_APP_VIB_SENSOR

#include "event_driver.h"
#include "user_util.h"
#include "main.h"

typedef enum
{
    _EVENT_VIB_SENSOR_1,
    _EVENT_VIB_SENSOR_2,
    _EVENT_VIB_SENSOR_3,
    
    _EVENT_VIB_SENSOR_END,
}eKindEventVibSensor;

typedef struct
{
    uint8_t Sensor1;
    uint8_t Sensor2;
    uint8_t Sensor3;
}StructStatusVib;

extern sEvent_struct       sEventAppVibSensor[];

/*================= Function Handle ================*/
uint8_t     AppVibSensor_Task(void);



#endif

