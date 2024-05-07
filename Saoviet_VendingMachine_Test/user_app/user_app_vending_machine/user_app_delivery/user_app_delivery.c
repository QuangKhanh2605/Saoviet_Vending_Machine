#include "user_app_delivery.h"
#include "user_inc_vending_machine.h"
#include "tim.h"
#include "user_modem.h"
/*=============Function Static==============*/
static uint8_t fevent_motor_entry(uint8_t event);
static uint8_t fevent_wait_free_motor(uint8_t event);

static uint8_t fevent_lock_magnetis_begin(uint8_t event);
static uint8_t fevent_elevator_lower_begin(uint8_t event);
static uint8_t fevent_delivery_entry(uint8_t event);
static uint8_t fevent_calculator_coordinates(uint8_t event);
static uint8_t fevent_push_motor(uint8_t event);
static uint8_t fevent_elevator_lower_end(uint8_t event);
static uint8_t fevent_lock_magnetis_end(uint8_t event);

static uint8_t fevent_fix_motor(uint8_t event);
static uint8_t fevent_ctrl_elevator_upper(uint8_t event);
static uint8_t fevent_ctrl_elevator_lower(uint8_t event);
static uint8_t fevent_handle_error_delivery(uint8_t event);
static uint8_t fevent_test_elevator(uint8_t event);
static uint8_t fevent_test_lock_magnetis(uint8_t event);

static uint8_t fevent_test_ele(uint8_t event);
static uint8_t fevent_test_motor(uint8_t event);
/*================ Struct =================*/
sEvent_struct         sEventAppDelivery[] =
{   
  { _EVENT_MOTOR_ENTRY,               1, 0, 0,                          fevent_motor_entry},
  { _EVENT_WAIT_FREE_MOTOR,           0, 0, 60000,                      fevent_wait_free_motor},
  
  { _EVENT_LOCK_MAGNETIS_BEGIN,       0, 0, 0,                          fevent_lock_magnetis_begin},
  { _EVENT_ELEVATOR_LOWER_BEGIN,      0, 0, 0,                          fevent_elevator_lower_begin},
  { _EVENT_DELIVERY_ENTRY,            0, 0, 0,                          fevent_delivery_entry},
  { _EVENT_CALCULATOR_COORDINATES,    0, 0, 0,                          fevent_calculator_coordinates},
  { _EVENT_PUSH_MOTOR,                0, 0, 0,                          fevent_push_motor},
  { _EVENT_ELEVATOR_LOWER_END,        0, 0, 0,                          fevent_elevator_lower_end},
  { _EVENT_LOCK_MAGNETIS_END,         0, 0, 0,                          fevent_lock_magnetis_end},
  
  { _EVENT_FIX_MOTOR,                 0, 0, 0,                          fevent_fix_motor},
  { _EVENT_CTRL_ELEVATOR_UPPER,       0, 5, 500,                        fevent_ctrl_elevator_upper},
  { _EVENT_CTRL_ELEVATOR_LOWER,       0, 5, 500,                        fevent_ctrl_elevator_lower},
  
  { _EVENT_HANDLE_ERROR_DELIVERY,     0, 5, 20000,                      fevent_handle_error_delivery},
  
  { _EVENT_TEST_ELEVATOR,             0, 5, 0,                          fevent_test_elevator},
  { _EVENT_TEST_LOCK_MAGNETIS,        0, 5, 2000,                       fevent_test_lock_magnetis},
  
  { _EVENT_TEST_ELE,                  1, 0, 0,                          fevent_test_ele},
  { _EVENT_TEST_MOTOR,                1, 0, 0,                          fevent_test_motor},
};

static GPIO_TypeDef*  CTRL_MOTOR_PORT[NUMBER_MOTOR] = {CTRL_MOTOR_1_PORT, CTRL_MOTOR_2_PORT, CTRL_MOTOR_3_PORT,
                                                       CTRL_MOTOR_4_PORT, CTRL_MOTOR_5_PORT, CTRL_MOTOR_6_PORT,
                                                       CTRL_MOTOR_7_PORT, CTRL_MOTOR_8_PORT, CTRL_MOTOR_9_PORT,
                                                       CTRL_MOTOR_10_PORT};
static const uint16_t CTRL_MOTOR_PIN[NUMBER_MOTOR] = {CTRL_MOTOR_1_PIN, CTRL_MOTOR_2_PIN, CTRL_MOTOR_3_PIN, 
                                                      CTRL_MOTOR_4_PIN, CTRL_MOTOR_5_PIN, CTRL_MOTOR_6_PIN, 
                                                      CTRL_MOTOR_7_PIN, CTRL_MOTOR_8_PIN, CTRL_MOTOR_9_PIN,
                                                      CTRL_MOTOR_10_PIN};

