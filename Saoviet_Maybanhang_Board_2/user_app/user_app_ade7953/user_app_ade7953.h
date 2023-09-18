
#ifndef USER_APP_ADE7953_H_
#define USER_APP_ADE7953_H_

#define USING_APP_ADE7953

#include "stm32l0xx_hal.h"
#include "event_driven.h"

#define true    1
#define false   0

#define TIME_LED_STATUS     2000
#define VOLTAGE_ACTIVE_ADE  25
#define TIME_SAVE_ENERGY    2*60*60*1000
#define NUMBER_SAVE_ENERGY  15

typedef enum
{
    _EVENT_ADE_ENTRY,
    _EVENT_ADE_CHECK_SCOURCE,
    _EVENT_ADE_HANDLE,
    
    _EVENT_ADE_SAVE_ENERGY,
    
    _EVENT_CTRL_LED_STATUS,
    
    _EVENT_ADE_END,
}eKindenum;

typedef struct
{
    uint16_t Voltage;
    uint16_t Current;
    int32_t  Power;
    uint32_t Energy;
}Struct_Infor_Electric;

typedef struct 
{
    uint64_t Reg_Energy;
    uint64_t Dis_Energy;
    uint64_t Pre_Energy;
}Struct_Infor_Energy;

extern sEvent_struct               sEventAppAde7953[];
extern Struct_Infor_Electric       sInforElectric;
extern Struct_Infor_Energy         sInforEnergy;
/*============= Function =============*/
uint8_t     AppAde7953_Task(void);

void        Init_Ade7953(void);
void        Init_Energy(void);

#endif

