#include "user_app_vib_sensor.h"

/*=============== Function static ================*/
static uint8_t fevent_vib_sensor_1(uint8_t event);
static uint8_t fevent_vib_sensor_2(uint8_t event);
static uint8_t fevent_vib_sensor_3(uint8_t event);
/*================== Struct ===================*/
sEvent_struct       sEventAppVibSensor[]=
{
  {_EVENT_VIB_SENSOR_1,     0, 0, 5,    fevent_vib_sensor_1},
  {_EVENT_VIB_SENSOR_2,     0, 0, 5,    fevent_vib_sensor_2},
  {_EVENT_VIB_SENSOR_3,     0, 0, 5,    fevent_vib_sensor_3},
};


StructStatusVib     sStatusVib = {0};
/*================== Function Handle ==============*/
static uint8_t fevent_vib_sensor_1(uint8_t event)
{
    static uint8_t count_handle  = 0;
    static uint8_t status_current= 0;
    static uint8_t status_before = INIT_STATUS_VIB_SENSOR_INPUT;
    static uint8_t temp_status   = INIT_STATUS_VIB_SENSOR_INPUT;
    
    status_current = HAL_GPIO_ReadPin(Vib_Sensor_IN_1_GPIO_Port, Vib_Sensor_IN_1_Pin);
    if(status_current == status_before)
    {
        count_handle++;
        if(count_handle >= NUMBER_SPLG_VIB_SENSOR_INPUT)
        {
            count_handle = NUMBER_SPLG_VIB_SENSOR_INPUT;
            if(status_current != temp_status)
            {
                if(status_current == 0x01 ^ INIT_STATUS_VIB_SENSOR_INPUT)
                {
                    sStatusVib.Sensor1 = 1;
                }
                else
                {
                    sStatusVib.Sensor1 = 0;
                }
            }
            temp_status = status_current;
        }
    }
    else
    {
        count_handle = 0;
    }
    status_before = HAL_GPIO_ReadPin(Vib_Sensor_IN_1_GPIO_Port, Vib_Sensor_IN_1_Pin);
    
    fevent_enable(sEventAppVibSensor, event);
    return 1;
}

static uint8_t fevent_vib_sensor_2(uint8_t event)
{
    static uint8_t count_handle  = 0;
    static uint8_t status_current= 0;
    static uint8_t status_before = INIT_STATUS_VIB_SENSOR_INPUT;
    static uint8_t temp_status   = INIT_STATUS_VIB_SENSOR_INPUT;
    
    status_current = HAL_GPIO_ReadPin(Vib_Sensor_IN_2_GPIO_Port, Vib_Sensor_IN_2_Pin);
    if(status_current == status_before)
    {
        count_handle++;
        if(count_handle >= NUMBER_SPLG_VIB_SENSOR_INPUT)
        {
            count_handle = NUMBER_SPLG_VIB_SENSOR_INPUT;
            if(status_current != temp_status)
            {
                if(status_current == 0x01 ^ INIT_STATUS_VIB_SENSOR_INPUT)
                {
                    sStatusVib.Sensor2 = 1;
                }
                else
                {
                    sStatusVib.Sensor2 = 0;
                }
            }
            temp_status = status_current;
        }
    }
    else
    {
        count_handle = 0;
    }
    status_before = HAL_GPIO_ReadPin(Vib_Sensor_IN_2_GPIO_Port, Vib_Sensor_IN_2_Pin);
    
    fevent_enable(sEventAppVibSensor, event);
    return 1;
}

static uint8_t fevent_vib_sensor_3(uint8_t event)
{
    static uint8_t count_handle  = 0;
    static uint8_t status_current= 0;
    static uint8_t status_before = INIT_STATUS_VIB_SENSOR_INPUT;
    static uint8_t temp_status   = INIT_STATUS_VIB_SENSOR_INPUT;
    
    status_current = HAL_GPIO_ReadPin(Vib_Sensor_IN_3_GPIO_Port, Vib_Sensor_IN_3_Pin);
    if(status_current == status_before)
    {
        count_handle++;
        if(count_handle >= NUMBER_SPLG_VIB_SENSOR_INPUT)
        {
            count_handle = NUMBER_SPLG_VIB_SENSOR_INPUT;
            if(status_current != temp_status)
            {
                if(status_current == 0x01 ^ INIT_STATUS_VIB_SENSOR_INPUT)
                {
                    sStatusVib.Sensor3 = 1;
                }
                else
                {
                    sStatusVib.Sensor3 = 0;
                }
            }
            temp_status = status_current;
        }
    }
    else
    {
        count_handle = 0;
    }
    status_before = HAL_GPIO_ReadPin(Vib_Sensor_IN_3_GPIO_Port, Vib_Sensor_IN_3_Pin);
    
    fevent_enable(sEventAppVibSensor, event);
    return 1;
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


