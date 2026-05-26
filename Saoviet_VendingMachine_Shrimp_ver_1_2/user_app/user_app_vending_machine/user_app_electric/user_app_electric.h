

#ifndef USER_APP_ELECTRIC_H_
#define USER_APP_ELECTRIC_H_

#define USING_APP_ELECTRIC

#include "event_driven.h"
#include "user_util.h"
#include "user_uart.h"

#define TIME_SEND_METER     60000
#define TIME_INIT_UART      15*60000

#define NUMBER_POWER_ON_OFF    3   //So lan dem xac nhan loi power

#define MACHINE_VOLTAGE_MIN 110

#define DEFAULT_ELECTRIC_VOL_CUR_SCALE  0xFF

#define DEFAULT_ELECTRIC_POW_ENE_SCALE  0xFD

#define DEFAULT_ID_SLAVE                0x01

#define DETECT_ELECTRIC_PORT                Door_Sensor_2_GPIO_Port  
#define DETECT_ELECTRIC_PIN                 Door_Sensor_2_Pin

typedef enum
{
    _EVENT_ELECTRIC_ENTRY,
    _EVENT_ELECTRIC_CHANGE_STATUS_POWER,
    _EVENT_ELECTRIC_CHECK_CONNECT,
    
    _EVENT_CHECK_POWER_SUPPLY,

    _EVENT_ELECTRIC_END,
}eKindEventElectric;

typedef enum
{
    POWER_OFF = 0,
    POWER_ON,
    POWER_ERROR,
}eStatusPower;

typedef struct 
{
    uint8_t  ID;
    uint16_t Voltage;
    uint16_t Current;
    int32_t  Power;
    uint32_t Energy;
    
    uint8_t  ScaleVolCur;   //Scale Votage Current
    uint8_t  ScalePowEne;   //Scale Power Energy
    uint8_t  PowerPresent;
    uint8_t  PowerBefore;
}Struct_Electric_Current;

extern sEvent_struct                sEventAppElectric[];
extern Struct_Electric_Current      sElectric;
/*============= Function handle ==============*/
uint8_t     AppElectric_Task(void);
void        Init_AppElectric(void);

void        Status_Power_Respond_PcBox(void);

void        Save_StatusElectric(void);
void        Init_StatusElectric(void);

void        Save_IdSlaveElectric(void);
void        Init_IdSlaveElectric(void);

void        AppElectric_Debug(void);
void        Handle_State_Power(void);
#endif

