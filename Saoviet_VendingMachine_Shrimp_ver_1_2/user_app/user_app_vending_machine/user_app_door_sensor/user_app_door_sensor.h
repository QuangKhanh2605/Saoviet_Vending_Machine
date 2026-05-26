

#ifndef USER_APP_DOOR_SENSOR_H_
#define USER_APP_DOOR_SENSOR_H_

#define USING_APP_DOOR_SENSOR

#include "event_driven.h"
#include "user_util.h"

#define INIT_STATUS_DOOR_SENSOR_INPUT     GPIO_PIN_RESET
#define INIT_STATIS_DOOR_DELIVERY_INPUT   GPIO_PIN_SET

#define DOOR_SENSOR_PORT                  Door_Sensor_1_GPIO_Port          
#define DOOR_SENSOR_PIN                   Door_Sensor_1_Pin

//#define DOOR_DELIVERY_PORT                FB_LOCK_GPIO_Port  
//#define DOOR_DELIVERY_PIN                 FB_LOCK_Pin

typedef enum
{
    _EVENT_DOOR_ENTRY,
    _EVENT_DOOR_SENSOR,
    _EVENT_DOOR_CTRL_RESPOND,
    _EVENT_DOOR_RESPOND_PC_BOX,
    
    _EVENT_DOOR_DELIVERY,
    _EVENT_LOCK_MAGNETIS,
    
    _EVENT_DOOR_SENSOR_END,
}eKindEventDoorSensor;

typedef enum
{
    DOOR_OPEN,
    DOOR_CLOSE,
}eNumStatusDoor;

typedef enum
{
    LOCK_MAGNETIS_OPEN,
    LOCK_MAGNETIS_CLOSE,
}eNumStatusLockMagnetis;

typedef struct 
{
    uint8_t SensorMachine;
    uint8_t SensorDelivery;
    uint8_t Handle_Respond;         //Xu ly gui ban tin Server
}StructStatusDoor;

extern sEvent_struct    sEventAppDoorSensor[];
extern StructStatusDoor sStatusDoor;
/*=============== Function Hanlde ================*/
uint8_t     AppDoorSensor_Task(void);
void        Init_AppDoorSensor(void);
void        Log_Data_Door_Respond_PcBox(void);
void        AppDoorSensor_Debug(void);


#endif


