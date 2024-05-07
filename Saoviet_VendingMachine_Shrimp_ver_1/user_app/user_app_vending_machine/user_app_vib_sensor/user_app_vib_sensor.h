

#ifndef USER_APP_VIB_SENSOR_H_
#define USER_APP_VIB_SENSOR_H_

#define USING_APP_VIB_SENSOR

#include "event_driven.h"
#include "user_util.h"

#define TIME_LED_TOGGLE     150
#define NUMBER_LED_TOGGLE   1

#define TIME_LEVEL_ALARM                60000

#define TIME_ON_ALARM_1                 3000
#define TIME_ON_ALARM_2                 10000

typedef enum
{
    _EVENT_VIB_SENSOR_ENTRY,
    _EVENT_VIB_SENSOR_SCAN,
    _EVENT_VIB_SENSOR,
    _EVENT_VIB_OFF_ALARM,
    _EVENT_VIB_LED_WARNING,
    
    _EVENT_VIB_SENSOR_END,
}eKindEventVibSensor;

typedef struct
{
    uint32_t Sensor1;
    uint32_t Sensor1_Scan;
    uint8_t  LevelWarning;       //level rung canh bao
}StructStatusVib;

extern sEvent_struct       sEventAppVibSensor[];
extern StructStatusVib     sStatusVib;
/*================= Function Handle ================*/
uint8_t     AppVibSensor_Task(void);

void        Log_Data_Vib_Respond_PcBox(void);
void        AppVibSensor_Debug(void);



#endif

