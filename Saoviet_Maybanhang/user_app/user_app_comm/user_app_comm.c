#include "user_app_comm.h"
#include "user_inc_vending_machine.h"

/*=========================== Func App Main ========================*/

void SysApp_Init (void)
{

}

void SysApp_Setting (void)
{
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
  HAL_Delay(500);
  for(;;)
  {
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
    ADC_Init();
}