struct_InforMotor           sInforPush = {0};
Struct_ParamDelivery        sParamDelivery = {0};
Struct_Elevator             sElevator = {0};
uint8_t                     DetectErrorDelivery = 0;

uint8_t                     StateDebugDelivery = 0;
/*================ Function Handler =================*/
static uint8_t fevent_motor_entry(uint8_t event)
{   
    HAL_GPIO_WritePin(PW_LOCK_PORT, PW_LOCK_PIN, GPIO_PIN_SET); //Cap nguon cho Lock Magnetis
    return 1;
}

/*
    Xac nhan trang thai ban hang ranh roi (Phuc vu cho cac nhiem vu khac)
*/
static uint8_t fevent_wait_free_motor(uint8_t event)
{
    sStatusApp.Delivery = _APP_FREE;
    return 1;
}

/*==================== Function Handle Delivery ===================*/
/*
    Kiem tra khoa tu da dong chua de thuc hien lenh tiep theo
*/
static uint8_t fevent_lock_magnetis_begin(uint8_t event)
{
    StateDebugDelivery = 1;
      
    //Kiem tra Door tra hang. Neu Door close thi lock LockMagnetis
    if(sStatusDoor.SensorDelivery == DOOR_CLOSE)
        Control_LockMagnetis(GPIO_PIN_SET);     // Lock LockMagnetis
    
    //Check Feedback LockMagnetis.
    if(sParamDelivery.LockMagnetis == LOCK_MAGNETIS_CLOSE)
    {
        fevent_active(sEventAppDelivery, _EVENT_ELEVATOR_LOWER_BEGIN);  // Chuyen sang event xu ly tiep theo
        
        Enable_Handle_Idle_Delivery(_EVENT_ELEVATOR_LOWER_BEGIN);       // Enable handle error
    }
    else
    {
        fevent_enable(sEventAppDelivery, event);
    }
    return 1;
}

/*
    Kiem tra Elevator tang duoi cung (Floor = 0)
*/
static uint8_t fevent_elevator_lower_begin(uint8_t event)
{
    StateDebugDelivery = 2;
    
    if(HAL_GPIO_ReadPin(FB_Bot_Elevator_GPIO_Port, FB_Bot_Elevator_Pin) == GPIO_PIN_SET)
    {
        if(sElevator.State == ELEVATOR_STOP)
        {
            Control_Elevator(ELEVATOR_DOWN, LEVEL_PWM_ELEVATOR);    //Elevator di chuyen xuong duoi
        }
        fevent_enable(sEventAppDelivery, event);
    }
    else
    {
        OFF_Elevator();         //Stop Elevator
        sElevator.Floor = 0;    //Elevator tang duoi cung
        fevent_active(sEventAppDelivery, _EVENT_DELIVERY_ENTRY);    //Chuyen sang event xu ly tiep theo

        Enable_Handle_Idle_Delivery(_EVENT_DELIVERY_ENTRY);         //Enable handle error
    }
    return 1;
}

/*
    Bat dau vao che do tra hang 
*/
static uint8_t fevent_delivery_entry(uint8_t event)
{
    StateDebugDelivery = 3;
  
    if(sParamDelivery.SumHandle > 0)    // Kiem tra tong so vi tri can tra hang
    {
        sParamDelivery.PosHandle = NUMBER_MOTOR;
        while(sParamDelivery.PosHandle >0)
        {
            sParamDelivery.PosHandle--;
            if(sParamDelivery.aDataPush[sParamDelivery.PosHandle] > 0)  //Neu gap aDataPush > 0 (co hang can tra)
            {
                sParamDelivery.aPulse[sParamDelivery.PosHandle] = 0;    //Cai xung da tra hang = 0
                sParamDelivery.SumHandle--;     //Xac nhan so luong vi tri can tra hang con lai
                sParamDelivery.PosHandle++;     //Xac nhan vi tri tra hang hien tai
                switch(sParamDelivery.PosHandle)    //Xac nhan tang tra hang
                {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                      sParamDelivery.FloorHandle = 4;
                      break;
                      
                    case 5:
                    case 6: 
                      sParamDelivery.FloorHandle = 3;
                      break;
                      
                    case 7:
                    case 8:
                      sParamDelivery.FloorHandle = 2;
                      break;
                      
                    case 9:
                    case 10:
                      sParamDelivery.FloorHandle = 1;
                      break;
                      
                    default:
                      break;
                }
                fevent_active(sEventAppDelivery, _EVENT_CALCULATOR_COORDINATES);    //Chuyen sang event xu ly tiep theo
                    
                Enable_Handle_Idle_Delivery(_EVENT_CALCULATOR_COORDINATES);         //Enable handle error 
                break;
            }
        }
    }
    return 1;
}

