#include "user_app_comm.h"
#include "user_inc_vending_machine.h"
#include "user_app_at_debug.h"

/*=========================== Func App Main ========================*/

void SysApp_Init (void)
{
    Init_AppRelay();
    Pc_Box_Init();
    Init_AppElectric();
    Init_AppTemperature();
}

void SysApp_Setting (void)
{
    Init_DCU_ID();
    Init_PcBox();
    Init_Using_Crc();
    AppComm_Init();
}

/*
    Func: Init Queue trong main
*/
uint8_t TaskStatus_u8 = 0;
void Main_Task (void)
{
  SysApp_Init();
  SysApp_Setting();
  HAL_Delay(50);
  for(;;)
  {
    #ifdef USING_APP_AT_DEBUG
        TaskStatus_u8 |= AppAtDebug_Task();
    #endif
    
    #ifdef USING_APP_PC_BOX
        TaskStatus_u8 |= AppPcBox_Task();
    #endif
        
    #ifdef USING_APP_CTRL_MOTOR 
        TaskStatus_u8 |= AppMotor_Task();
    #endif
        
    #ifdef USING_APP_DOOR_SENSOR
        TaskStatus_u8 |= AppDoorSensor_Task();
    #endif
        
    #ifdef USING_APP_ELECTRIC
        TaskStatus_u8 |= AppElectric_Task();
    #endif
        
    #ifdef USING_APP_RELAY
        TaskStatus_u8 |= AppRelay_Task();
    #endif
        
    #ifdef USING_APP_TEMPERATURE
        TaskStatus_u8 |= AppTemperature_Task();
    #endif
        
    #ifdef USING_APP_VIB_SENSOR
        TaskStatus_u8 |= AppVibSensor_Task();
    #endif
  }
}


/*============= Function App Main ===============*/

void AppComm_Init (void)
{
    Init_Uart_Module(); 
}




