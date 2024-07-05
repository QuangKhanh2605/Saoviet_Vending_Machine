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
#include "user_external_flash.h"
#include "user_inc_vending_machine.h"
#include "user_adc.h"
#include "math.h"
/*============== Function Static ================*/
static uint8_t fevent_temp_entry(uint8_t event);
static uint8_t fevent_temp_get_adc(uint8_t event);
static uint8_t fevent_temp_calculator(uint8_t event);
static uint8_t fevent_temp_set_setuptemp(uint8_t event);
static uint8_t fevent_temp_ctrl_fridge(uint8_t event);
static uint8_t fevent_temp_time_get(uint8_t event);
static uint8_t fevent_temp_off_fridge_frozen(uint8_t event);
static uint8_t fevent_temp_respond_error(uint8_t event);
/*=================== Struct ====================*/
sEvent_struct                   sEventAppTemperature[] =
{
  {_EVENT_TEMP_ENTRY,           1, 5, TIME_ON_DCU,          fevent_temp_entry},
  {_EVENT_TEMP_GET_ADC,         0, 0, 10,                   fevent_temp_get_adc},
  {_EVENT_TEMP_CALCULATOR,      0, 0, 0,                    fevent_temp_calculator},
    
  {_EVENT_TEMP_SET_SETUPTEMP,   0, 0, 0,                    fevent_temp_set_setuptemp},
  
  {_EVENT_TEMP_CTRL_FRIDGE,     0, 0, 0,                    fevent_temp_ctrl_fridge},
  
  {_EVENT_TEMP_TIME_GET,        0, 0, TIME_GET_TEMP,        fevent_temp_time_get},
  {_EVENT_TEMP_OFF_FRIGE_FROZEN,0, 0, TIME_OFF_FROZEN,      fevent_temp_off_fridge_frozen},
  
  {_EVENT_TEMP_RESPOND_ERROR,   0, 0, TIME_ERROR_TEMP,      fevent_temp_respond_error},
};

uint32_t ADC_Temp[3]={0};
uint32_t ADC_Avg[3]={0};
Struct_Temperature      sTemperature={0};
Struct_Temperature      sTemp_Thresh_Recv={0};
Struct_Control_Fridge   sTemp_Crtl_Fridge=
{
  .TempSetup = DEFAULT_SETUP_TEMP, 
  .Threshold = DEFAULT_THRESH_TEMP, 
  .Scale     = DEFAULT_TEMP_SCALE,
};
/*=================== Function Handle ============*/
static uint8_t fevent_temp_entry(uint8_t event)
{
/*----------------Lay ADC lan dau----------------*/
    sEventAppTemperature[_EVENT_TEMP_ENTRY].e_period = 20;
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
/*----------------Get ADC-----------------*/
    static uint8_t count_adc = 0;
    uint8_t status = HAL_ERROR; 
    status = HAL_ADC_Start_DMA(&ADC_TEMPERATURE, (uint32_t*)ADC_Temp, 3);
    if( status == HAL_OK)
    {
        ADC_Avg[0] += ADC_Temp[0];
        ADC_Avg[1] += ADC_Temp[1];
        ADC_Avg[2] += ADC_Temp[2];
    } 
    count_adc++;
    if(count_adc >= NUM_SAMPLING_ADC)
    {
        count_adc = 0;
        fevent_active(sEventAppTemperature, _EVENT_TEMP_CALCULATOR);
        return 1;
    }
    
    fevent_enable(sEventAppTemperature, event);
    return 1;
}