/*
    Elevator den vi tri can tra hang
*/
static uint8_t fevent_calculator_coordinates(uint8_t event)
{
    StateDebugDelivery = 4;
  
    if(sParamDelivery.FloorHandle != sElevator.Floor) //Kiem tra Elevator da den tang can tra hang
    {
        if(sElevator.State == ELEVATOR_STOP)
            Control_Elevator(ELEVATOR_UP, LEVEL_PWM_ELEVATOR); //Elevator chay len tren 
        
        fevent_active(sEventAppDelivery, event);
    }
    else
    {
        OFF_Elevator(); //Stop elevator
        fevent_active(sEventAppDelivery,_EVENT_PUSH_MOTOR); //Chuyen sang event su ly tiep theo 
        
        Enable_Handle_Idle_Delivery(_EVENT_PUSH_MOTOR); //Enable handle error
    }
    return 1;
}

/*
    Push Motor tra hang
*/
static uint8_t fevent_push_motor(uint8_t event)
{
    StateDebugDelivery = 5;
  
    //Kiem tra so xung tra hang da bang so luong hang can tra tai vi tri PosHandle
    if(sParamDelivery.aPulse[sParamDelivery.PosHandle - 1] != sParamDelivery.aDataPush[sParamDelivery.PosHandle - 1])
    {
        On_Motor_Push(sParamDelivery.PosHandle);    //On motor push
        fevent_enable(sEventAppDelivery, event);
    }
    else
    {
        Off_Motor_Push();   //Off Motor Push
        if(sParamDelivery.SumHandle > 0)    //Neu van con vi tri can tra hang
        {
          fevent_enable(sEventAppDelivery, _EVENT_DELIVERY_ENTRY);  //Vao entry Delivery
          
          Enable_Handle_Idle_Delivery(_EVENT_DELIVERY_ENTRY);       //Enable handle error 
        }
        else
        {
          fevent_active(sEventAppDelivery, _EVENT_ELEVATOR_LOWER_END);  //Chuyen sang event xu ly tiep theo
          Enable_Handle_Idle_Delivery(_EVENT_ELEVATOR_LOWER_END);       //enable handle error
        }
        
        Respond_Delivery_PcBox(RESPOND_PUSH_MOTOR);     //Phan hoi lai PcBox
    }
    return 1;
}

/*
    Elevator chay xuong tang thap nhat (Floor = 0)
*/
static uint8_t fevent_elevator_lower_end(uint8_t event)
{
    StateDebugDelivery = 6;
  
    //Kiem tra Elevator o duoi cung
    if(HAL_GPIO_ReadPin(FB_Bot_Elevator_GPIO_Port, FB_Bot_Elevator_Pin) == GPIO_PIN_SET)
    {
        if(sElevator.State == ELEVATOR_STOP)
        {
            Control_Elevator(ELEVATOR_DOWN, LEVEL_PWM_ELEVATOR);    //Elevator chay xuong duoi
        }
        fevent_enable(sEventAppDelivery, event);
    }
    else
    {
        OFF_Elevator();     //Off elevator
        sElevator.Floor = 0;
        fevent_active(sEventAppDelivery, _EVENT_LOCK_MAGNETIS_END); //Chuyen sang event xu ly tiep theo 
        Enable_Handle_Idle_Delivery(_EVENT_LOCK_MAGNETIS_END);      //Enable handle error
    }
    return 1;
}

/*
    Unlock Magnetis 
*/
static uint8_t fevent_lock_magnetis_end(uint8_t event)
{
    StateDebugDelivery = 7;
  
    //Unlock LockMagnetis
    Control_LockMagnetis(GPIO_PIN_RESET);     // UnLock LockMagnetis
    
    //Check feedback lockmagnetis
    if(sParamDelivery.LockMagnetis == LOCK_MAGNETIS_OPEN)
    {
        Respond_Delivery_PcBox(RESPOND_COMPLETE_DELIVERY);  //Phan hoi lai PcBox
        Disable_Handle_Idle_Delivery(); //Disable handle error
    }
    else
    {
        fevent_enable(sEventAppDelivery, event);
    }
    return 1;
}

