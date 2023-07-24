#include "user_app_ir_sensor.h"

/*============== Function Static ==============*/
static uint8_t fevent_ir_sensor_output(uint8_t event);
static uint8_t fevent_ir_sensor_input(uint8_t event);

/*================= Struct ===================*/
sEvent_struct               sEventAppIrSensor[]=
{
  {_EVENT_IR_SENSOR_OUTPUT,         0, 0, 5,    fevent_ir_sensor_output},
  {_EVENT_IR_SENSOR_INPUT,          0, 0, 5,    fevent_ir_sensor_input},
};


uint8_t Response_Ir_Sensor = 0;
/*================= Function Handle ==============*/
static uint8_t fevent_ir_sensor_output(uint8_t event)
{
    return 1;
}

static uint8_t fevent_ir_sensor_input(uint8_t event)
{
    static uint8_t count_handle  = 0;
    static uint8_t status_current= 0;
    static uint8_t status_before = INIT_STATUS_IR_SENSOR_INPUT;
    static uint8_t temp_status   = INIT_STATUS_IR_SENSOR_INPUT;
    
    status_current = HAL_GPIO_ReadPin(IR_Sensor_IN_GPIO_Port, IR_Sensor_IN_Pin);
    if(status_current == status_before)
    {
        count_handle++;
        if(count_handle >= NUMBER_SPLG_IR_SENSOR_INPUT)
        {
            count_handle = NUMBER_SPLG_IR_SENSOR_INPUT;
            if(status_current != temp_status)
            {
                if(status_current == 0x01 ^ INIT_STATUS_IR_SENSOR_INPUT)
                {
                    Response_Ir_Sensor = 1;
                }
                else
                {
                    Response_Ir_Sensor = 0;
                }
            }
            temp_status = status_current;
        }
    }
    else
    {
        count_handle = 0;
    }
    status_before = HAL_GPIO_ReadPin(IR_Sensor_IN_GPIO_Port, IR_Sensor_IN_Pin);
    
    fevent_enable(sEventAppIrSensor, event);
    return 1;
}

uint8_t AppIrSensor_Task(void)
{
    uint8_t i = 0;
    uint8_t Result = false;
    for(i = 0; i < _EVENT_IR_SENSOR_END; i++)
    {
        if(sEventAppIrSensor[i].e_status == 1)
        {
            Result = true;
            
            if((sEventAppIrSensor[i].e_systick == 0) ||
               ((HAL_GetTick() - sEventAppIrSensor[i].e_systick) >= sEventAppIrSensor[i].e_period))
            {
                sEventAppIrSensor[i].e_status = 0;  //Disable event
                sEventAppIrSensor[i].e_systick = HAL_GetTick();
                sEventAppIrSensor[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}

