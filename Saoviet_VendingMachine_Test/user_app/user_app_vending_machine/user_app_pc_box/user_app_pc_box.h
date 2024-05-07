

#ifndef USER_APP_PC_BOX_H_
#define USER_APP_PC_BOX_H_

#define USING_APP_PC_BOX

#include "event_driven.h"
#include "user_util.h"
#include "user_uart.h"
#include "user_comm_vending_machine.h"
#include "queue_p.h"

#define NUMBER_ITEM_QUEUE           30
#define NUMBER_MAX_BUFFER           40

#define TIME_SEND_TSVH              60000
#define TIME_RESET_WDG              1000


#define TIME_RESPOND_PC_BOX         2000  

#define TIME_PING_PCBOX             2000

#define TIME_REFRESH_DCU            2*86400*TIME_ONE_SECOND

#define BEFORE_RESET_DCU            0
#define AFTER_RESET_DCU             1

#define NUMBER_MAX_PING_PCBOX       10

#define DEFAULT_OFF_PCBOX_HOURS     21
#define DEFAULT_OFF_PCBOX_MINUTES   0
#define DEFAULT_ON_PCBOX_HOURS      6
#define DEFAULT_ON_PCBOX_MINUTES    0

#define MINUTES_OF_HOURS            60
#define MINUTES_OF_DAY              1440

typedef enum
{
    _EVENT_PC_BOX_ENTRY = 0,
    _EVENT_PC_BOX_RECEIVE_HANDLE,
    _EVENT_PC_BOX_COMPLETE_RECEIVE,
    _EVENT_PC_BOX_LOG_TSVH,
    _EVENT_WDG_STM32F4,
    
    _EVENT_QUEUE_RESPOND_IMMEDIATELY,
    _EVENT_QUEUE_RESPOND_TIME,
    
    _EVENT_ON_OFF_CYCLE_PC_BOX,
    _EVENT_DCU_PING_PC_BOX,
    _EVENT_OFF_PC_BOX,
    _EVENT_ON_PC_BOX,
    
    _EVENT_REFRESH_DCU,
    _EVENT_GET_REAL_TIME_DCU,

    _EVENT_PC_BOX_END,
}eKindEventPcBox;

typedef enum
{
    _APP_FREE = 0,
    _APP_BUSY,
}eStatusApp;

typedef enum
{
    _RECV_PCBOX=0,
    _TRANS_PCBOX,
}eKindTranRecvPcBox;

typedef enum
{
    _STT_PCBOX_DISCONNECT = 0,
    _STT_PCBOX_CONNECT,
    _STT_PCBOX_SLEEP,
    _STT_PCBOX_RESET,
    _STT_PCBOX_END,
}eKindStatusPcBox;

typedef enum
{
    _PCBOX_OFF,
    _PCBOX_ON,
}eKindStatePcBox;

typedef enum
{
    _UNUSING_CRC,
    _USING_CRC,
}eKindStatusCrcPcBox;

typedef struct
{
    uint8_t Delivery;
    uint8_t Door;
    uint8_t Temperature;
    uint8_t RL_Warm;
    uint8_t Pcbox;
}StructStatusApp;


typedef struct
{
    uint8_t CountResetPcBox;    //Dem so lan mat ket noi reset PcBox
    uint8_t TimeTSVH;           //Thoi gian gui thong so van hanh (Phut)
    uint8_t StatePcBox;
    uint8_t UsingCrc;
}StructParamPcBox;

typedef enum
{
    _OFF_CYCLE,
    _ON_CYCLE,
}eKindOnOffCyclePcBox;

typedef struct
{
    uint8_t State;
    uint8_t HoursOFF;
    uint8_t MinutesOFF;
    uint8_t HoursON;
    uint8_t MinutesON;
}StructOnOffCyclePcBox;


typedef struct 
{
    uint8_t aData_u8[NUMBER_MAX_BUFFER];
    uint8_t Length;
}sDataQueueRespondPcBox;

extern sEvent_struct            sEventAppPcBox[];
extern StructStatusApp          sStatusApp;
extern StructParamPcBox         sParamPcBox;
extern sData                    sRespPcBox;
extern StructOnOffCyclePcBox    sCycleOnOffPcBox;
/*================ Function ===================*/
uint8_t     AppPcBox_Task(void);
void        Init_AppPcBox(void);

uint8_t     Save_DcuID(uint8_t *aData, uint16_t Length);

void        Init_QueuePcBox(void);
void        Log_TSVH(void);
void        AppPcBox_Debug(uint8_t aData[], uint8_t length, uint8_t TransRecv);
void        Write_Queue_Repond_PcBox(Struct_Queue_Type *qQueueSend, uint8_t aData[], uint8_t Length, uint8_t KindSend);
void        Transmit_PCBOX(uint8_t aData[],uint8_t length);

uint8_t     Set_TimeTSVH(uint8_t Time);
void        Init_TimePcBox(void);

uint8_t     Save_UsingCrc(uint8_t UsingCrc);
void        Init_UsingCrc();

uint8_t     Save_StatePcBox(uint8_t State);
void        Init_StatePcBox();

uint8_t     Save_CycleOnOffPcBox(uint8_t State, uint8_t HoursOFF, uint8_t MinutesOFF, uint8_t HoursON, uint8_t MinutesON);
void        Init_CycleOnOffPcBox(void);

void        RespondPcBox_String(uint8_t *aData, uint16_t Length);
void        Packing_Respond_PcBox(uint8_t aData[], uint16_t Length);
void        ResetDCU(void);

void        Cb_Timer_Event_OnOffPcBox(void *context);
void        OnOffPcBox(uint16_t time);
uint8_t     Check_RealTime_OFF_PcBox(uint8_t Hours, uint8_t Minutes);
uint8_t     Check_RealTime_ON_PcBox(uint8_t Hours, uint8_t Minutes);
#endif