/*
    Fix Motor (xoay motor 1 vong)
*/
static uint8_t fevent_fix_motor(uint8_t event)
{
    static uint8_t Handle_Once = 0;
    static uint32_t gettick_ms_error = 0;

    if(Handle_Once == 0)
    {
        gettick_ms_error = HAL_GetTick();
        Handle_Once = 1;
    }
  
    if(sParamDelivery.aPulse[sParamDelivery.PosHandle - 1] != sParamDelivery.aDataPush[sParamDelivery.PosHandle - 1])
    {
        On_Motor_Push(sParamDelivery.PosHandle);
    }
    else
    {
        Off_Motor_Push();
//        Respond_Delivery_PcBox(RESPOND_FIX_MOTOR);
        Delivery_Handle_State(DELIVERY_FREE);
        DCU_Respond(_AT_REQUEST_SERIAL, (uint8_t*)"TestMotor Success\r\n", 19, 0);
        Handle_Once = 0;
        return 1;
    }
    
    if(HAL_GetTick() - gettick_ms_error > 15000)
    {
        Off_Motor_Push();
        Delivery_Handle_State(DELIVERY_FREE);
        DCU_Respond(_AT_REQUEST_SERIAL, (uint8_t*)"TestMotor error\r\n", 17, 0);
        Handle_Once = 0;
        return 1;
    }
    
    fevent_enable(sEventAppDelivery, event);
    return 1;
}

/*
    Elevator chay len (sau khi tat 500ms)
*/
static uint8_t fevent_ctrl_elevator_upper(uint8_t event)
{
    __HAL_TIM_SetCompare (&htim3, TIM_CHANNEL_2, LEVEL_PWM_ELEVATOR);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
//    HAL_GPIO_WritePin(ON_OFF_MOTOR_PWM_2_PORT, ON_OFF_MOTOR_PWM_2_PIN, STATE_GPIO_ON);
    return 1;
}

/*
    Elevator chay xuong (sau khi tat 500ms)
*/
static uint8_t fevent_ctrl_elevator_lower(uint8_t event)
{
    __HAL_TIM_SetCompare (&htim4, TIM_CHANNEL_4, LEVEL_PWM_ELEVATOR);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
//    HAL_GPIO_WritePin(ON_OFF_MOTOR_PWM_1_PORT, ON_OFF_MOTOR_PWM_1_PIN, STATE_GPIO_ON);
    return 1;
}

/*
    Xu ly loi trong qua trinh push (Neu k thuc hien thanh cong sau 60s)
*/
static uint8_t fevent_handle_error_delivery(uint8_t event)
{
    //Reset Delivery
    Delivery_Handle_State(DELIVERY_FREE);
    fevent_disable(sEventAppDelivery, DetectErrorDelivery);
    UTIL_MEM_set(sParamDelivery.aPulse, 0x00, NUMBER_MOTOR);
    UTIL_MEM_set(sParamDelivery.aDataPush, 0x00, NUMBER_MOTOR);
    OFF_Elevator();
    Off_Motor_Push();
    
    switch(DetectErrorDelivery)
    {
        case _EVENT_LOCK_MAGNETIS_BEGIN:
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"Delivery Error: _EVENT_LOCK_MAGNETIS_BEGIN", sizeof("Delivery Error: _EVENT_LOCK_MAGNETIS_BEGIN")-1);
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
          break;
          
        case _EVENT_ELEVATOR_LOWER_BEGIN:
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"Delivery Error: _EVENT_ELEVATOR_LOWER_BEGIN", sizeof("Delivery Error: _EVENT_ELEVATOR_LOWER_BEGIN")-1);
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
          break;
          
        case _EVENT_DELIVERY_ENTRY:
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"Delivery Error: _EVENT_DELIVERY_ENTRY", sizeof("Delivery Error: _EVENT_DELIVERY_ENTRY")-1);
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
          break;
          
        case _EVENT_CALCULATOR_COORDINATES:
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"Delivery Error: _EVENT_CALCULATOR_COORDINATES", sizeof("Delivery Error: _EVENT_CALCULATOR_COORDINATES")-1);
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
          break;
          
        case _EVENT_PUSH_MOTOR:
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"Delivery Error: _EVENT_PUSH_MOTOR", sizeof("Delivery Error: _EVENT_PUSH_MOTOR")-1);
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
          break;
          
        case _EVENT_ELEVATOR_LOWER_END:
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"Delivery Error: _EVENT_ELEVATOR_LOWER_END", sizeof("Delivery Error: _EVENT_ELEVATOR_LOWER_END")-1);
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
          break;
          
        case _EVENT_LOCK_MAGNETIS_END:
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"Delivery Error: _EVENT_LOCK_MAGNETIS_END", sizeof("Delivery Error: _EVENT_LOCK_MAGNETIS_END")-1);
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
          break;
          
        default:
          break;
    }
    
