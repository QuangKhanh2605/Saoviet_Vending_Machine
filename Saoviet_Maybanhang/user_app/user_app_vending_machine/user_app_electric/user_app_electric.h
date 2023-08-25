

#ifndef USER_APP_ELECTRIC_H_
#define USER_APP_ELECTRIC_H_

#define USING_APP_ELECTRIC

#include "event_driven.h"
#include "user_util.h"
#include "user_uart.h"

typedef enum
{
    _EVENT_ELECTRIC_ENTRY,
    _EVENT_ELECTRIC_PGOOD,
    _EVENT_ELECTRIC_TRANSMIT_485,
    _EVENT_ELECTRIC_RECEIVE_485,
    _EVENT_ELECTRIC_HANDLE_485,
    _EVENT_ELECTRIC_OFF_POWER,
    
    _EVENT_ELECTRIC_END,
}eKindEventElectric;

typedef enum
{
    POWER_OFF = 0,
    POWER_ON,
}eStatusPower;

typedef struct 
{
    uint16_t Voltage;
    uint16_t Current;
    uint8_t  Scale;
}Struct_Electric_Current;

extern sEvent_struct                sEventAppElectric[];
extern Struct_Electric_Current      sElectric;
extern uint8_t                      Status_Supply_Power;
/*============= Function handle ==============*/

uint8_t     AppElectric_Task(void);
void        Send_Command_IVT (uint8_t SlaveID, uint8_t Func_Code, uint16_t Addr_Register, uint16_t Infor_Register, void (*pFuncResetRecvData) (void));
void        AppIVT_Clear_Before_Recv (void);
uint8_t     Status_Power_Respond_PcBox(uint8_t aData[]);
#endif

