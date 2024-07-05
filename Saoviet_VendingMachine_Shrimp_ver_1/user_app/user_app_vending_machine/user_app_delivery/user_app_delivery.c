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
static uint8_t fevent_finish_delivery(uint8_t event);

static uint8_t fevent_handle_error_delivery(uint8_t event);
static uint8_t fevent_free_elevator_lower(uint8_t event);

static uint8_t fevent_fix_motor(uint8_t event);

static uint8_t fevent_control_pwm_elevator(uint8_t event);
static uint8_t fevent_ctrl_elevator_upper(uint8_t event);
static uint8_t fevent_ctrl_elevator_lower(uint8_t event);
static uint8_t fevent_alter_pwm_elevator(uint8_t event);


static uint8_t fevent_test_elevator(uint8_t event);
static uint8_t fevent_elevator_floor(uint8_t event);

/*================ Struct =================*/
sEvent_struct         sEventAppDelivery[] =
{   
  { _EVENT_MOTOR_ENTRY,               1, 0, 0,                          fevent_motor_entry},
  { _EVENT_WAIT_FREE_MOTOR,           0, 0, 60000,                      fevent_wait_free_motor},
  
  { _EVENT_LOCK_MAGNETIS_BEGIN,       0, 5, 0,                          fevent_lock_magnetis_begin},
  { _EVENT_ELEVATOR_LOWER_BEGIN,      0, 5, 2000,                       fevent_elevator_lower_begin},
  { _EVENT_DELIVERY_ENTRY,            0, 5, 2000,                       fevent_delivery_entry},
  { _EVENT_CALCULATOR_COORDINATES,    0, 5, 0,                          fevent_calculator_coordinates},
  { _EVENT_PUSH_MOTOR,                0, 5, 2000,                       fevent_push_motor},
  { _EVENT_ELEVATOR_LOWER_END,        0, 5, 2000,                       fevent_elevator_lower_end},
  { _EVENT_LOCK_MAGNETIS_END,         0, 5, 0,                          fevent_lock_magnetis_end},
  { _EVENT_FINISH_DELIVERY,           0, 5, 5000,                       fevent_finish_delivery},
  
  { _EVENT_HANDLE_ERROR_DELIVERY,     0, 5, 20000,                      fevent_handle_error_delivery},
  
  { _EVENT_FREE_ELEVATOR_LOWER,       1, 5, 5000,                       fevent_free_elevator_lower},
  
  { _EVENT_FIX_MOTOR,                 0, 0, 0,                          fevent_fix_motor},
  
  { _EVENT_CONTROL_PWM_ELEVATOR,      1, 0, 0,                          fevent_control_pwm_elevator},
  { _EVENT_ALTER_PWM_ELEVATOR,        1, 5, 200,                        fevent_alter_pwm_elevator},
  { _EVENT_CTRL_ELEVATOR_UPPER,       0, 5, 500,                        fevent_ctrl_elevator_upper},
  { _EVENT_CTRL_ELEVATOR_LOWER,       0, 5, 500,                        fevent_ctrl_elevator_lower},
  
  { _EVENT_TEST_ELEVATOR,             0, 5, 0,                          fevent_test_elevator},
  { _EVENT_ELEVATOR_FLOOR,            0, 5, 0,                          fevent_elevator_floor},
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
Struct_MotorPush            sMotorPush = {0};
uint8_t                     DetectErrorDelivery = 0;

uint8_t                     StateDebugDelivery = 0;
//uint8_t                     Count_Error = 0;
/*================ Function Handler =================*/
static uint8_t fevent_motor_entry(uint8_t event)
{   
    HAL_GPIO_WritePin(PW_LOCK_PORT, PW_LOCK_PIN, GPIO_PIN_SET); //Cap nguon cho Lock Magnetis
    HAL_GPIO_WritePin(ON_OFF_Pos_Elevator_GPIO_Port, ON_OFF_Pos_Elevator_Pin, GPIO_PIN_SET);
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
//    Count_Error++;
//    if(Count_Error >= 5)
//    {
//        Count_Error = 0;
//        return 1;
//    }
    
    StateDebugDelivery = 1;

    //Check Feedback LockMagnetis.
    if(sParamDelivery.LockMagnetis == LOCK_MAGNETIS_CLOSE)
    {
        Control_LockMagnetis(GPIO_PIN_RESET);     // Lock LockMagnetis
        
        fevent_active(sEventAppDelivery, _EVENT_ELEVATOR_LOWER_BEGIN);  // Chuyen sang event xu ly tiep theo
        
        Enable_Handle_Idle_Delivery(_EVENT_ELEVATOR_LOWER_BEGIN, 20000);       // Enable handle error
    }
    else
    {
        Control_LockMagnetis(GPIO_PIN_RESET);     // Lock LockMagnetis
        
        fevent_active(sEventAppDelivery, _EVENT_ELEVATOR_LOWER_BEGIN);  // Chuyen sang event xu ly tiep theo
        
        Enable_Handle_Idle_Delivery(_EVENT_ELEVATOR_LOWER_BEGIN, 20000);       // Enable handle error
      
        
//        fevent_enable(sEventAppDelivery, event);
    }
    return 1;
}

/*
    Kiem tra Elevator tang duoi cung (Floor = 0)
*/
static uint8_t fevent_elevator_lower_begin(uint8_t event)
{
    StateDebugDelivery = 2;
    uint8_t Pos_Elevator = 0;
    
//            fevent_active(sEventAppDelivery, _EVENT_DELIVERY_ENTRY);    //Chuyen sang event xu ly tiep theo
//
//            Enable_Handle_Idle_Delivery(_EVENT_DELIVERY_ENTRY, 20000);         //Enable handle error
//            
//            return 1;
    
    Pos_Elevator = Check_Top_Bot_Elevator();
    
    switch(Pos_Elevator)
    {
        case _ELEVATOR_FLOOR:
        case _ELEVATOR_TOP:
            Control_Elevator(ELEVATOR_DOWN, LEVEL_PWM_ELEVATOR_DOWN);    //Elevator di chuyen xuong duoi

            fevent_enable(sEventAppDelivery, event);
            break;
            
        case _ELEVATOR_BOT:
            fevent_active(sEventAppDelivery, _EVENT_DELIVERY_ENTRY);    //Chuyen sang event xu ly tiep theo

            Enable_Handle_Idle_Delivery(_EVENT_DELIVERY_ENTRY, 20000);         //Enable handle error
            break; 
            
        case _ELEVATOR_ERROR:
          fevent_enable(sEventAppDelivery, event);
          break;
          
        default:
          break;
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
                sMotorPush.aPulse[sParamDelivery.PosHandle] = 0;    //Cai xung da tra hang = 0
                sParamDelivery.SumHandle--;     //Xac nhan so luong vi tri can tra hang con lai
                sParamDelivery.PosHandle++;     //Xac nhan vi tri tra hang hien tai
                switch(sParamDelivery.PosHandle)    //Xac nhan tang tra hang
                {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                      sElevator.FloorHandle = 4;
                      break;
                      
                    case 5:
                    case 6: 
                      sElevator.FloorHandle = 3;
                      break;
                      
                    case 7:
                    case 8:
                      sElevator.FloorHandle = 2;
                      break;
                      
                    case 9:
                    case 10:
                      sElevator.FloorHandle = 1;
                      break;
                      
                    default:
                      break;
                }
                fevent_active(sEventAppDelivery, _EVENT_CALCULATOR_COORDINATES);    //Chuyen sang event xu ly tiep theo
                    
                Enable_Handle_Idle_Delivery(_EVENT_CALCULATOR_COORDINATES, 20000);         //Enable handle error 
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
    uint8_t Pos_Elevator = 0;
    
//            Control_Elevator(ELEVATOR_STOP, LEVEL_PWM_ELEVATOR_OFF); //Stop elevator
//            fevent_enable(sEventAppDelivery,_EVENT_PUSH_MOTOR); //Chuyen sang event su ly tiep theo 
//            
//            Enable_Handle_Idle_Delivery(_EVENT_PUSH_MOTOR, 60000); //Enable handle error
//            
//            return 1;
    
    Pos_Elevator = Check_Top_Bot_Elevator();
    
    if(sElevator.FloorCurrent != sElevator.FloorHandle) //Kiem tra Elevator da den tang can tra hang
    {
        switch(Pos_Elevator)
        {
            case _ELEVATOR_BOT:
              Control_Elevator(ELEVATOR_UP, LEVEL_PWM_ELEVATOR_UP);
              break;
              
            case _ELEVATOR_FLOOR:
                if(sElevator.FloorCurrent < sElevator.FloorHandle)
                    Control_Elevator(ELEVATOR_UP, LEVEL_PWM_ELEVATOR_UP);
                else
                    Control_Elevator(ELEVATOR_DOWN, LEVEL_PWM_ELEVATOR_DOWN);
                
                break;
                
            case _ELEVATOR_TOP:
                Control_Elevator(ELEVATOR_DOWN, LEVEL_PWM_ELEVATOR_DOWN);
                break;
                
            case _ELEVATOR_ERROR:
              break;
              
            default:
              break;
        }
        fevent_active(sEventAppDelivery, event);
    }
    else
    {
        Control_Elevator(ELEVATOR_STOP, LEVEL_PWM_ELEVATOR_OFF); //Stop elevator
        fevent_enable(sEventAppDelivery,_EVENT_PUSH_MOTOR); //Chuyen sang event su ly tiep theo 
        
        Enable_Handle_Idle_Delivery(_EVENT_PUSH_MOTOR, 60000); //Enable handle error
    }
  
    return 1;
}

/*
    Push Motor tra hang
*/
static uint8_t fevent_push_motor(uint8_t event)
{
    StateDebugDelivery = 5;
    
//        sMotorPush.aPulse[sParamDelivery.PosHandle - 1] = sParamDelivery.aDataPush[sParamDelivery.PosHandle - 1];
//    
//        Off_Motor_Push();   //Off Motor Push
//        if(sParamDelivery.SumHandle > 0)    //Neu van con vi tri can tra hang
//        {
//          fevent_enable(sEventAppDelivery, _EVENT_DELIVERY_ENTRY);  //Vao entry Delivery
//          
//          Enable_Handle_Idle_Delivery(_EVENT_DELIVERY_ENTRY, 20000);       //Enable handle error 
//        }
//        else
//        {
//          sElevator.FloorHandle = 0;
//          fevent_active(sEventAppDelivery, _EVENT_ELEVATOR_LOWER_END);  //Chuyen sang event xu ly tiep theo
//          Enable_Handle_Idle_Delivery(_EVENT_ELEVATOR_LOWER_END, 20000);       //enable handle error
//        }
//        
//        Respond_Delivery_PcBox(RESPOND_PUSH_MOTOR);     //Phan hoi lai PcBox
//        sParamDelivery.aDataPush[sParamDelivery.PosHandle - 1] = 0;
//        
//        return 1;
  
    //Kiem tra so xung tra hang da bang so luong hang can tra tai vi tri PosHandle
    if(sMotorPush.aPulse[sParamDelivery.PosHandle - 1] != sParamDelivery.aDataPush[sParamDelivery.PosHandle - 1])
    {
        if(sMotorPush.State == MOTOR_PUSH_OFF)
        {  
            On_Motor_Push(sParamDelivery.PosHandle);    //On motor push
        }
        else if(HAL_GetTick() - sMotorPush.TimeOnMotor > TIME_MOTOR_PUSH_LATE)
        {
            Off_Motor_Push();
            sMotorPush.aPulse[sParamDelivery.PosHandle - 1]++;
            DCU_Respond(_AT_REQUEST_SERIAL, (uint8_t*)"Push_Late\r\n", 11, 0); 
        }
        
        fevent_active(sEventAppDelivery, event);
    }
    else
    {
        Off_Motor_Push();   //Off Motor Push
        if(sParamDelivery.SumHandle > 0)    //Neu van con vi tri can tra hang
        {
          fevent_enable(sEventAppDelivery, _EVENT_DELIVERY_ENTRY);  //Vao entry Delivery
          
          Enable_Handle_Idle_Delivery(_EVENT_DELIVERY_ENTRY, 20000);       //Enable handle error 
        }
        else
        {
          sElevator.FloorHandle = 0;
          fevent_active(sEventAppDelivery, _EVENT_ELEVATOR_LOWER_END);  //Chuyen sang event xu ly tiep theo
          Enable_Handle_Idle_Delivery(_EVENT_ELEVATOR_LOWER_END, 20000);       //enable handle error
        }
        
        Respond_Delivery_PcBox(RESPOND_PUSH_MOTOR);     //Phan hoi lai PcBox
        sParamDelivery.aDataPush[sParamDelivery.PosHandle - 1] = 0;
    }
    return 1;
}

/*
    Elevator chay xuong tang thap nhat (Floor = 0)
*/
static uint8_t fevent_elevator_lower_end(uint8_t event)
{
    StateDebugDelivery = 6;
    uint8_t Pos_Elevator = 0;
    
//            fevent_active(sEventAppDelivery, _EVENT_LOCK_MAGNETIS_END); //Chuyen sang event xu ly tiep theo 
//            Enable_Handle_Idle_Delivery(_EVENT_LOCK_MAGNETIS_END, 20000);      //Enable handle error
//            
//            return 1;
    
    Pos_Elevator = Check_Top_Bot_Elevator();
    
    switch(Pos_Elevator)
    {
        case _ELEVATOR_FLOOR:
        case _ELEVATOR_TOP:
            if(sElevator.State == ELEVATOR_STOP)
                Control_Elevator(ELEVATOR_DOWN, LEVEL_PWM_ELEVATOR_DOWN);    //Elevator chay xuong duoi

            fevent_enable(sEventAppDelivery, event);
            break;
            
        case _ELEVATOR_BOT:
            fevent_active(sEventAppDelivery, _EVENT_LOCK_MAGNETIS_END); //Chuyen sang event xu ly tiep theo 
            Enable_Handle_Idle_Delivery(_EVENT_LOCK_MAGNETIS_END, 20000);      //Enable handle error
            break;
            
        case _ELEVATOR_ERROR:
          fevent_enable(sEventAppDelivery, event);
          break;
          
        default:
          break;
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
    Control_LockMagnetis(GPIO_PIN_SET);     // UnLock LockMagnetis
    
    fevent_enable(sEventAppDelivery, _EVENT_FINISH_DELIVERY); //Chuyen sang event xu ly tiep theo 
    Enable_Handle_Idle_Delivery(_EVENT_FINISH_DELIVERY, 20000);      //Enable handle error
    
//    //Check feedback lockmagnetis
//    if(sParamDelivery.LockMagnetis == LOCK_MAGNETIS_OPEN)
//    {
//        Respond_Delivery_PcBox(RESPOND_COMPLETE_DELIVERY);  //Phan hoi lai PcBox
//        Disable_Handle_Idle_Delivery(); //Disable handle error
//        
//        DCU_Respond(_AT_REQUEST_SERIAL, (uint8_t*)"Purchase Success\r\n", 18, 0);
//        Delivery_Handle_State(DELIVERY_FREE);
//    }
//    else
//    {
//        fevent_enable(sEventAppDelivery, event);
//    }
    return 1;
}

/*
    Xu ly hoan thanh ban hang
*/
static uint8_t fevent_finish_delivery(uint8_t event)
{
    StateDebugDelivery = 8;
    Respond_Delivery_PcBox(RESPOND_COMPLETE_DELIVERY);  //Phan hoi lai PcBox
    Delivery_Handle_State(DELIVERY_FREE);
    
    Disable_Handle_Idle_Delivery(); //Disable handle error
    DCU_Respond(_AT_REQUEST_SERIAL, (uint8_t*)"Purchase Success\r\n", 18, 0);
    return 1;
}

/*
    Xu ly loi trong qua trinh push (Neu k thuc hien thanh cong sau 60s)
*/
static uint8_t fevent_handle_error_delivery(uint8_t event)
{
    //Reset Delivery
    fevent_disable(sEventAppDelivery, DetectErrorDelivery);
    
    Control_Elevator(ELEVATOR_STOP, LEVEL_PWM_ELEVATOR_OFF);
    Off_Motor_Push();
    
    Respond_Delivery_PcBox(RESPOND_WARNING_DELIVERY);
    Delivery_Handle_State(DELIVERY_FREE);
    
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
          
        case _EVENT_FINISH_DELIVERY:
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"Delivery Error: _EVENT_FINISH_DELIVERY", sizeof("Delivery Error: _EVENT_FINISH_DELIVERY")-1);
            UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
          break;
          
        default:
          break;
    }
//    Enable_Test_Elevator();
    return 1;
}

/*
    Di chuyen thang nang xuong duoi cung neu delivery free
*/
static uint8_t fevent_free_elevator_lower(uint8_t event)
{
    uint8_t Pos_Elevator = 0;
    
    if(sParamDelivery.StateHanlde == DELIVERY_FREE)
    {
        Pos_Elevator = Check_Top_Bot_Elevator();
        switch(Pos_Elevator)
        {
            case _ELEVATOR_FLOOR:
            case _ELEVATOR_TOP:
                Control_Elevator(ELEVATOR_DOWN, LEVEL_PWM_ELEVATOR_DOWN);    //Elevator di chuyen xuong duoi
                break;
                
            case _ELEVATOR_BOT:
                break; 
                
            case _ELEVATOR_ERROR:
              break;
              
            default:
              break;
        }
    }
    
    fevent_enable(sEventAppDelivery, event);
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
  
    if(sMotorPush.aPulse[sParamDelivery.PosHandle - 1] != sParamDelivery.aDataPush[sParamDelivery.PosHandle - 1])
    {
        On_Motor_Push(sParamDelivery.PosHandle);
    }
    else
    {
        Respond_Delivery_PcBox(RESPOND_FIX_MOTOR);
        Delivery_Handle_State(DELIVERY_FREE);
        Off_Motor_Push();
        DCU_Respond(_AT_REQUEST_SERIAL, (uint8_t*)"FIX_MOTOR Success\r\n", 19, 0);
        Handle_Once = 0;
        return 1;
    }
    
    if(HAL_GetTick() - gettick_ms_error > TIME_MOTOR_PUSH_LATE)
    {
        Respond_Delivery_PcBox(RESPOND_FIX_MOTOR);
        Off_Motor_Push();
        Delivery_Handle_State(DELIVERY_FREE);
        DCU_Respond(_AT_REQUEST_SERIAL, (uint8_t*)"FIX_MOTOR error\r\n", 17, 0);
        Handle_Once = 0;
        return 1;
    }
    
    fevent_enable(sEventAppDelivery, event);
    return 1;
}

static uint8_t fevent_control_pwm_elevator(uint8_t event)
{
    switch(sElevator.State)
    {
        case ELEVATOR_STOP:
          sElevator.Level_PWM = 0;
          break;
          
        case ELEVATOR_UP:
          if(sElevator.FloorCurrent + 1 == sElevator.FloorHandle)
            sElevator.Level_PWM = LEVEL_PWM_ELEVATOR_UP_REDUCE;
          else
            sElevator.Level_PWM = LEVEL_PWM_ELEVATOR_UP;
          break;
          
        case ELEVATOR_DOWN:
          if(sElevator.FloorCurrent == sElevator.FloorHandle + 1)
            sElevator.Level_PWM = LEVEL_PWM_ELEVATOR_DOWN_REDUCE;
          else
            sElevator.Level_PWM = LEVEL_PWM_ELEVATOR_DOWN;
          break;
    
        default:
        break;
    }

    fevent_active(sEventAppDelivery, event);
    return 1;
}

/*
    Elevator alter PWM
*/
static uint8_t fevent_alter_pwm_elevator(uint8_t event)
{
    if(sElevator.RunLevel_PWM != sElevator.Level_PWM)
    {
        if(sElevator.RunLevel_PWM < sElevator.Level_PWM)
            sElevator.RunLevel_PWM++;
        else if(sElevator.RunLevel_PWM > sElevator.Level_PWM)
            sElevator.RunLevel_PWM--;
        
        switch(sElevator.State)
        {
            case ELEVATOR_STOP:
              break;
              
            case ELEVATOR_UP:
              if(sElevator.Status == ELEVATOR_ON)
              {
                fevent_active(sEventAppDelivery, _EVENT_CTRL_ELEVATOR_UPPER);
              }
              break;
              
            case ELEVATOR_DOWN:
              if(sElevator.Status == ELEVATOR_ON)
              {
                fevent_active(sEventAppDelivery, _EVENT_CTRL_ELEVATOR_LOWER);
              }
              break;
              
        
            default:
            break;
        }
    }
     
    fevent_enable(sEventAppDelivery, event);
    return 1;
}

/*
    Elevator chay len (sau khi tat 500ms)
*/
static uint8_t fevent_ctrl_elevator_upper(uint8_t event)
{
    if(sElevator.State != ELEVATOR_STOP)
    {
        __HAL_TIM_SetCompare (&htim3, TIM_CHANNEL_2, sElevator.RunLevel_PWM);
        HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    //    HAL_GPIO_WritePin(ON_OFF_MOTOR_PWM_2_PORT, ON_OFF_MOTOR_PWM_2_PIN, STATE_GPIO_ON);
        sElevator.Status = ELEVATOR_ON;
    }
    return 1;
}

/*
    Elevator chay xuong (sau khi tat 500ms)
*/
static uint8_t fevent_ctrl_elevator_lower(uint8_t event)
{
    if(sElevator.State != ELEVATOR_STOP)
    {
        __HAL_TIM_SetCompare (&htim4, TIM_CHANNEL_4, sElevator.RunLevel_PWM);
        HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
    //    HAL_GPIO_WritePin(ON_OFF_MOTOR_PWM_1_PORT, ON_OFF_MOTOR_PWM_1_PIN, STATE_GPIO_ON);
        sElevator.Status = ELEVATOR_ON;
    }
    return 1;
}

/*
    Test Elevator
*/
static uint8_t fevent_test_elevator(uint8_t event)
{
    static uint8_t Handle_Once = 0;
    static uint32_t gettick_ms_error = 0;
    
    static uint8_t StateHandle = 0;
    
    uint8_t Pos_Elevator = 0;
    
    Pos_Elevator = Check_Top_Bot_Elevator();

    if(Handle_Once == 0)
    {
        gettick_ms_error = HAL_GetTick();
        Handle_Once = 1;
    }

    if(StateHandle == 0)
    {
        switch(Pos_Elevator)
        {
            case _ELEVATOR_BOT:
            case _ELEVATOR_FLOOR:
                    Control_Elevator(ELEVATOR_UP, LEVEL_PWM_ELEVATOR_UP);
                    
                break;
                
            case _ELEVATOR_TOP:
                gettick_ms_error = HAL_GetTick();
                StateHandle = 1;
                sElevator.FloorHandle = 0;
                break;
                
            case _ELEVATOR_ERROR:
              break;
              
            default:
              break;
        }
    }
    
    if(StateHandle == 1)
    {
        switch(Pos_Elevator)
        {
            case _ELEVATOR_FLOOR:  
            case _ELEVATOR_TOP:
                Control_Elevator(ELEVATOR_DOWN, LEVEL_PWM_ELEVATOR_DOWN);
                
                break;
                
            case _ELEVATOR_BOT:
                Delivery_Handle_State(DELIVERY_FREE);
                DCU_Respond(_AT_REQUEST_SERIAL, (uint8_t*)"Elevator Success\r\n", 18, 0);
                Handle_Once = 0;
                StateHandle = 0;
                return 1;
                
            case _ELEVATOR_ERROR:
                break;
              
            default:
              break;
        }
    }
  
    if(HAL_GetTick() - gettick_ms_error > 15000)
    {
        Control_Elevator(ELEVATOR_STOP, LEVEL_PWM_ELEVATOR_OFF);
        DCU_Respond(_AT_REQUEST_SERIAL, (uint8_t*)"Elevator Error\r\n", 16, 0);
        Delivery_Handle_State(DELIVERY_FREE);
        Handle_Once = 0;
        StateHandle = 0;
        return 1;
    }
  
    fevent_enable(sEventAppDelivery, event);
    return 1;
}

static uint8_t fevent_elevator_floor(uint8_t event)
{
    static uint8_t Handle_Once = 0;
    static uint32_t gettick_ms_error = 0;

    uint8_t Pos_Elevator = 0;
    
    Pos_Elevator = Check_Top_Bot_Elevator();

    if(Handle_Once == 0)
    {
        gettick_ms_error = HAL_GetTick();
        Handle_Once = 1;
    }

    if(sElevator.FloorCurrent != sElevator.FloorHandle) //Kiem tra Elevator da den tang can tra hang
    {
        switch(Pos_Elevator)
        {
            case _ELEVATOR_BOT:
              Control_Elevator(ELEVATOR_UP, LEVEL_PWM_ELEVATOR_UP);
              break;
              
            case _ELEVATOR_FLOOR:
                if(sElevator.FloorCurrent < sElevator.FloorHandle)
                {
                    Control_Elevator(ELEVATOR_UP, LEVEL_PWM_ELEVATOR_UP);
                }
                else
                {
                    Control_Elevator(ELEVATOR_DOWN, LEVEL_PWM_ELEVATOR_DOWN);
                }
                
                break;
                
            case _ELEVATOR_TOP:
                Control_Elevator(ELEVATOR_DOWN, LEVEL_PWM_ELEVATOR_DOWN);
                break;
                
            case _ELEVATOR_ERROR:
              break;
              
            default:
              break;
        }
    }
    else
    {
        Control_Elevator(ELEVATOR_STOP, LEVEL_PWM_ELEVATOR_OFF);
        Delivery_Handle_State(DELIVERY_FREE);
        DCU_Respond(_AT_REQUEST_SERIAL, (uint8_t*)"Floor Success\r\n", 15, 0);
        Handle_Once = 0;
        return 1;
    }

  
    if(HAL_GetTick() - gettick_ms_error > 15000)
    {
        Control_Elevator(ELEVATOR_STOP, LEVEL_PWM_ELEVATOR_OFF);
        DCU_Respond(_AT_REQUEST_SERIAL, (uint8_t*)"Floor Error\r\n", 13, 0);
        Delivery_Handle_State(DELIVERY_FREE);
        Handle_Once = 0;
        return 1;
    }
  
    fevent_enable(sEventAppDelivery, event);
    return 1;
}

/*================ Function Handle =================*/
/*
    @brief  Fix motor
    @param  Pos: Vi tri can Fix
*/
uint8_t Fix_Motor(uint8_t Pos)
{
    if(sParamDelivery.StateHanlde == DELIVERY_FREE)
    {
        if(Pos > 0 && Pos <= NUMBER_MOTOR)
        {
            Reset_StateDelivery();
            sParamDelivery.PosHandle = Pos;
            Delivery_Handle_State(DELIVERY_FIX_MOTOR);
            sParamDelivery.aDataPush[sParamDelivery.PosHandle-1] = 1;
            sMotorPush.aPulse[sParamDelivery.PosHandle-1] = 0;
            fevent_active(sEventAppDelivery, _EVENT_FIX_MOTOR);
            return 1;
        }
    }
    return 0;
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
        UTIL_MEM_set(sMotorPush.aPulse, 0x00, NUMBER_MOTOR);

        fevent_active(sEventAppDelivery, _EVENT_LOCK_MAGNETIS_BEGIN);
        Enable_Handle_Idle_Delivery(_EVENT_LOCK_MAGNETIS_BEGIN, 20000);
        
        StateDebugDelivery = 0;
        
        //Xac nhan app Elevator busy (thuc hien cho cac nhiem vu khac)
        sStatusApp.Delivery      = _APP_BUSY;
    }
}