//    Enable_Test_Elevator();
    return 1;
}

/*
    Test Elevator
*/
static uint8_t fevent_test_elevator(uint8_t event)
{
    static uint8_t Handle_Once = 0;
    static uint32_t gettick_ms_error = 0;

    if(Handle_Once == 0)
    {
        gettick_ms_error = HAL_GetTick();
        Handle_Once = 1;
    }

    if(sElevator.Floor != 5)
    {
        if(HAL_GPIO_ReadPin(FB_Top_Elevator_GPIO_Port, FB_Top_Elevator_Pin) == GPIO_PIN_SET)
        {
            if(sElevator.State == ELEVATOR_STOP)
            {
                Control_Elevator(ELEVATOR_UP, LEVEL_PWM_ELEVATOR);
            }
        }
        else
        {
            OFF_Elevator();
            sElevator.Floor = 5;
            gettick_ms_error = HAL_GetTick();
        }
    }
    else if(sElevator.Floor != 0)
    {
        if(HAL_GPIO_ReadPin(FB_Bot_Elevator_GPIO_Port, FB_Bot_Elevator_Pin) == GPIO_PIN_SET)
        {
            if(sElevator.State == ELEVATOR_STOP)
            {
                Control_Elevator(ELEVATOR_DOWN, LEVEL_PWM_ELEVATOR);
            }
        }
        else
        {
            OFF_Elevator();
            sElevator.Floor = 0;
            Delivery_Handle_State(DELIVERY_FREE);
            Handle_Once = 0;
            DCU_Respond(_AT_REQUEST_SERIAL, (uint8_t*)"Elevator Success\r\n", 18, 0);
            return 1;
        }
    }
  
    if(HAL_GetTick() - gettick_ms_error > 15000)
    {
        OFF_Elevator();
        DCU_Respond(_AT_REQUEST_SERIAL, (uint8_t*)"Elevator Error\r\n", 16, 0);
        Delivery_Handle_State(DELIVERY_FREE);
        Handle_Once = 0;
        return 1;
    }
  
    fevent_enable(sEventAppDelivery, event);
    return 1;
}

static uint8_t fevent_test_lock_magnetis(uint8_t event)
{
    static uint8_t countStateDebug = 0;
    Delivery_Handle_State(DELIVERY_TEST_LOCK_MAGNETIS);
    
    switch(countStateDebug)
    {
        case 0:
          Control_LockMagnetis(GPIO_PIN_SET);   //Lock Magnetis
          countStateDebug++;
          break;
          
        case 1:
          if(sParamDelivery.LockMagnetis == LOCK_MAGNETIS_CLOSE)
            countStateDebug++;
          else
          {
            DCU_Respond(PortConfig, (uint8_t*)"LOCK_MAGNETIS error\r\n", 21, 0);
            countStateDebug = 0;
            Delivery_Handle_State(DELIVERY_FREE);
            return 1;
          }
          break;
          
        case 2:
          Control_LockMagnetis(GPIO_PIN_RESET);     //UnLock Magnetis
          countStateDebug++;
          break;
          
        case 3:
          if(sParamDelivery.LockMagnetis == LOCK_MAGNETIS_OPEN)
          {
            DCU_Respond(PortConfig, (uint8_t*)"LOCK_MAGNETIS success\r\n", 23, 0);
          }
          else
          {
            DCU_Respond(PortConfig, (uint8_t*)"LOCK_MAGNETIS error\r\n", 21, 0);
          }
          countStateDebug = 0;
          Delivery_Handle_State(DELIVERY_FREE);
          return 1;
          break;
          
          
        default:
          break;
    }

    fevent_enable(sEventAppDelivery, event);
    return 1;
}

