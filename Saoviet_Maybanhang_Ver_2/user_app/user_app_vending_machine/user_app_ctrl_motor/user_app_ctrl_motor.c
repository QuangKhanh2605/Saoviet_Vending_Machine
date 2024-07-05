#include "user_app_ctrl_motor.h"
#include "user_inc_vending_machine.h"
/*=============Function Static==============*/
static uint8_t fevent_motor_entry(uint8_t event);
static uint8_t fevent_control_motor_push(uint8_t event);
static uint8_t fevent_input_motor_push(uint8_t event);
static uint8_t fevent_motor_push_off_error(uint8_t event);
static uint8_t fevent_respond_pcbox(uint8_t event);
static uint8_t fevent_wait_free_motor(uint8_t event);
/*================ Struct =================*/
sEvent_struct         sEventAppMotor[] =
{   
  { _EVENT_MOTOR_ENTRY,               0, 0, 5,                          fevent_motor_entry},
  { _EVENT_CONTROL_MOTOR_PUSH,        0, 0, 5,                          fevent_control_motor_push}, 
  { _EVENT_INPUT_MOTOR_PUSH,          0, 0, 5,                          fevent_input_motor_push},
  { _EVENT_MOTOR_PUSH_OFF_ERROR,      0, 0, TIME_MOTOR_PUSH_LATE,       fevent_motor_push_off_error},
  { _EVENT_RESPOND_PCBOX,             0, 0, TIME_MOTOR_RESPOND_PC_BOX,  fevent_respond_pcbox},
  
  { _EVENT_WAIT_FREE_MOTOR,           0, 0, 60000,                      fevent_wait_free_motor},
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
    //Thuc hien reset thong so truoc khi push motor
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
    sPushMotor.IrSensor = 0;
    sPushMotor.NumHandle++;
    
    fevent_enable(sEventAppMotor, _EVENT_MOTOR_PUSH_OFF_ERROR);
    
    fevent_enable(sEventAppMotor, _EVENT_RESPOND_PCBOX);
    
    fevent_active(sEventAppMotor, _EVENT_INPUT_MOTOR_PUSH);
    
    On_Motor_Push(sPushMotor.Pos);
    
    return 1;
}