void IRQ_Pluse_OffMotorPush(uint8_t Pos)
{
    uint8_t Pos_Push = 0;
    Pos_Push = Pos - 1;
    if(sMotorPush.State == MOTOR_PUSH_ON)
    {
        if(HAL_GetTick() - sMotorPush.TimeOnMotor < 1000)
        {
           DCU_Respond(_AT_REQUEST_SERIAL, (uint8_t*)"Push_Early\r\n", 12, 0); 
        }
        else 
        {
            sMotorPush.aPulse[Pos_Push]++;
            Off_Motor_Push();
        }
    }
}

/*
	@brief  ON motor tai vi tri Pos
	@param  Pos vi tri motor muon chay
*/
void On_Motor_Push(uint8_t Pos)
{
    if(sMotorPush.State == MOTOR_PUSH_OFF)
    {
        sMotorPush.State = MOTOR_PUSH_ON;
        sMotorPush.TimeOnMotor = HAL_GetTick();
        
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
    if(sMotorPush.State == MOTOR_PUSH_ON)
    {
        sMotorPush.State = MOTOR_PUSH_OFF;
        
        uint8_t i = 0;
        for(i = 0; i < NUMBER_MOTOR ; i++)
            HAL_GPIO_WritePin(CTRL_MOTOR_PORT[i], CTRL_MOTOR_PIN[i], GPIO_PIN_RESET); 
        
        HAL_GPIO_WritePin(ON_OFF_V2_GPIO_Port, ON_OFF_V2_Pin, GPIO_PIN_RESET); 
        
        sMotorPush.TimeOnMotor = HAL_GetTick() - sMotorPush.TimeOnMotor;
        
        uint8_t aData[10]={0};
        uint8_t length = 0;
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"TimePush: ", sizeof("TimePush: ")-1);
        length = Convert_Int_To_String(aData, sMotorPush.TimeOnMotor);
        UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
        
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
    }
}