uint8_t test_elevator = 0;
static uint8_t fevent_test_ele(uint8_t event)
{
    if(sParamDelivery.StateHanlde == DELIVERY_FREE)
    {
        switch(test_elevator)
        {
            case 1:
                if(sElevator.State != ELEVATOR_DOWN)
                {
                  if(HAL_GPIO_ReadPin(FB_Bot_Elevator_GPIO_Port, FB_Bot_Elevator_Pin) == GPIO_PIN_SET)
                  {
                    Control_Elevator(ELEVATOR_DOWN, LEVEL_PWM_ELEVATOR);
                  }
                }
                break;
                
            case 2:
                if(sElevator.State != ELEVATOR_UP)
                {
                  if(HAL_GPIO_ReadPin(FB_Top_Elevator_GPIO_Port, FB_Top_Elevator_Pin) == GPIO_PIN_SET)
                  {
                    Control_Elevator(ELEVATOR_UP, LEVEL_PWM_ELEVATOR);
                  }
                }
                break;
                
            case 3:
                if(sElevator.State != ELEVATOR_STOP)
                {
                    OFF_Elevator();
                }
                break;
                
            default:
              break;
        }
    }
    
    fevent_enable(sEventAppDelivery, event);
    return 1;
}

uint8_t test_motor = 0;
static uint8_t fevent_test_motor(uint8_t event)
{
    if(sParamDelivery.StateHanlde == DELIVERY_FREE)
    {
        switch(test_motor)
        {
            case 1:
              On_Motor_Push(1);
              break;
              
            case 2:
              On_Motor_Push(2);
              break;
              
            case 3:
              On_Motor_Push(3);
              break;
              
            case 4:
              On_Motor_Push(4);
              break;
              
            case 5:
              On_Motor_Push(5);
              break;
              
            case 6:
              On_Motor_Push(6);
              break;
              
            case 7:
              On_Motor_Push(7);
              break;
              
            case 8:
              On_Motor_Push(8);
              break;
              
            case 9:
              On_Motor_Push(9);
              break;
              
            case 10:
              On_Motor_Push(10);
              break;
              
            case 100:
              Off_Motor_Push();
              break;
              
            default:
              break;
        }
    }
  
    fevent_enable(sEventAppDelivery, event);
    return 1;
}

/*================ Function Handle =================*/
/*
    @brief  Fix motor
    @param  Pos: Vi tri can Fix
*/
void Fix_Motor(uint8_t Pos)
{
    if(Pos > 0 && Pos <= NUMBER_MOTOR)
    {
        sParamDelivery.PosHandle = Pos;
        if(sParamDelivery.StateHanlde == DELIVERY_FREE)
        {
            Delivery_Handle_State(DELIVERY_TEST_MOTOR);
            sParamDelivery.aDataPush[sParamDelivery.PosHandle-1] = 1;
            sParamDelivery.aPulse[sParamDelivery.PosHandle-1] = 0;
            fevent_active(sEventAppDelivery, _EVENT_FIX_MOTOR);
        }
    }
}

/*
    @brief Delivery
*/
void Delivery_Entry(void)
{
    //Tinh tong so vi tri can tra hang
    sParamDelivery.SumHandle = 0;
    for(uint8_t i = 0; i<NUMBER_MOTOR; i++)
    {
        if(sParamDelivery.aDataPush[i] != 0)
            sParamDelivery.SumHandle++;
    }
    
    if(sParamDelivery.SumHandle > 0)
    {
        //Xac nhan vao che do tra hang va rest xung tra hang
        Delivery_Handle_State(DELIVERY_PURCHASE);
        UTIL_MEM_set(sParamDelivery.aPulse, 0x00, NUMBER_MOTOR);
        fevent_active(sEventAppDelivery, _EVENT_LOCK_MAGNETIS_BEGIN);
        Enable_Handle_Idle_Delivery(_EVENT_LOCK_MAGNETIS_BEGIN);
        
        StateDebugDelivery = 0;
        
        //Xac nhan app Elevator busy (thuc hien cho cac nhiem vu khac)
        sStatusApp.Delivery      = _APP_BUSY;
        fevent_disable(sEventAppDelivery, _EVENT_WAIT_FREE_MOTOR);
    }
}

void IRQ_Pluse_OffMotorPush(uint8_t Pos)
{
    uint8_t Pos_Push = 0;
    Pos_Push = Pos - 1;
    sParamDelivery.aPulse[Pos_Push]++;
    if(sParamDelivery.aPulse[Pos_Push] == sParamDelivery.aDataPush[Pos_Push])
    {
        Off_Motor_Push();
    }
}


uint32_t GetTick_TimePushMotor = 0;
uint8_t State_Motor = 0;
/*
	@brief  ON motor tai vi tri Pos
	@param  Pos vi tri motor muon chay
*/
void On_Motor_Push(uint8_t Pos)
{
    if(State_Motor == 0)
    {
        State_Motor = 1;
      
        GetTick_TimePushMotor = HAL_GetTick();
        
        uint8_t Pos_Push = 0;
        Pos_Push = Pos - 1;
        HAL_GPIO_WritePin(CTRL_MOTOR_PORT[Pos_Push], CTRL_MOTOR_PIN[Pos_Push], GPIO_PIN_SET);
        HAL_GPIO_WritePin(ON_OFF_V2_GPIO_Port, ON_OFF_V2_Pin, GPIO_PIN_SET); 
    }
}

