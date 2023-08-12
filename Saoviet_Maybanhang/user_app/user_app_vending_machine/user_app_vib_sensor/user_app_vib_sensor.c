
#include "user_app_vib_sensor.h"
#include "user_comm_vending_machine.h"
#include "user_app_relay.h"
/*=============== Function static ================*/
static uint8_t fevent_vib_sensor(uint8_t event);
static uint8_t fevent_vib_on_alarm(uint8_t event);
static uint8_t fevent_vib_off_alarm(uint8_t event);
/*================== Struct ===================*/
sEvent_struct       sEventAppVibSensor[]=
{
  {_EVENT_VIB_SENSOR,           1, 0, 3000,            fevent_vib_sensor},
  {_EVENT_VIB_ON_ALARM,         0, 0, 5,               fevent_vib_on_alarm},
  {_EVENT_VIB_OFF_ALARM,        0, 0, TIME_ON_ALARM,   fevent_vib_off_alarm},
};

StructStatusVib     sStatusVib = {0};
/*================== Function Handle ==============*/
static uint8_t fevent_vib_sensor(uint8_t event)
{
    if(sStatusVib.Sensor1 != 0)
    {
        sStatusVib.LevelWarning++;
        sStatusVib.Sensor1 = 0;
    }
    
    if(sStatusVib.Sensor2 != 0)
    {
        sStatusVib.LevelWarning++;
        sStatusVib.Sensor2 = 0;
    }
    
    if(sStatusVib.Sensor3 != 0)
    {
        sStatusVib.LevelWarning++;
        sStatusVib.Sensor3 = 0;
    }
    
    if(sStatusVib.LevelWarning != 0)
    {
        uint8_t aData[5];
        uint8_t length = 0;
        
        length = Log_Data_Vib(aData);
        Respond_PcBox(aData, length);
        
        AppVibSensor_Debug();
        
        if(sStatusVib.LevelWarning == 3)
        {
            fevent_active(sEventAppVibSensor, _EVENT_VIB_ON_ALARM);
        }
        sStatusVib.LevelWarning = 0;
    }
    
    fevent_enable(sEventAppVibSensor, event);
    return 1;
}

static uint8_t fevent_vib_on_alarm(uint8_t event)
{
    sStatusRelay.Alarm = ON_RELAY;
    fevent_active(sEventAppRelay, _EVENT_ON_OFF_RELAY_ALARM);
    
    fevent_active(sEventAppVibSensor, _EVENT_VIB_OFF_ALARM);
    sEventAppVibSensor[_EVENT_VIB_OFF_ALARM].e_systick = HAL_GetTick();
    
    return 1;
}

static uint8_t fevent_vib_off_alarm(uint8_t event)
{
    sStatusRelay.Alarm = OFF_RELAY;
    fevent_active(sEventAppRelay, _EVENT_ON_OFF_RELAY_ALARM);
    
    return 1;
}
/*============== Function Handle ==============*/
uint8_t Log_Data_Vib(uint8_t *aData)
{
    uint8_t length = 0;
    uint16_t TempCrc = 0;
    
/*=============== Log ===============*/
    
    aData[length++] = OBIS_WARNING_VIB_SENSOR;
    aData[length++] = 0x01;
    aData[length++] = sStatusVib.LevelWarning;
    
    Calculator_Crc_U16(&TempCrc, aData, length);
    
    aData[length++] = TempCrc;
    aData[length++] = TempCrc << 8;

      
    return length;
}

void AppVibSensor_Debug(void)
{
#ifdef USING_APP_VIB_SENSOR_DEBUG
    char cData[1];
    Convert_Int_To_String(cData, sStatusVib.LevelWarning);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_vib_sensor: Level ", sizeof("app_vib_sensor: Level"));
    UTIL_Printf(DBLEVEL_M, (uint8_t*)cData, 1);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n"));
#endif
}

uint8_t AppVibSensor_Task(void)
{
    uint8_t i = 0;
    uint8_t Result =  false;
    
    for(i = 0; i < _EVENT_VIB_SENSOR_END; i++)
    {
        if(sEventAppVibSensor[i].e_status == 1)
        {
            Result = true; 
            
            if((sEventAppVibSensor[i].e_systick == 0) ||
               ((HAL_GetTick() - sEventAppVibSensor[i].e_systick) >= sEventAppVibSensor[i].e_period))
            {
                sEventAppVibSensor[i].e_status = 0; //Disable event
                sEventAppVibSensor[i].e_systick= HAL_GetTick();
                sEventAppVibSensor[i].e_function_handler(i);
            }
        }
    }
    
    
    return Result;
}