static uint8_t fevent_temp_calculator(uint8_t event)
{
/*------------------Calculator Temperature----------------*/
    float Resistor = 0;
    float Vpower = 0;
    float Vcc = 0;
    float Vs  = 0;
//    float VinH = 0;
    //float VinL = 0;
    float Isensor = 0;
    float LogSensor = 0;
    float TempC = 0;
    
    uint32_t B_Const = 3950;
    
    ADC_Avg[0] = ADC_Avg[0]/NUM_SAMPLING_ADC;
    ADC_Avg[1] = ADC_Avg[1]/NUM_SAMPLING_ADC;
    ADC_Avg[2] = ADC_Avg[2]/NUM_SAMPLING_ADC;
    
    Vpower = ((VDDA_VREFINT_CAL * (*VREFINT_CAL))/(float)ADC_Avg[2])/1000;
    
    Vs  = ((ADC_Avg[0]* Vpower)/ADC_RESOLUTION );
    Vcc = ((ADC_Avg[1]* Vpower)/ADC_RESOLUTION );
    
    Vs = (Vs/3);
    Isensor = (Vcc - Vs)/100000;
    Resistor = (Vs/Isensor);
    
    LogSensor = log (Resistor/10000);
    
    TempC = (1 / ((LogSensor / B_Const) + (1 / (25 + 273.15))));
    
    sTemperature.Value  =  (int16_t)((TempC - 273.15)* 10) ;
    sTemperature.Scale  = DEFAULT_TEMP_SCALE;
    
//    float param1 = -247.29;
//    float param2 = 2.3992;
//    float param3 = 0.00063962;
//    float param4 = 0.0000010241;
//    //float Stamp = 0;
//    
//    Vs  = ((ADC_Avg[0]* Vpower)/ADC_RESOLUTION );
//    Vcc = ((ADC_Avg[1]* Vpower)/ADC_RESOLUTION );
//    
//    VinH = (Vcc/(10000 + 120000/19))*(120000/19) + (float)0.002;
//    //VinL = VinH - (Vs/7);
//    
//    Resistor = (500*(VinH - Vs/7))/(Vcc - (VinH - Vs/7)) -120;
//    
//    sTemperature.Value  =  (int16_t)( (param1 + (Resistor)* ( param2 + (Resistor)* (param3 + param4 * (Resistor))))*10);
//    sTemperature.Value  = sTemperature.Value - CALIB_TEMP;
//    sTemperature.Scale  = DEFAULT_TEMP_SCALE;
    
    ADC_Avg[0] = 0;
    ADC_Avg[1] = 0;
    ADC_Avg[2] = 0;
    

    fevent_active(sEventAppTemperature, _EVENT_TEMP_CTRL_FRIDGE);

    
//    if(sTemperature.Value >500 || sTemperature.Value < -300)
//        fevent_active(sEventAppTemperature, _EVENT_TEMP_RESPOND_ERROR);
//    else
//        fevent_disable(sEventAppTemperature, _EVENT_TEMP_RESPOND_ERROR);
    
    return 1;
}
    
static uint8_t fevent_temp_set_setuptemp(uint8_t event)
{
/*-----------------Cai dat nhiet do---------------*/
    Set_Threshold_Temperature(sTemp_Thresh_Recv.Value, sTemp_Thresh_Recv.Scale);
    SetupTemp_Respond_Pc_Box_Setup();
    return 1;
}

void Init_Temp_Ctrl_Fridge(void)
{
/*-----------------Doc nhiet do duoc cai dat tu Flash---------------*/
    uint8_t read[4] = {0};
    eFlash_S25FL_BufferRead(read, EX_FLASH_ADDR_TEMP_SET_THRESH , 4);
    if( read[0] == BYTE_TEMP_FIRST_EXFLASH)
    {
        sTemp_Crtl_Fridge.TempSetup  = read[1] << 8;
        sTemp_Crtl_Fridge.TempSetup |= read[2];
        sTemp_Crtl_Fridge.Threshold  = read[3];
    }
}

static uint8_t fevent_temp_ctrl_fridge(uint8_t event)
{
/*---------------------Dieu khien dan nong va dan lanh theo nhiet do cai dat-----------------*/
    if(sElectric.PowerPresent != POWER_OFF)     //Neu khong mat dien cho pheo dieu khien dieu hoa
    {
        if(sTemperature.Value >= sTemp_Crtl_Fridge.TempSetup + sTemp_Crtl_Fridge.Threshold)
        {
            if(sStatusRelay.FridgeHeat == OFF_RELAY)
            ControlRelay(RELAY_FRIDGE_HEAT, ON_RELAY, _RL_RESPOND, _RL_UNDEBUG, _RL_UNCTRL);
            
            if(sStatusRelay.FridgeCool == OFF_RELAY)
            ControlRelay(RELAY_FRIDGE_COOL, ON_RELAY, _RL_RESPOND, _RL_UNDEBUG, _RL_UNCTRL);
            
            fevent_disable(sEventAppTemperature, _EVENT_TEMP_OFF_FRIGE_FROZEN);
            
            sStatusApp.Temperature = _APP_BUSY;
        }
        else if(sTemperature.Value <= sTemp_Crtl_Fridge.TempSetup - sTemp_Crtl_Fridge.Threshold)
        {
            if(sStatusRelay.FridgeHeat == ON_RELAY)
                ControlRelay(RELAY_FRIDGE_HEAT, OFF_RELAY, _RL_RESPOND, _RL_UNDEBUG, _RL_UNCTRL);

            fevent_enable(sEventAppTemperature, _EVENT_TEMP_OFF_FRIGE_FROZEN);
        }
    }
//    AppTemperature_Debug();
    fevent_enable(sEventAppTemperature, _EVENT_TEMP_TIME_GET);
    return 1;
}

