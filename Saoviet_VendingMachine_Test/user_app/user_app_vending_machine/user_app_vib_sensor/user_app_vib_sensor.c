#include "user_app_vib_sensor.h"
#include "user_inc_vending_machine.h"
/*=============== Function static ================*/
static uint8_t fevent_vib_sensor_entry(uint8_t event);
static uint8_t fevent_vib_sensor_scan(uint8_t event);
static uint8_t fevent_vib_sensor(uint8_t event);
static uint8_t fevent_vib_off_alarm(uint8_t event);
static uint8_t fevent_vib_led_warning(uint8_t event);
/*================== Struct ===================*/
sEvent_struct       sEventAppVibSensor[]=
{
  {_EVENT_VIB_SENSOR_ENTRY,     1, 0, TIME_ON_DCU,     fevent_vib_sensor_entry},
  {_EVENT_VIB_SENSOR_SCAN,      1, 0, 200,             fevent_vib_sensor_scan},

  {_EVENT_VIB_SENSOR,           0, 0, 3000,            fevent_vib_sensor},
  {_EVENT_VIB_OFF_ALARM,        0, 0, 0,               fevent_vib_off_alarm},
  
  {_EVENT_VIB_LED_WARNING,      0, 0, 0,               fevent_vib_led_warning},
};

StructStatusVib     sStatusVib = {0};
/*================== Function Handle ==============*/
static uint8_t fevent_vib_sensor_entry(uint8_t event)
{
    fevent_active(sEventAppVibSensor, _EVENT_VIB_SENSOR);
    return 1;
}

static uint8_t fevent_vib_sensor_scan(uint8_t event)
{
  /*
    Quet de xac nhan muc do rung cua sensor 
    va chon muc rung lon nhat
  */
    if(sStatusVib.Sensor1_Scan > sStatusVib.Sensor1)
        sStatusVib.Sensor1 = sStatusVib.Sensor1_Scan;
    
    sStatusVib.Sensor1_Scan = 0;
    
    fevent_enable(sEventAppVibSensor, event);
    return 1;
}

static uint8_t fevent_vib_sensor(uint8_t event)
{
/*---------------Kiem tra tung sensor vib va muc do rung--------------*/
    static uint32_t GetTickLevelAlarm = 0;
    if(sStatusVib.Sensor1 > 0)  
        sStatusVib.LevelWarning = 1;
    else if(sStatusVib.Sensor1 > 40)
        sStatusVib.LevelWarning = 2;
    else if(sStatusVib.Sensor1 > 80)
        sStatusVib.LevelWarning = 3;
    
    sStatusVib.Sensor1 = 0;
    
    if(sStatusVib.LevelWarning != 0)
    {
        Log_Data_Vib_Respond_PcBox();
        
        AppVibSensor_Debug();

        if(sStatusVib.LevelWarning >= 2)
        {
            fevent_active(sEventAppVibSensor, _EVENT_VIB_LED_WARNING);
            
            if(sStatusVib.LevelWarning == 3)
            {
              ControlRelay(RELAY_ALARM, ON_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_UNCTRL);
                
                if((HAL_GetTick() - GetTickLevelAlarm < TIME_LEVEL_ALARM) && (GetTickLevelAlarm != 0))
                    sEventAppVibSensor[_EVENT_VIB_OFF_ALARM].e_period = TIME_ON_ALARM_2;
                else
                    sEventAppVibSensor[_EVENT_VIB_OFF_ALARM].e_period = TIME_ON_ALARM_1;

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
/*------------------Tat relay canh bao--------------*/
    ControlRelay(RELAY_ALARM, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_UNCTRL);
    return 1;
}

static uint8_t fevent_vib_led_warning(uint8_t event)
{
/*-----------Canh bao rung nhap nhay led-----------*/
    static uint8_t Count_Toggle = 0;
    static uint8_t Count_Morse  = 0;
    
    if(sEventAppVibSensor[_EVENT_VIB_LED_WARNING].e_period == TIME_LED_TOGGLE)
    {
        if(Count_Morse < NUMBER_LED_TOGGLE)
        {
            OnOff_Relay(RELAY_LAMP, ON_RELAY);
            switch(Count_Toggle)
            {
                case 4:
                case 5:
                case 6:
                    sEventAppVibSensor[_EVENT_VIB_LED_WARNING].e_period = 200;
                    break;
                    
                default:
                    sEventAppVibSensor[_EVENT_VIB_LED_WARNING].e_period = 30;
                    break;
            }
        }
        else
        {
            Count_Morse = 0;
            Count_Toggle = 0;
            OnOff_Relay(RELAY_LAMP, sStatusRelay.Lamp);
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
                sEventAppVibSensor[_EVENT_VIB_LED_WARNING].e_period = 1000;
        }
    }
    
    fevent_enable(sEventAppVibSensor, event);
    return 1;
}
/*============== Function Handle ==============*/

/*
    @brief  Phan hoi canh bao rung len PcBox
*/
void Log_Data_Vib_Respond_PcBox(void)
{
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_WARNING_VIB_SENSOR;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sStatusVib.LevelWarning;
    
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
}

/*
    @brief  Debug
*/
void AppVibSensor_Debug(void)
{
#ifdef USING_APP_VIB_SENSOR_DEBUG
    uint8_t aData[1];
    Convert_Int_To_String(aData, sStatusVib.LevelWarning);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_vib_sensor: Level ", sizeof("app_vib_sensor: Level ")-1);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, 1);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
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