void Respond_Delivery_PcBox(eStateRespondPcBox StateRes)
{
    sRespPcBox.Length_u16 = 0;
    uint8_t countHandle = 0;
    switch(StateRes)
    {
        case RESPOND_PUSH_MOTOR:
            for(uint8_t i = 0; i<NUMBER_MOTOR; i++)
            {
                if(sParamDelivery.aDataPush[i] != 0)
                    countHandle++;
            }
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_ON_GOING_PUSH;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x02;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamDelivery.PosHandle;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamDelivery.aDataPush[sParamDelivery.PosHandle-1];
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamDelivery.SumHandle - countHandle;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamDelivery.SumHandle; 
          break;
          
        case RESPOND_COMPLETE_DELIVERY:
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_COMPLETE_PUSH;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x02;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamDelivery.Weighing >> 8;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamDelivery.Weighing;
          break;
          
        case RESPOND_FIX_MOTOR:
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_PC_BOX_FIX_MOTOR;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sParamDelivery.PosHandle;
          break;
          
        case RESPOND_WARNING_DELIVERY:
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_WARNING_DELIVERY;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
            sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
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

uint8_t Check_Top_Bot_Elevator(void)
{
    uint8_t state_top = 0;
    uint8_t state_bot = 0;
    
    if(HAL_GPIO_ReadPin(FB_Top_Elevator_GPIO_Port, FB_Top_Elevator_Pin) == GPIO_PIN_SET)
      state_top = 1;
    
    if(HAL_GPIO_ReadPin(FB_Bot_Elevator_GPIO_Port, FB_Bot_Elevator_Pin) == GPIO_PIN_SET)
      state_bot = 1;
    
    if(state_top == 0 && state_bot == 0)
      return _ELEVATOR_FLOOR;
    
    if(state_top == 1 && state_bot == 0)
    {
      if(sElevator.State == ELEVATOR_UP)
        Control_Elevator(ELEVATOR_STOP, LEVEL_PWM_ELEVATOR_OFF);
      
      sElevator.FloorCurrent = NUMBER_MAX_FLOOR;
      return _ELEVATOR_TOP;
    }
    
    if(state_top == 0 && state_bot == 1)
    {
      if(sElevator.State == ELEVATOR_DOWN)
        Control_Elevator(ELEVATOR_STOP, LEVEL_PWM_ELEVATOR_OFF);
      
      sElevator.FloorCurrent = 0;
      return _ELEVATOR_BOT;
    }
    
    Control_Elevator(ELEVATOR_STOP, LEVEL_PWM_ELEVATOR_OFF);
    return _ELEVATOR_ERROR;
}

void Control_Elevator(uint8_t Direction, uint8_t Level)
{
    switch(Direction)
    {
        case ELEVATOR_STOP:
            if(sElevator.State != ELEVATOR_STOP)
            {
                __HAL_TIM_SetCompare (&htim3, TIM_CHANNEL_2, 0);
                HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
              
                __HAL_TIM_SetCompare (&htim4, TIM_CHANNEL_4, 0);
                HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
                
                sElevator.Status = ELEVATOR_OFF;
                
//                Debug_Encoder();
                sElevator.Encoder_C1 = 0;
                sElevator.Encoder_C2 = 0;
                sElevator.RunLevel_PWM = 0;
                
                GPIO_Init_Pos_Elevator(ELEVATOR_STOP);
            }
          break;
          
        case ELEVATOR_UP:
            if(sElevator.State != ELEVATOR_UP)
            {
                __HAL_TIM_SetCompare (&htim4, TIM_CHANNEL_4, 0);
                HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
                fevent_enable(sEventAppDelivery, _EVENT_CTRL_ELEVATOR_UPPER);
                
//                Debug_Encoder();
                sElevator.Encoder_C1 = 0;
                sElevator.Encoder_C2 = 0;
                sElevator.RunLevel_PWM = LEVEL_PWM_ELEVATOR_UP;
                
                GPIO_Init_Pos_Elevator(ELEVATOR_UP);
                sElevator.TimeOnMotor = HAL_GetTick();
            }
          break;
          
        case ELEVATOR_DOWN:
            if(sElevator.State != ELEVATOR_DOWN)
            {
                __HAL_TIM_SetCompare (&htim3, TIM_CHANNEL_2, 0);
                HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
                fevent_enable(sEventAppDelivery, _EVENT_CTRL_ELEVATOR_LOWER);
                
//                Debug_Encoder();
                sElevator.Encoder_C1 = 0;
                sElevator.Encoder_C2 = 0;
                sElevator.RunLevel_PWM = LEVEL_PWM_ELEVATOR_DOWN;
                
                GPIO_Init_Pos_Elevator(ELEVATOR_DOWN);
                sElevator.TimeOnMotor = HAL_GetTick();
            }
          break;
          
        default:
          break;
    }

    sElevator.State = Direction;
}

void GPIO_Init_Pos_Elevator(uint8_t Kind)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  switch(Kind)
  {
      case ELEVATOR_STOP:
        break;
        
      case ELEVATOR_UP:
        GPIO_InitStruct.Pin = FB_Pos_Elevator_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(FB_Pos_Elevator_GPIO_Port, &GPIO_InitStruct);
        break;
          
      case ELEVATOR_DOWN:
        GPIO_InitStruct.Pin = FB_Pos_Elevator_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(FB_Pos_Elevator_GPIO_Port, &GPIO_InitStruct);
        break;
        
      default:
        break;
  }
}

void Enable_Handle_Idle_Delivery(uint8_t State, uint32_t timeout)
{
    sEventAppDelivery[_EVENT_HANDLE_ERROR_DELIVERY].e_period = timeout;
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
    sElevator.FloorHandle = 5;
    sElevator.FloorCurrent = 0;
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

void Debug_Encoder(void)
{
    uint8_t aData[10]={0};
    uint8_t length = 0;
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"Encoder_C1: ", sizeof("Encoder_C1: ")-1);
    length = Convert_Int_To_String(aData, sElevator.Encoder_C1);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"Encoder_C2: ", sizeof("Encoder_C2: ")-1);
    length = Convert_Int_To_String(aData, sElevator.Encoder_C2);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
}