/*
    @brief  OFF toan bo motor
*/
void Off_Motor_Push(void)
{
    if(State_Motor == 1)
    {
        State_Motor = 0;
        
        uint8_t i = 0;
        for(i = 0; i < NUMBER_MOTOR ; i++)
            HAL_GPIO_WritePin(CTRL_MOTOR_PORT[i], CTRL_MOTOR_PIN[i], GPIO_PIN_RESET); 
        
        HAL_GPIO_WritePin(ON_OFF_V2_GPIO_Port, ON_OFF_V2_Pin, GPIO_PIN_RESET); 
        
        GetTick_TimePushMotor = HAL_GetTick() - GetTick_TimePushMotor;
        
        uint8_t aData[10]={0};
        uint8_t length = 0;
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"TimePush: ", sizeof("TimePush: ")-1);
        length = Convert_Int_To_String(aData, GetTick_TimePushMotor);
        UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
        
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
    }
}

void Respond_Delivery_PcBox(eStateRespondPcBox StateRes)
{
    switch(StateRes)
    {
        case RESPOND_PUSH_MOTOR:
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_ON_GOING_PUSH;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x02;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamDelivery.PosHandle;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamDelivery.aDataPush[sParamDelivery.PosHandle-1];
          break;
          
        case RESPOND_COMPLETE_DELIVERY:
            Delivery_Handle_State(DELIVERY_FREE);
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_COMPLETE_PUSH;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x02;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamDelivery.Weighing >> 8;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamDelivery.Weighing;
          break;
          
        case RESPOND_FIX_MOTOR:
            Delivery_Handle_State(DELIVERY_FREE);
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_PC_BOX_FIX_MOTOR;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamDelivery.PosHandle;
            fevent_enable(sEventAppDelivery, _EVENT_WAIT_FREE_MOTOR);
          break;
          
        default:
          break;
    }
 
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
    AppDelivery_Debug(StateRes);
}

/*
    @brief  Debug trang thai push
*/
void AppDelivery_Debug(uint8_t KindDelivery)
{
#ifdef USING_APP_DELIVERY_DEBUG
    uint8_t aData[10];
    uint8_t length = 0;
    
    switch(KindDelivery)
    {
        case RESPOND_PUSH_MOTOR:
          UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_ctrl_motor: Push:", sizeof("app_ctrl_motor: Push:")-1);
          length = Convert_Int_To_String(aData, sParamDelivery.PosHandle);
          UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
          
          UTIL_Printf(DBLEVEL_M, (uint8_t*)"Number", sizeof("Number")-1);
          length = Convert_Int_To_String(aData, sParamDelivery.aDataPush[sParamDelivery.PosHandle-1]);
          UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
          break;
          
        case RESPOND_FIX_MOTOR:
          UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_ctrl_motor: FixMotor:", sizeof("app_ctrl_motor: FixMotor:")-1);
          length = Convert_Int_To_String(aData, sParamDelivery.PosHandle);
          UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
          break;
          
        case RESPOND_COMPLETE_DELIVERY:
          UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_ctrl_motor: Weight:", sizeof("app_ctrl_motor: Weight:")-1);
          length = Convert_Int_To_String(aData, sParamDelivery.Weighing);
          UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
          break;
      
        default:
          break;
    }
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
#endif
}

void OFF_Elevator(void)
{
    if(sElevator.State != ELEVATOR_STOP)
        Debug_Encoder();
  
    __HAL_TIM_SetCompare (&htim3, TIM_CHANNEL_2, 0);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
//    HAL_GPIO_WritePin(ON_OFF_MOTOR_PWM_2_PORT, ON_OFF_MOTOR_PWM_2_PIN, STATE_GPIO_OFF);
  
    __HAL_TIM_SetCompare (&htim4, TIM_CHANNEL_4, 0);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
//    HAL_GPIO_WritePin(ON_OFF_MOTOR_PWM_1_PORT, ON_OFF_MOTOR_PWM_1_PIN, STATE_GPIO_OFF);
    
    sElevator.State = ELEVATOR_STOP;
}

