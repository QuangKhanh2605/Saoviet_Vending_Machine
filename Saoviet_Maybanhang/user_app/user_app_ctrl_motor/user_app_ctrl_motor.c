#include "user_app_ctrl_motor.h"

/*=============Function Static==============*/
static uint8_t fevent_control_motor_push(uint8_t event);
static uint8_t fevent_input_motor_push(uint8_t event);
/*================ Struct =================*/
sEvent_struct         sEventAppMotor[] =
{   
  { _EVENT_CONTROL_MOTOR_PUSH,        1, 0, 5,       fevent_control_motor_push}, 
  { _EVENT_INPTU_MOTOR_PUSH,          1, 0, 5,       fevent_input_motor_push},
};  


static GPIO_TypeDef*  LAYER_PORT[7] = {Layer_1_GPIO_Port, Layer_2_GPIO_Port, Layer_3_GPIO_Port,
                                       Layer_4_GPIO_Port, Layer_5_GPIO_Port, Layer_6_GPIO_Port,
                                       Layer_7_GPIO_Port};
static const uint16_t LAYER_PIN[7] = {Layer_1_Pin, Layer_2_Pin, Layer_3_Pin, Layer_4_Pin, 
                                      Layer_5_Pin, Layer_6_Pin, Layer_7_Pin};

static GPIO_TypeDef*  SLOT_PORT[10] = {Slot_1_GPIO_Port, Slot_2_GPIO_Port, Slot_3_GPIO_Port,
                                       Slot_4_GPIO_Port, Slot_5_GPIO_Port, Slot_6_GPIO_Port,
                                       Slot_7_GPIO_Port, Slot_8_GPIO_Port, Slot_9_GPIO_Port,
                                       Slot_10_GPIO_Port};
static const uint16_t SLOT_PIN[10] = {Slot_1_Pin, Slot_2_Pin, Slot_3_Pin, Slot_4_Pin, 
                                      Slot_5_Pin, Slot_6_Pin, Slot_7_Pin, Slot_8_Pin,
                                      Slot_9_Pin, Slot_10_Pin};


struct_ControlMotor       sControlMotor=
{
    .Status = 0,
    .Layer  = 0,
    .Slot   = 0,
};
/*================ Function Handler =================*/

static uint8_t fevent_control_motor_push(uint8_t event)
{
    if(sControlMotor.Status == 1)
    {
        sControlMotor.Status = 0;
        HAL_GPIO_WritePin(LAYER_PORT[sControlMotor.Layer -1], LAYER_PIN[sControlMotor.Layer -1], GPIO_PIN_SET);
        HAL_Delay(10);
        HAL_GPIO_WritePin(SLOT_PORT[sControlMotor.Slot -1], SLOT_PIN[sControlMotor.Slot -1], GPIO_PIN_SET);
    }
    fevent_enable(sEventAppMotor, event);
    return 1;
}

static uint8_t fevent_input_motor_push(uint8_t event)
{
    static uint8_t count = 0;
    static uint8_t status_before = 0;
    static uint8_t status_current= 0;
    
//    static uint8_t temp_status   = 0;
//    static uint8_t count_handle=0;
//    status_current = HAL_GPIO_ReadPin(Count_GPIO_Port, Count_Pin);
//
//    if(status_current == status_before)
//    {
//        count_handle++;
//        if(count_handle >= 5)
//        {
//            count_handle = 5;
//            if(status_current != temp_status)
//            {
//                count++;
//            }
//            temp_status = status_current;
//        }
//    }
//    else
//    {
//        count_handle = 0;
//    }

    
    if(status_current != status_before)
    {
        count++;
    }
    
    if(count == 2)
    {
        count = 0;
        Off_Motor_Push();
    }
    
    status_before = HAL_GPIO_ReadPin(Count_GPIO_Port, Count_Pin);
    fevent_enable(sEventAppMotor, event);
    return 1;
}


/*================ Function Handle =================*/
void Off_Motor_Push(void)
{
    uint8_t i = 0;
    for(i = 0; i < 10 ; i++)
    {
        HAL_GPIO_WritePin(LAYER_PORT[i], LAYER_PIN[i], GPIO_PIN_RESET); 
    }
    for(i = 0; i < 7 ; i++)
    {
        HAL_GPIO_WritePin(SLOT_PORT[i], SLOT_PIN[i], GPIO_PIN_RESET);
    }
}

uint8_t AppMotor_Task(void)
{
	uint8_t i = 0;
	uint8_t Result = false;

	for (i = 0; i < _EVENT_MOTOR_END; i++)
	{
		if (sEventAppMotor[i].e_status == 1)
		{
            Result = true;

			if ((sEventAppMotor[i].e_systick == 0) ||
					((HAL_GetTick() - sEventAppMotor[i].e_systick)  >=  sEventAppMotor[i].e_period))
			{
                sEventAppMotor[i].e_status = 0;  //Disable event
				sEventAppMotor[i].e_systick = HAL_GetTick();
				sEventAppMotor[i].e_function_handler(i);
			}
		}
	}
    
	return Result;
}