/*
    Scale
    00->1 
    FF->0.1 
    FE->0.01 
    FD->0.001  
    FC->0.0001  
    FB->0.00001 
*/
#include "user_app_temperature.h"
#include "user_comm_vending_machine.h"
#include "user_external_flash.h"
#include "user_app_relay.h"

/*============== Function Static ================*/
static uint8_t fevent_temp_entry(uint8_t event);
static uint8_t fevent_temp_get_adc(uint8_t event);
static uint8_t fevent_temp_calculator(uint8_t event);
static uint8_t fevent_temp_set_threshold(uint8_t event);
static uint8_t fevent_temp_read_threshold(uint8_t event);
static uint8_t fevent_temp_ctrl_fridge(uint8_t event);
static uint8_t fevent_temp_time_get(uint8_t event);
static uint8_t fevent_temp_off_fridge_frozen(uint8_t event);
/*=================== Struct ====================*/
sEvent_struct                   sEventAppTemperature[] =
{
  {_EVENT_TEMP_ENTRY,           1, 0, 20,                   fevent_temp_entry},
  {_EVENT_TEMP_GET_ADC,         0, 0, 10,                   fevent_temp_get_adc},
  {_EVENT_TEMP_CALCULATOR,      0, 0, 0,                    fevent_temp_calculator},
    
  {_EVENT_TEMP_SET_THRESHOLD,   0, 0, 0,                    fevent_temp_set_threshold},
  {_EVENT_TEMP_READ_THRESHOLD,  1, 0, 0,                    fevent_temp_read_threshold},
  
  {_EVENT_TEMP_CTRL_FRIDGE,     0, 0, 0,                    fevent_temp_ctrl_fridge},
  
  {_EVENT_TEMP_TIME_GET,        0, 0, TIME_GET_TEMP,        fevent_temp_time_get},
  {_EVENT_TEMP_OFF_FRIGE_FROZEN,0, 0, TIME_OFF_FROZEN,      fevent_temp_off_fridge_frozen},
};

uint32_t ADC_Temp[3]={0};
uint32_t ADC_Avg[3]={0};
Struct_Temperature      sTemperature={0};
Struct_Temperature      sTemp_Thresh_Recv={0};
int16_t Threshold_Ctrl = DEFAULT_THRESHOLD;
/*=================== Function Handle ============*/
static uint8_t fevent_temp_entry(uint8_t event)
{
    static uint8_t once = 0;
    once++;
    HAL_ADC_Start_DMA(&ADC_TEMPERATURE, (uint32_t*)ADC_Temp, 3);
    if(once == NUM_SAMPLING_ADC)
    {
        fevent_active(sEventAppTemperature, _EVENT_TEMP_GET_ADC);
        return 1;
    }
    fevent_enable(sEventAppTemperature, event);
    return 1; 
}

static uint8_t fevent_temp_get_adc(uint8_t event)
{
    static uint8_t count = 0;
    uint8_t status = HAL_ERROR; 
    status = HAL_ADC_Start_DMA(&ADC_TEMPERATURE, (uint32_t*)ADC_Temp, 3);
    if( status == HAL_OK)
    {
        count++;
        ADC_Avg[0] += ADC_Temp[0];
        ADC_Avg[1] += ADC_Temp[1];
        ADC_Avg[2] += ADC_Temp[2];
        if(count >= NUM_SAMPLING_ADC)
        {
            count = 0;
            fevent_active(sEventAppTemperature, _EVENT_TEMP_CALCULATOR);
            return 1;
        }
    }
    fevent_enable(sEventAppTemperature, event);
    return 1;
}


static uint8_t fevent_temp_calculator(uint8_t event)
{
    float Resistor = 0;
    float Vpower = 0;
    float Vcc = 0;
    float Vs  = 0;
    float VinH = 0;
    //float VinL = 0;
    
    ADC_Avg[0] = ADC_Avg[0]/NUM_SAMPLING_ADC;
    ADC_Avg[1] = ADC_Avg[1]/NUM_SAMPLING_ADC;
    ADC_Avg[2] = ADC_Avg[2]/NUM_SAMPLING_ADC;
    
    Vpower = ((VDDA_VREFINT_CAL * (*VREFINT_CAL))/(float)ADC_Avg[2])/1000;
    
    float param1 = -247.29;
    float param2 = 2.3992;
    float param3 = 0.00063962;
    float param4 = 0.0000010241;
    //float Stamp = 0;
    
    Vs  = ((ADC_Avg[0]* Vpower)/ADC_RESOLUTION );
    Vcc = ((ADC_Avg[1]* Vpower)/ADC_RESOLUTION );
    
    VinH = (Vcc/(10000 + 120000/19))*(120000/19) + (float)0.002;
    //VinL = VinH - (Vs/7);
    
    Resistor = (500*(VinH - Vs/7))/(Vcc - (VinH - Vs/7)) -120;
    
    sTemperature.Value  =  (int16_t)( (param1 + (Resistor)* ( param2 + (Resistor)* (param3 + param4 * (Resistor))))*10);
    sTemperature.Value  = sTemperature.Value - CALIB_TEMP;
    sTemperature.Scale  = DEFAULT_TEMP_SCALE;

    ADC_Avg[0] = 0;
    ADC_Avg[1] = 0;
    ADC_Avg[2] = 0;
    AppTemperature_Debug();
    
    fevent_active(sEventAppTemperature, _EVENT_TEMP_CTRL_FRIDGE);
    return 1;
}

