
#ifndef USER_APP_TEMPERATURE_H_
#define USER_APP_TEMPERATURE_H_

#define USING_APP_TEMPERATURE

#include "event_driver.h"
#include "user_util.h"
#include "adc.h"

#define ADC_TEMPERATURE     hadc1

typedef enum
{
    _EVENT_TEMP_GET_ADC,
    _EVENT_TEMPERATURE_2,
    
    _EVENT_TEMPERATURE_END,
}eKindEventTemperature;

extern  sEvent_struct       sEventAppTemperature[];

/*=============== Function ================*/
uint8_t     AppTemperature_Task(void);
void        ADC_Init(void);

#endif