void Delivery_Handle_State(uint8_t State)
{
    static uint8_t State_Before = DELIVERY_FREE;
    
    if(State != State_Before)
    {
        State_Before = State;
        switch(State)
        {
            case DELIVERY_FREE:
              Reset_StateDelivery();
              
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
              
            case DELIVERY_FIX_MOTOR:
              sParamDelivery.StateHanlde = DELIVERY_FIX_MOTOR;
              UTIL_Printf(DBLEVEL_M, (uint8_t*)"Hanlde: DELIVERY_FIX_MOTOR", sizeof("Hanlde: DELIVERY_FIX_MOTOR")-1);
              break;
              
            default:
              break;
        }
        
        if(State == DELIVERY_FREE)
            fevent_enable(sEventAppDelivery, _EVENT_WAIT_FREE_MOTOR);
        else
            fevent_disable(sEventAppDelivery, _EVENT_WAIT_FREE_MOTOR);
      
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
    }
}

void Reset_StateDelivery(void)
{
    UTIL_MEM_set(sMotorPush.aPulse, 0x00, NUMBER_MOTOR);
    UTIL_MEM_set(sParamDelivery.aDataPush, 0x00, NUMBER_MOTOR);
    sParamDelivery.SumHandle = 0;
    sParamDelivery.PosHandle = 0;
    sElevator.FloorHandle = 0;
}

/*======================== Function Handle ========================*/
void Init_AppDelivery(void)
{

}

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



