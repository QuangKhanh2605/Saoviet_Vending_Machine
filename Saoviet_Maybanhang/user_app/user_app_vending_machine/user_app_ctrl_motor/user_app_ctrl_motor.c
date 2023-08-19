

#include "user_app_ctrl_motor.h"
#include "user_comm_vending_machine.h"
#include "user_app_pc_box.h"
/*=============Function Static==============*/
static uint8_t fevent_motor_entry(uint8_t event);
static uint8_t fevent_control_motor_push(uint8_t event);
static uint8_t fevent_input_motor_push(uint8_t event);
static uint8_t fevent_motor_push_off_error(uint8_t event);
static uint8_t fevent_respond_pcbox(uint8_t event);
/*================ Struct =================*/
sEvent_struct         sEventAppMotor[] =
{   
  { _EVENT_MOTOR_ENTRY,               0, 0, 5,                          fevent_motor_entry},
  { _EVENT_CONTROL_MOTOR_PUSH,        0, 0, 5,                          fevent_control_motor_push}, 
  { _EVENT_INPUT_MOTOR_PUSH,          0, 0, 5,                          fevent_input_motor_push},
  { _EVENT_MOTOR_PUSH_OFF_ERROR,      0, 0, TIME_MOTOR_PUSH_LATE,       fevent_motor_push_off_error},
  { _EVENT_RESPOND_PCBOX,             0, 0, TIME_MOTOR_RESPOND_PC_BOX,  fevent_respond_pcbox},
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

struct_ControlMotor         sPushMotor = {0};
struct_InforMotor           sInforPush = {0};
/*================ Function Handler =================*/
static uint8_t fevent_motor_entry(uint8_t event)
{   
    sInforPush.NumEarly = 0;
    sInforPush.NumLate  = 0;
    sInforPush.IrSensor = 0;
    
    sPushMotor.NumHandle = 0;
    fevent_active(sEventAppMotor, _EVENT_CONTROL_MOTOR_PUSH);
    sPushMotor.StatePush = ON_GOING_PUSH;
    return 1;
}

static uint8_t fevent_control_motor_push(uint8_t event)
{
    if(sPushMotor.Pos <= NUMBER_MAX_MOTOR)
    {
        sPushMotor.IrSensor = 0;
        sPushMotor.NumHandle++;
        
        fevent_enable(sEventAppMotor, _EVENT_MOTOR_PUSH_OFF_ERROR);
        
        fevent_enable(sEventAppMotor, _EVENT_RESPOND_PCBOX);
        
        fevent_active(sEventAppMotor, _EVENT_INPUT_MOTOR_PUSH);
        
        On_Motor_Push(sPushMotor.Pos);
    }
    
    return 1;
}

static uint8_t fevent_input_motor_push(uint8_t event)
{
    if(sPushMotor.PulseCount == 3)
    {
        if(HAL_GetTick() - sEventAppMotor[_EVENT_MOTOR_PUSH_OFF_ERROR].e_systick < TIME_MOTOR_PUSH_EARLY)
        {
            sInforPush.NumEarly++;
            sPushMotor.PulseCount = 2;
        }
        else
        {
            Off_Motor_Push();
            sPushMotor.PulseCount = 0;
            fevent_disable(sEventAppMotor, _EVENT_MOTOR_PUSH_OFF_ERROR);
            return 1;
        }
    }
    
    fevent_enable(sEventAppMotor, event);
    return 1;
}

static uint8_t fevent_motor_push_off_error(uint8_t event)
{
    Off_Motor_Push();
    sPushMotor.PulseCount = 0;
    sInforPush.NumLate++;
    
    return 1;
}

static uint8_t fevent_respond_pcbox(uint8_t event)
{
    if(sPushMotor.State == PUSH_MOTOR)
    {
        sInforPush.IrSensor += sPushMotor.IrSensor;
        uint8_t aData[10];
        uint8_t length = 0;
        uint16_t TempCrc = 0;
        
    /*=============== Log ===============*/
        if(sPushMotor.NumHandle < sPushMotor.Num)
        {
            aData[length++] = OBIS_ON_GOING_PUSH;
            aData[length++] = 0x04;
            aData[length++] = sPushMotor.Pos;
            aData[length++] = sPushMotor.IrSensor;
            aData[length++] = sPushMotor.NumHandle;
            aData[length++] = sPushMotor.Num;
            fevent_active(sEventAppMotor, _EVENT_CONTROL_MOTOR_PUSH);
        }
        else
        {
            sPushMotor.StatePush = COMPLETE_PUSH;
            aData[length++] = OBIS_COMPLETE_PUSH;
            aData[length++] = 0x04;
            aData[length++] = sPushMotor.Pos;
            aData[length++] = sInforPush.IrSensor;
            aData[length++] = sInforPush.NumEarly;
            aData[length++] = sInforPush.NumLate;
            AppMotor_Debug();
        }
        
        Calculator_Crc_U16(&TempCrc, aData, length);
        
        aData[length++] = TempCrc;
        aData[length++] = TempCrc >> 8;
        Write_Queue_Repond_PcBox(aData, length);
    }
    return 1;
}

/*================ Function Handle =================*/
void On_Motor_Push(uint8_t Pos)
{
    uint8_t Pos_Push = 0;
    Pos_Push = Pos - 1;
    HAL_GPIO_WritePin(LAYER_PORT[Pos_Push/10], LAYER_PIN[Pos_Push/10], GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(SLOT_PORT[Pos_Push%10], SLOT_PIN[Pos_Push%10], GPIO_PIN_SET);
}

void Off_Motor_Push(void)
{
    uint8_t i = 0;
    for(i = 0; i < 7 ; i++)
    {
        HAL_GPIO_WritePin(LAYER_PORT[i], LAYER_PIN[i], GPIO_PIN_RESET); 
    }
    for(i = 0; i < 10 ; i++)
    {
        HAL_GPIO_WritePin(SLOT_PORT[i], SLOT_PIN[i], GPIO_PIN_RESET);
    }
}

void AppMotor_Debug(void)
{
#ifdef USING_APP_CTRL_MOTOR_DEBUG
    char cData[2];
    Convert_Int_To_String(cData, sPushMotor.Pos);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_ctrl_motor: Pos:", sizeof("app_ctrl_motor: Pos:"));
    UTIL_Printf(DBLEVEL_M, (uint8_t*)cData, 2);
    
    cData[1]=0x00;
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" IR:", sizeof(" IR:"));
    Convert_Int_To_String(cData, sInforPush.IrSensor);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)cData, 2);
    
    cData[1]=0x00;
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" Early:", sizeof(" Early:"));
    Convert_Int_To_String(cData, sInforPush.NumEarly);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)cData, 2);
    
    cData[1]=0x00;
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" Late:", sizeof(" Late:"));
    Convert_Int_To_String(cData, sInforPush.NumLate);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)cData, 2);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n"));
#endif
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
				sEventAppMotor[i].e_systick= HAL_GetTick();
				sEventAppMotor[i].e_function_handler(i);
			}
		}
	}
    
	return Result;
}

