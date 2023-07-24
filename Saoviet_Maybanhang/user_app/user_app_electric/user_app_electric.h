

#ifndef USER_APP_ELECTRIC_H_
#define USER_APP_ELECTRIC_H_

#define USING_APP_ELECTRIC

#include "event_driver.h"
#include "user_util.h"
#include "user_uart.h"
#include "main.h"

typedef enum
{
    _EVENT_ELECTRIC_PGOOD,
    _EVENT_ELECTRIC_2,
    
    _EVENT_ELECTRIC_END,
}eKindEventElectric;

extern sEvent_struct       sEventAppElectric[];

/*============= Function handle ==============*/

uint8_t     AppElectric_Task(void);
void        Send_Command_IVT (uint8_t SlaveID, uint8_t Func_Code, uint16_t Addr_Register, uint16_t Infor_Register, void (*pFuncResetRecvData) (void));
void        AppIVT_Clear_Before_Recv (void);
#endif

