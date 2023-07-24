#include "user_app_comm.h"


/*=========================== Func App Main ========================*/
/*
    Func: Init Queue trong main
*/
uint8_t TaskStatus_u8 = 0;
void Main_Task (void)
{
  for(;;)
  {
    TaskStatus_u8 |= AppMotor_Task();
  }
}