void Control_Elevator(uint8_t Direction, uint8_t Level)
{
    OFF_Elevator();
    if(Direction == ELEVATOR_UP)
        fevent_enable(sEventAppDelivery, _EVENT_CTRL_ELEVATOR_UPPER);
    else
        fevent_enable(sEventAppDelivery, _EVENT_CTRL_ELEVATOR_LOWER);
    
    sElevator.State = Direction;
    
    sParamDelivery.Encoder_C1 = 0;
    sParamDelivery.Encoder_C2 = 0;
}

void Enable_Handle_Idle_Delivery(uint8_t State)
{
    fevent_enable(sEventAppDelivery, _EVENT_HANDLE_ERROR_DELIVERY);
    DetectErrorDelivery = State;
}

void Disable_Handle_Idle_Delivery(void)
{
    fevent_disable(sEventAppDelivery, _EVENT_HANDLE_ERROR_DELIVERY);
}

void Enable_Test_Elevator(void)
{
    Delivery_Handle_State(DELIVERY_TEST_ELEVATOR);
    fevent_active(sEventAppDelivery, _EVENT_TEST_ELEVATOR);
    sParamDelivery.FloorHandle = 5;
    sElevator.Floor = 0;
}

void Debug_Encoder(void)
{
    uint8_t aData[10]={0};
    uint8_t length = 0;
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"Encoder_C1: ", sizeof("Encoder_C1: ")-1);
    length = Convert_Int_To_String(aData, sParamDelivery.Encoder_C1);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"Encoder_C2: ", sizeof("Encoder_C2: ")-1);
    length = Convert_Int_To_String(aData, sParamDelivery.Encoder_C2);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
}

void Control_LockMagnetis(GPIO_PinState StateCtrl)
{
    HAL_GPIO_WritePin(SW_LOCK_PORT, SW_LOCK_PIN, StateCtrl);
}

void Debug_Floor_Elevator(uint8_t Floor)
{
    uint8_t aData[10]={0};
    uint8_t length = 0;
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"Floor: ", sizeof("Floor: ")-1);
    length = Convert_Int_To_String(aData, Floor);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
}

void Delivery_Handle_State(uint8_t State)
{
    switch(State)
    {
        case DELIVERY_FREE:
          sParamDelivery.StateHanlde = DELIVERY_FREE;
          UTIL_Printf(DBLEVEL_M, (uint8_t*)"Hanlde: DELIVERY_FREE", sizeof("Hanlde: DELIVERY_FREE")-1);
          break;
          
        case DELIVERY_PURCHASE:
          sParamDelivery.StateHanlde = DELIVERY_PURCHASE;
          UTIL_Printf(DBLEVEL_M, (uint8_t*)"Hanlde: DELIVERY_PURCHASE", sizeof("Hanlde: DELIVERY_PURCHASE")-1);
          break;
          
        case DELIVERY_TEST_ELEVATOR:
          sParamDelivery.StateHanlde = DELIVERY_TEST_ELEVATOR;
          UTIL_Printf(DBLEVEL_M, (uint8_t*)"Hanlde: DELIVERY_TEST_ELEVATOR", sizeof("Hanlde: DELIVERY_TEST_ELEVATOR")-1);
          break;
          
        case DELIVERY_TEST_MOTOR:
          sParamDelivery.StateHanlde = DELIVERY_TEST_MOTOR;
          UTIL_Printf(DBLEVEL_M, (uint8_t*)"Hanlde: DELIVERY_TEST_MOTOR", sizeof("Hanlde: DELIVERY_TEST_MOTOR")-1);
          break;
          
        case DELIVERY_TEST_LOCK_MAGNETIS:
          sParamDelivery.StateHanlde = DELIVERY_TEST_LOCK_MAGNETIS;
          UTIL_Printf(DBLEVEL_M, (uint8_t*)"Hanlde: DELIVERY_TEST_LOCK_MAGNETIS", sizeof("Hanlde: DELIVERY_TEST_LOCK_MAGNETIS")-1);
          break;
          
        default:
          break;
    }
  
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
}

/*======================== Function Handle ========================*/
uint8_t AppDelivery_Task(void)
{
	uint8_t i = 0;
	uint8_t Result = false;

	for (i = 0; i < _EVENT_MOTOR_END; i++)
	{
		if (sEventAppDelivery[i].e_status == 1)
		{
            Result = true;

			if ((sEventAppDelivery[i].e_systick == 0) ||
					((HAL_GetTick() - sEventAppDelivery[i].e_systick)  >=  sEventAppDelivery[i].e_period))
			{
                sEventAppDelivery[i].e_status = 0;  //Disable event
				sEventAppDelivery[i].e_systick= HAL_GetTick();
				sEventAppDelivery[i].e_function_handler(i);
			}
		}
	}
    
	return Result;
}