static uint8_t fevent_temp_time_get(uint8_t event)
{
    fevent_active(sEventAppTemperature, _EVENT_TEMP_GET_ADC);
    return 1;
}

static uint8_t fevent_temp_off_fridge_frozen(uint8_t event)
{
/*---------------------Tat relay cuc nong---------------------*/
    if(sStatusRelay.FridgeCool == ON_RELAY)
    ControlRelay(RELAY_FRIDGE_COOL, OFF_RELAY, _RL_RESPOND, _RL_UNDEBUG, _RL_UNCTRL);
    
    sStatusApp.Temperature = _APP_FREE;
    return 1;
}

static uint8_t fevent_temp_respond_error(uint8_t event)
{
    Respond_Error_Temp();
      
    fevent_enable(sEventAppTemperature, event);
    return 1;
}

/*
    @brief  Phan hoi canh bao rung len PcBox
*/
void Respond_Error_Temp(void)
{ 
/*=============== Log ===============*/
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_WARNING_ERROR_TEMP;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
}

/*=============== Function Handle ============== */
/*
    @brief  Cai dat nhiet do va luu vao Flash
    @param  temp: Nhiet do cai dat
    @param  scale: Scale nhiet do cai dat
*/
void Set_Threshold_Temperature(int16_t temp, uint8_t scale)
{
    uint8_t write[4]={0x00};
    int16_t threshold = 0;
    threshold = Calculator_Scale_Int(temp , scale);
    sTemp_Crtl_Fridge.TempSetup = threshold;
    write[0] = BYTE_TEMP_FIRST_EXFLASH;
    write[1] = sTemp_Crtl_Fridge.TempSetup >> 8;
    write[2] = sTemp_Crtl_Fridge.TempSetup;
    write[3] = sTemp_Crtl_Fridge.Threshold;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_TEMP_SET_THRESH);
    eFlash_S25FL_BufferWrite(write, EX_FLASH_ADDR_TEMP_SET_THRESH, 4);
}

/*
    @brief  Phan hoi lai PcBox nhiet do cai dat
*/
void SetupTemp_Respond_Pc_Box_Setup(void)
{
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_SETUP_TEMP;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x03;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sTemp_Crtl_Fridge.TempSetup >> 8;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sTemp_Crtl_Fridge.TempSetup;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sTemp_Crtl_Fridge.Scale;
    
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
}

/*
    @brief  Debug
*/
void AppTemperature_Debug(void)
{
#ifdef  USING_APP_TEMPERATURE_DEBUG
    uint8_t aData[5]={0};
    uint8_t length = 0;
//    sprintf(c, "%d \r\n", sTemperature.Value);
    length = Convert_Int_To_String_Scale(aData, (int)sTemperature.Value, sTemperature.Scale);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_temperature: Temp: ", sizeof("app_temperature: Temp: ")-1);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"°C", sizeof("°C")-1);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" Setup: ", sizeof(" Setup: ")-1);
    length = Convert_Int_To_String_Scale(aData, (int)sTemp_Crtl_Fridge.TempSetup , sTemp_Crtl_Fridge.Scale);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"°C", sizeof("°C")-1);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" Thresh: ", sizeof(" Thresh: ")-1);
    length = Convert_Int_To_String_Scale(aData, (int)sTemp_Crtl_Fridge.Threshold , sTemp_Crtl_Fridge.Scale);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"°C", sizeof("°C")-1);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
    
    if(sStatusRelay.FridgeHeat == ON_RELAY)
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"Relay Heat: ON ", sizeof("Relay Heat: ON ")-1);
    else
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"Relay Heat: OFF ", sizeof("Relay Heat: OFF ")-1);
    
    if(sStatusRelay.FridgeCool == ON_RELAY)
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"Relay Cool: ON ", sizeof("Relay Cool: ON ")-1);
    else
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"Relay Cool: OFF ", sizeof("Relay Cool: OFF ")-1);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
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

void Init_AppTemperature(void)
{
    Init_Temp_Ctrl_Fridge();
    ADC_Init();
}

/*
    @brief  Init ADC
*/
void ADC_Init(void)
{
	HAL_ADC_Start_DMA(&ADC_TEMPERATURE, (uint32_t*) ADC_Temp,3);
}
