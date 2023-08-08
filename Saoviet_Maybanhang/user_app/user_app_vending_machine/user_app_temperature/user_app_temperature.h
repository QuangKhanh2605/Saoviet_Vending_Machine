
#ifndef USER_APP_TEMPERATURE_H_
#define USER_APP_TEMPERATURE_H_

#define USING_APP_TEMPERATURE

#include "event_driven.h"
#include "user_util.h"
#include "adc.h"
#include "string.h"

#define ADC_TEMPERATURE     hadc1

#if defined (STM32F405xx) 
    #define VREFINT_CAL         ((uint16_t*) ((uint32_t)  0x1FFF7A2A))  
#endif

#define VDDA_VREFINT_CAL        ((uint32_t) 3300)  
#define NUM_SAMPLING_ADC        100

#define DEFAULT_THRESHOLD       100
#define TIME_ONE_MINUTES        60*1000
#define TIME_OFF_FROZEN         10*TIME_ONE_MINUTES

#define THRESHOLD_UPPER         25
#define THRESHOLD_LOWER         25

#define TIME_GET_TEMP           30000

typedef enum
{
    _EVENT_TEMP_ENTRY,
    _EVENT_TEMP_GET_ADC,
    _EVENT_TEMP_CALCULATOR,
    
    _EVENT_TEMP_SET_THRESHOLD,
    _EVENT_TEMP_READ_THRESHOLD,
    
    _EVENT_TEMP_CTRL_FRIDGE,
    _EVENT_TEMP_TIME_GET,
    _EVENT_TEMP_OFF_FRIGE_FROZEN,
    
    _EVENT_TEMPERATURE_END,
}eKindEventTemperature;

typedef struct
{
    int16_t Value;
    uint8_t Scale;
}Struct_Temperature;

extern  sEvent_struct           sEventAppTemperature[];
extern  Struct_Temperature      sTemp_Thresh_Recv;
extern  Struct_Temperature      sTemperature;
extern  int16_t                 Threshold_Ctrl;
/*=============== Function ================*/
uint8_t     AppTemperature_Task(void);
void        ADC_Init(void);
void        AppTemperature_Debug(void);

#endif

