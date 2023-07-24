#include "user_app_temperature.h"

/*============== Function Static ================*/
static uint8_t fevent_temp_get_adc(uint8_t event);
static uint8_t fevent_temperature_2(uint8_t event);

/*=================== Struct ====================*/
sEvent_struct                   sEventAppTemperature[] =
{
  {_EVENT_TEMP_GET_ADC,       0, 0, 5,      fevent_temp_get_adc},
  {_EVENT_TEMPERATURE_2,      0, 0, 5,      fevent_temperature_2},
};

uint16_t ADC_Temp[2];
/*=================== Function Handle ============*/

static uint8_t fevent_temp_get_adc(uint8_t event)
{
    HAL_ADC_Start_DMA(&ADC_TEMPERATURE, (uint32_t*)ADC_Temp, 2);
    fevent_enable(sEventAppTemperature, event);
    return 1;
}

static uint8_t fevent_temperature_2(uint8_t event)
{
    return 2;
}

uint8_t AppTemperature_Task(void)
{
    uint8_t i = 0;
    uint8_t Result = false;
    
    for(i = 0; i < _EVENT_TEMPERATURE_END; i++)
    {
        if(sEventAppTemperature[i].e_status == 1)
        {
            Result = true;
            
            if((sEventAppTemperature[i].e_systick == 0) ||
               ((HAL_GetTick() - sEventAppTemperature[i].e_systick) >= sEventAppTemperature[i].e_period))
            {
                sEventAppTemperature[i].e_status = 0; //Disable event
                sEventAppTemperature[i].e_systick= HAL_GetTick();
                sEventAppTemperature[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}

void ADC_Init(void)
{
	HAL_ADC_Start_DMA(&ADC_TEMPERATURE, (uint32_t*) ADC_Temp,2);
}
