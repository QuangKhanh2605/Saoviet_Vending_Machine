
#include "user_app_vib_sensor.h"
#include "user_comm_vending_machine.h"
#include "user_app_relay.h"
#include "user_app_pc_box.h"
/*=============== Function static ================*/
static uint8_t fevent_vib_sensor(uint8_t event);
static uint8_t fevent_vib_off_alarm(uint8_t event);
static uint8_t fevent_vib_led_warning(uint8_t event);
/*================== Struct ===================*/
sEvent_struct       sEventAppVibSensor[]=
{
  {_EVENT_VIB_SENSOR,           1, 0, 3000,            fevent_vib_sensor},
  {_EVENT_VIB_OFF_ALARM,        0, 0, 0,               fevent_vib_off_alarm},
  
  {_EVENT_VIB_LED_WARNING,      0, 0, 0,               fevent_vib_led_warning},
};

StructStatusVib     sStatusVib = {0};

/*================== Function Handle ==============*/
static uint8_t fevent_vib_sensor(uint8_t event)
{
    static uint32_t GetTickLevelAlarm = 0;
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
        Write_Queue_Repond_PcBox(aData, length);
        
        AppVibSensor_Debug();

        if(sStatusVib.LevelWarning >= 2)
        {
            fevent_active(sEventAppVibSensor, _EVENT_VIB_LED_WARNING);
            
            if(sStatusVib.LevelWarning == 3)
            {
//                sStatusRelay.Alarm = ON_RELAY;
//                fevent_active(sEventAppRelay, _EVENT_ON_OFF_RELAY_ALARM);
              ControlRelay(RELAY_ALARM, ON_RELAY, _RL_UNRESPOND, _RL_DEBUG);
                
                if((HAL_GetTick() - GetTickLevelAlarm < TIME_LEVEL_ALARM) && (GetTickLevelAlarm != 0))
                {
                    sEventAppVibSensor[_EVENT_VIB_OFF_ALARM].e_period = TIME_ON_ALARM_2;
                }
                else
                {
                    sEventAppVibSensor[_EVENT_VIB_OFF_ALARM].e_period = TIME_ON_ALARM_1;
                }
                fevent_enable(sEventAppVibSensor, _EVENT_VIB_OFF_ALARM);
                
                GetTickLevelAlarm = HAL_GetTick();
            }
        }
        
        sStatusVib.LevelWarning = 0;
    }
    
    fevent_enable(sEventAppVibSensor, event);
    return 1;
}

static uint8_t fevent_vib_off_alarm(uint8_t event)
{
//    sStatusRelay.Alarm = OFF_RELAY;
//    fevent_active(sEventAppRelay, _EVENT_ON_OFF_RELAY_ALARM);
    ControlRelay(RELAY_ALARM, OFF_RELAY, _RL_UNRESPOND, _RL_DEBUG);
    
    return 1;
}

static uint8_t fevent_vib_led_warning(uint8_t event)
{
    static uint8_t Count_Toggle = 0;
    static uint8_t Count_Morse  = 0;
    
    if(sEventAppVibSensor[_EVENT_VIB_LED_WARNING].e_period == TIME_LED_TOGGLE)
    {
        if(Count_Morse < NUMBER_LED_TOGGLE)
        {
            switch(Count_Toggle)
            {
                case 4:
                case 5:
                case 6:
                    OnOff_Relay(RELAY_LAMP, ON_RELAY);
                    sEventAppVibSensor[_EVENT_VIB_LED_WARNING].e_period = 250;
                    break;
                    
                default:
                    OnOff_Relay(RELAY_LAMP, ON_RELAY);
                    sEventAppVibSensor[_EVENT_VIB_LED_WARNING].e_period = 40;
                    break;
            }
        }
        else
        {
            Count_Morse = 0;
            Count_Toggle = 0;
            if(sStatusRelay.Lamp == 1)
            {
                OnOff_Relay(RELAY_LAMP, ON_RELAY);
            }
            else
            {
                OnOff_Relay(RELAY_LAMP, OFF_RELAY);
            }
            sEventAppVibSensor[_EVENT_VIB_LED_WARNING].e_period = 0;  
            return 1;
        }
    }
    else
    {
        OnOff_Relay(RELAY_LAMP, OFF_RELAY);
        sEventAppVibSensor[_EVENT_VIB_LED_WARNING].e_period = TIME_LED_TOGGLE;  
        Count_Toggle++;
        if(Count_Toggle >= 10)
        {
            Count_Toggle = 0;
            Count_Morse++;
            if(Count_Morse < NUMBER_LED_TOGGLE)
            {
                sEventAppVibSensor[_EVENT_VIB_LED_WARNING].e_period = 1000;
            }
        }
    }
    
    fevent_enable(sEventAppVibSensor, event);
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
    aData[length++] = TempCrc >> 8;

      
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


