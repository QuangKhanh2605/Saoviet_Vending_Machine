

#ifndef USER_APP_PC_BOX_H_
#define USER_APP_PC_BOX_H_

#define USING_APP_PC_BOX

#include "event_driven.h"
#include "user_util.h"
#include "user_uart.h"
#include "user_comm_vending_machine.h"



typedef enum
{
    _EVENT_PC_BOX_ENTRY = 0,
    _EVENT_PC_BOX_RECEIVE_HANDLE,
    _EVENT_PC_BOX_COMPLETE_RECEIVE,
    _EVENT_PC_BOX_LOG_TSVH,
    _EVENT_PC_BOX_SET_DCU_ID,
    _EVENT_PC_BOX_GET_DCU_ID,
    _EVENT_WDG_STM32F4,
    _EVENT_RESET_DCU,
    
    _EVENT_QUEUE_RESPOND_IMMEDIATELY,
    _EVENT_QUEUE_RESPOND_TIME,
    
    _EVENT_DCU_PING_PC_BOX,
    _EVENT_RESET_PC_BOX,
    _EVENT_WAIT_RESET_PC_BOX,
    
    _EVENT_REFRESH_DCU,

    _EVENT_PC_BOX_END,
}eKindEventPcBox;

typedef enum
{
    FREE = 0,
    BUSY,
}eStatusApp;

typedef enum
{
    _RECV_PCBOX=0,
    _TRANS_PCBOX,
}eKindTranRecvPcBox;

typedef enum
{
    _DISCONNECT_PCBOX = 0,
    _CONNECT_PCBOX,
}eKindStatusPcBox;

typedef enum
{
    _UNUSING_CRC,
    _USING_CRC,
}eKindStatusCrcPcBox;

typedef struct
{
    uint8_t Motor;
    uint8_t Door;
    uint8_t Temperature;
    uint8_t RL_Warm;
    uint8_t Pcbox;
}StructStatusApp;

typedef struct
{
    uint8_t CountResetPcBox;    //Dem so lan mat ket noi reset PcBox
    uint8_t TimeResetPcBox;     //Thoi gian de PcBox Reset (Phut)
    uint8_t TimeTSVH;           //Thoi gian gui thong so van hanh (Phut)
    uint8_t ConnectPcBox;
    uint8_t UsingCrc;
}StructParamPcBox;

typedef struct 
{
    uint8_t aData_u8[NUMBER_MAX_BUFFER];
    uint8_t Length;
}sDataQueueRespondPcBox;

extern sEvent_struct    sEventAppPcBox[];
extern sData            sDCU_ID;
extern StructStatusApp  sStatusApp;
extern StructParamPcBox sParamPcBox;
extern sData            sRespPcBox;
/*================ Function ===================*/

uint8_t     AppPcBox_Task(void);
void        Init_DCU_ID(void);
void        Pc_Box_Init(void);
void        Log_TSVH(void);
void        AppPcBox_Debug(uint8_t aData[], uint8_t length, uint8_t TransRecv);
void        Write_Queue_Repond_PcBox(uint8_t aData[], uint8_t Length);
void        Transmit_PCBOX(uint8_t aData[],uint8_t length);

void        Set_TimeTSVH(uint8_t Time);
void        Set_TimeResetPcBox(uint8_t Time);
void        Init_PcBox(void);

void        Write_Flash_Using_Crc(void);
void        Init_Using_Crc();

void        Packing_Respond_PcBox(uint8_t aData[], uint16_t Length);
void        ResetDCU(void);
#endif
