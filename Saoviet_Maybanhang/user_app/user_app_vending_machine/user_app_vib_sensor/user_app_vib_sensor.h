

#ifndef USER_APP_VIB_SENSOR_H_
#define USER_APP_VIB_SENSOR_H_

#define USING_APP_VIB_SENSOR

#include "event_driven.h"
#include "user_util.h"

#define TIME_LEVEL_ALARM    60000

#define TIME_ON_ALARM_1     5000
#define TIME_ON_ALARM_2     10000
#define TIME_LED_TOGGLE     200
#define NUMBER_LED_TOGGLE   1

typedef enum
{
    _EVENT_VIB_SENSOR,
    _EVENT_VIB_OFF_ALARM,
    _EVENT_VIB_LED_WARNING,
    
    _EVENT_VIB_SENSOR_END,
}eKindEventVibSensor;

typedef struct
{
    uint32_t Sensor1;
    uint32_t Sensor2;
    uint32_t Sensor3;
    uint8_t  LevelWarning;       //level rung canh bao
}StructStatusVib;

extern sEvent_struct       sEventAppVibSensor[];
extern StructStatusVib     sStatusVib;

/*================= Function Handle ================*/
uint8_t     AppVibSensor_Task(void);

uint8_t     Log_Data_Vib(uint8_t *aData);
void        AppVibSensor_Debug(void);



#endif

