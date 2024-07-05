
#include "user_app_comm.h"
#include "user_app_slave.h"
#include "user_app_ade7953.h"

/*=========================== Func App Main ========================*/

void SysApp_Init (void)
{
    Init_AppSlave();
}

void SysApp_Setting (void)
{
    Init_Energy();
    AppComm_Init();
    Init_Ade7953();
}

/*
    Func: Init Queue trong main
*/
uint8_t TaskStatus_u8 = 0;
void Main_Task (void)
{
  SysApp_Init();
  SysApp_Setting();
  HAL_Delay(100);
  for(;;)
  {
    #ifdef  USING_APP_SLAVE
        AppSlave_Task();
    #endif
        
    #ifdef  USING_APP_ADE7953
        AppAde7953_Task();
    #endif    
          
    IWDG_ResetDCU();
  }
}


/*============= Function App Main ===============*/

void AppComm_Init (void)
{

}

void IWDG_ResetDCU(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}




