
#ifndef USER_APP_RS485_H__
#define USER_APP_RS485_H__

#define USING_APP_RS485

#include "user_util.h"
#include "event_driven.h"

#define ID_DEFAULT_ELECTRIC          26
#define ID_DEFAULT_WEIGHING          26

typedef enum
{
    _EVENT_RS485_ENTRY,
    _EVENT_RS485_TRANSMIT,
    _EVENT_RS485_RECEIVE_HANDLE,
    _EVENT_RS485_RECEIVE_COMPLETE,
    
    _EVENT_RS485_INIT_UART,
    _EVENT_RS485_END,
}eKindEventRs485;

typedef enum
{
    _RS485_OPERA_ELECTRIC,
    _RS485_OPERA_WEIGHING,
}eKindMode485;

typedef enum 
{
    DISCONNECT_SLAVE,
    CONNECT_SLAVE,
}eNumConnectSlave;

typedef struct
{
    uint8_t Electric;
    uint8_t Weighing;
}Struct_IdSlave;

typedef struct
{
    uint8_t Electric;
}Struct_State_Slave;

typedef struct
{
    uint8_t Trans;
    uint8_t Recv;
}Struct_KindMode485;

extern sEvent_struct        sEventAppRs485[];
extern Struct_IdSlave       sIdSlave485;
extern Struct_KindMode485   sKindMode485;
extern Struct_State_Slave   sStateSlave485;
/*====================Function Handle====================*/

uint8_t    AppRs485_Task(void);
void       Init_AppRs485(void);
uint8_t    Save_IdSlave(uint8_t ID_Electric);
void       Init_IdSlave(void);
void       Average_MeasureOxy(uint16_t Oxy_Mg_L, uint16_t Oxy_Percent);
#endif