static uint8_t fevent_input_motor_push(uint8_t event)
{
    if(sPushMotor.PulseCount == 3)  //Ket thuc sau khi nhan duoc 3 xung (Hoan thanh 1 vong motor)
    {
        if(HAL_GetTick() - sEventAppMotor[_EVENT_MOTOR_PUSH_OFF_ERROR].e_systick < TIME_MOTOR_PUSH_EARLY)   
        {
            sInforPush.NumEarly++;  //Loi ket thuc som thoi gian cho truoc
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
    sInforPush.NumLate++;   //Loi ket thuc muon thoi gian cho truoc
    
    return 1;
}

static uint8_t fevent_respond_pcbox(uint8_t event)
{
/*-------------------Respond PcBox------------------*/
    sRespPcBox.Length_u16 = 0;
    if(sPushMotor.State == PUSH_MOTOR)  //Xac nhan trang thai push hoac fix motor
    {
        sInforPush.IrSensor += sPushMotor.IrSensor;

        /*=============== Log ===============*/
        if(sPushMotor.NumHandle < sPushMotor.SumHandle) //Xac nhan dang push hoac da hoan thanh
        {
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_ON_GOING_PUSH;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x04;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sPushMotor.Pos;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sPushMotor.IrSensor;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sPushMotor.NumHandle;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sPushMotor.SumHandle;
            fevent_active(sEventAppMotor, _EVENT_CONTROL_MOTOR_PUSH);
        }
        else
        {
            sPushMotor.StatePush = COMPLETE_PUSH;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_COMPLETE_PUSH;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x04;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sPushMotor.Pos;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sInforPush.IrSensor;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sInforPush.NumEarly;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sInforPush.NumLate;
            AppMotor_Debug();
            fevent_enable(sEventAppMotor, _EVENT_WAIT_FREE_MOTOR);
        }
    }
    else
    {
            sPushMotor.StatePush = COMPLETE_PUSH;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_PC_BOX_FIX_MOTOR;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sPushMotor.Pos;
            fevent_enable(sEventAppMotor, _EVENT_WAIT_FREE_MOTOR);
    }
    
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
    return 1;
}

static uint8_t fevent_wait_free_motor(uint8_t event)
{
    sStatusApp.Motor = _APP_FREE;
    return 1;
}

/*================ Function Handle =================*/
/*
    @brief  Fix motor
    @param  Pos: Vi tri can Fix
*/
void Fix_Motor(uint8_t Pos)
{
    if(Pos > 0 && Pos <= NUMBER_MAX_MOTOR)
    {
        if(sPushMotor.StatePush == COMPLETE_PUSH)
        {
            sStatusApp.Motor      = _APP_BUSY;
            fevent_disable(sEventAppMotor, _EVENT_WAIT_FREE_MOTOR);
              
            sPushMotor.State      = FIX_MOTOR;
            sPushMotor.StatePush  = ON_GOING_PUSH;
            sPushMotor.Pos        = Pos;
            sPushMotor.SumHandle  = 1;
            sPushMotor.PulseCount = 1;
            fevent_active(sEventAppMotor, _EVENT_MOTOR_ENTRY);
        } 
    }
}

/*
    @brief  Push motor
    @param  Pos: Vi tri Push
    @param  Number: So luong Push
*/
void Push_Motor(uint8_t Pos, uint8_t Number)
{
    if(Pos > 0 && Pos <= NUMBER_MAX_MOTOR)
    {
        if(Number > 0 && Number <= NUMBER_MAX_PUSH)
        {
            if(sPushMotor.StatePush == COMPLETE_PUSH)
            {
                sStatusApp.Motor      = _APP_BUSY;
                fevent_disable(sEventAppMotor, _EVENT_WAIT_FREE_MOTOR);
                
                sPushMotor.State      = PUSH_MOTOR;
                sPushMotor.StatePush  = ON_GOING_PUSH;
                sPushMotor.Pos        = Pos;
                sPushMotor.SumHandle  = Number;
                sPushMotor.PulseCount = 0;
                fevent_active(sEventAppMotor, _EVENT_MOTOR_ENTRY);
            } 
        }
    }
}

/*
	@brief  ON motor tai vi tri Pos
	@param  Pos vi tri motor muon chay
*/
void On_Motor_Push(uint8_t Pos)
{
    uint8_t Pos_Push = 0;
    Pos_Push = Pos - 1;
    HAL_GPIO_WritePin(LAYER_PORT[Pos_Push/10], LAYER_PIN[Pos_Push/10], GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(SLOT_PORT[Pos_Push%10], SLOT_PIN[Pos_Push%10], GPIO_PIN_SET);
}

/*
    @brief  OFF toan bo motor
*/
void Off_Motor_Push(void)
{
    uint8_t i = 0;
    for(i = 0; i < 7 ; i++)
        HAL_GPIO_WritePin(LAYER_PORT[i], LAYER_PIN[i], GPIO_PIN_RESET); 

    for(i = 0; i < 10 ; i++)
        HAL_GPIO_WritePin(SLOT_PORT[i], SLOT_PIN[i], GPIO_PIN_RESET);

}


/*
    @brief  Debug trang thai push
*/
void AppMotor_Debug(void)
{
#ifdef USING_APP_CTRL_MOTOR_DEBUG
    uint8_t aData[2];
    Convert_Int_To_String(aData, sPushMotor.Pos);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_ctrl_motor: Pos:", sizeof("app_ctrl_motor: Pos:")-1);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, 2);
    
    aData[1]=0x00;
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" IR:", sizeof(" IR:")-1);
    Convert_Int_To_String(aData, sInforPush.IrSensor);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, 2);
    
    aData[1]=0x00;
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" Early:", sizeof(" Early:")-1);
    Convert_Int_To_String(aData, sInforPush.NumEarly);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, 2);
    
    aData[1]=0x00;
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" Late:", sizeof(" Late:")-1);
    Convert_Int_To_String(aData, sInforPush.NumLate);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, 2);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
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

