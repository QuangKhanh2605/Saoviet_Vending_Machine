
#ifndef USER_APP_TEMPERATURE_H_
#define USER_APP_TEMPERATURE_H_

#define USING_APP_TEMPERATURE

#include "event_driven.h"
#include "user_util.h"
#include "adc.h"
#include "string.h"

#define ADC_TEMPERATURE     hadc1
#define TIME_ERROR_TEMP     60000

#if defined (STM32F405xx) 
    #define VREFINT_CAL         ((uint16_t*) ((uint32_t)  0x1FFF7A2A))  
#endif

#define NUM_SAMPLING_ADC        10

#define ADC_RESOLUTION                  4095
#define DEFAULT_TEMP_SCALE              0xFF

#define CALIB_TEMP                      10
#define DEFAULT_SETUP_TEMP              50
#define DEFAULT_THRESH_TEMP             25

#define TIME_OFF_FROZEN                 5*TIME_ONE_MINUTES

#define TIME_GET_TEMP                   10000

typedef enum
{
    _EVENT_TEMP_ENTRY,
    _EVENT_TEMP_GET_ADC,
    _EVENT_TEMP_CALCULATOR,
    
    _EVENT_TEMP_SET_SETUPTEMP,
    
    _EVENT_TEMP_CTRL_FRIDGE,
    _EVENT_TEMP_TIME_GET,
    _EVENT_TEMP_OFF_FRIGE_FROZEN,
    _EVENT_TEMP_RESPOND_ERROR,
    
    _EVENT_TEMPERATURE_END,
}eKindEventTemperature;

typedef struct
{
    int16_t Value;
    uint8_t Scale;
}Struct_Temperature;

typedef struct
{
    int16_t TempSetup;
    uint8_t Threshold;
    uint8_t Scale;
}Struct_Control_Fridge;

extern  sEvent_struct           sEventAppTemperature[];
extern  Struct_Temperature      sTemp_Thresh_Recv;
extern  Struct_Temperature      sTemperature;
extern  Struct_Control_Fridge   sTemp_Crtl_Fridge;
/*=============== Function ================*/
uint8_t     AppTemperature_Task(void);
void        Init_AppTemperature(void);
void        ADC_Init(void);
void        AppTemperature_Debug(void);
void        SetupTemp_Respond_Pc_Box_Setup(void);
void        Set_Threshold_Temperature(int16_t temp, uint8_t scale);
void        Respond_Error_Temp(void);
void        Init_Temp_Ctrl_Fridge(void);
#endif