static uint8_t fevent_temp_set_threshold(uint8_t event)
{
    uint8_t write[3]={0x00};
    int16_t threshold = 0;
    threshold = Calculator_Scale(sTemp_Thresh_Recv.Value ,sTemp_Thresh_Recv.Scale);
    Threshold_Ctrl = threshold;
    write[0] = 0xAA;
    write[1] = threshold >> 8;
    write[2] = threshold;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_TEMP_THRESH);
    HAL_Delay(1);
    eFlash_S25FL_BufferWrite(write, EX_FLASH_ADDR_TEMP_THRESH, 3);
    return 1;
}

static uint8_t fevent_temp_read_threshold(uint8_t event)
{
    uint8_t read[3] = {0};
    eFlash_S25FL_BufferRead(read, EX_FLASH_ADDR_TEMP_THRESH , 3);
    if( read[0] == 0xAA)
    {
        Threshold_Ctrl  = read[1] << 8;
        Threshold_Ctrl |= read[2];
    }
    return 1;
}

static uint8_t fevent_temp_ctrl_fridge(uint8_t event)
{
    if(sTemperature.Value > Threshold_Ctrl + THRESHOLD_UPPER)
    {
        sStatusRelay.FridgeHeat = ON_RELAY;
        fevent_active(sEventAppRelay, _EVENT_ON_OFF_RELAY_FRIDGE_HEAT);
        
        sStatusRelay.FridgeHeat = ON_RELAY;
        fevent_active(sEventAppRelay, _EVENT_ON_OFF_RELAY_FRIDGE_COOL);
        
        fevent_disable(sEventAppTemperature, _EVENT_TEMP_OFF_FRIGE_FROZEN);
    }
    else if(sTemperature.Value < Threshold_Ctrl - THRESHOLD_LOWER)
    {
        sStatusRelay.FridgeHeat = OFF_RELAY;
        fevent_active(sEventAppRelay, _EVENT_ON_OFF_RELAY_FRIDGE_HEAT);
        
        fevent_active(sEventAppTemperature, _EVENT_TEMP_OFF_FRIGE_FROZEN);
        sEventAppTemperature[_EVENT_TEMP_OFF_FRIGE_FROZEN].e_systick = HAL_GetTick();
    }
    
    fevent_active(sEventAppTemperature, _EVENT_TEMP_TIME_GET);
    sEventAppTemperature[_EVENT_TEMP_TIME_GET].e_systick = HAL_GetTick();
    return 1;
}

static uint8_t fevent_temp_time_get(uint8_t event)
{
    fevent_active(sEventAppTemperature, _EVENT_TEMP_GET_ADC);
    return 1;
}

static uint8_t fevent_temp_off_fridge_frozen(uint8_t event)
{
    sStatusRelay.FridgeCool = OFF_RELAY;
    fevent_active(sEventAppRelay, _EVENT_ON_OFF_RELAY_FRIDGE_COOL);
    return 1;
}

/*=============== Function Handle ============== */
void AppTemperature_Debug(void)
{
#ifdef  USING_APP_TEMPERATURE_DEBUG
    char cData[5]={0};
    uint8_t length = 0;
//    sprintf(c, "%d \r\n", sTemperature.Value);
    length = Convert_Int_To_String_Scale(cData, (int)sTemperature.Value, 0xFF);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_temperature: ", sizeof("app_temperature: "));
    UTIL_Printf(DBLEVEL_M, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" Threshold: ", sizeof(" Threshold: "));
    length = Convert_Int_To_String_Scale(cData, (int)Threshold_Ctrl, 0xFF);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n"));
#endif
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
