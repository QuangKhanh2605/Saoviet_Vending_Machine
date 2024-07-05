#ifndef USER_APP_COMM_H__
#define USER_APP_COMM_H__

#include "stm32l0xx_hal.h"
#include "stm32l0xx.h"
#include "iwdg.h"

/*=============Function=======================*/
void        Main_Task(void);

void        SysApp_Init (void);
void        SysApp_Setting (void);
void        AppComm_Init (void);
void        IWDG_ResetDCU (void);


#endif

