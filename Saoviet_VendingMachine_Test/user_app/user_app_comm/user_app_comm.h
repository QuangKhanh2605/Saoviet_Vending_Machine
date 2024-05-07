/*
 * user_app.h
 *
 *  Created on: Dec 14, 2021
 *      Author: Chien
 */

#ifndef INC_USER_APP_COMM_H_
#define INC_USER_APP_COMM_H_


#include "user_util.h"

#include "event_driven.h"

typedef enum
{
	_EVENT_TIMMER_IRQ = 0,       //0
	_EVENT_UART_RX_DEBUG,        //1
    _EVENT_UART_ERROR,
    _EVENT_SET_RTC,              //2
	_EVENT_IDLE,                 //3
    _EVENT_TX_TIMER,             //4
    
    _EVENT_SAVE_BOX,             //6
    
	_EVENT_END_COMM,             //7
}eKindEventAppCommon;


typedef void (*pCb_Packet_Data) (uint8_t *pData, uint16_t *Length);
typedef struct
{
    uint8_t             Obis_u8;        //Obis
    uint8_t             Status_u8;
    
    uint8_t             Scale_u8;
    uint8_t             *pScale;
    
    uint8_t             LenData_u8;
    uint8_t             *pLenData;
    
    void                *pData;
}sObisListCollect;



extern sData   sFirmVersion;
extern sEvent_struct sEventAppComm[];


/*=============Function=======================*/
void        SysApp_Init (void);
void        SysApp_Setting (void);
void        Main_Task (void);

uint8_t     AppComm_Task (void);
void        AppComm_Init (void);

void        AppComm_IRQ_Timer_CallBack (void);
uint32_t    AppComm_Get_Random_Follow_ID (void);

void        AppComm_Set_Next_TxTimer (void);
void        AppComm_SV_Protocol_Packet_Data (uint8_t *pTarget, uint16_t *LenTarget, uint8_t Obis, 
                                                void *pData, uint8_t LenData, uint8_t Scale);
void        AppComm_Sub_Packet_Integer (uint8_t *pTarget, uint16_t *LenTarget, void *Data, uint8_t LenData);



#endif /* INC_USER_APP_H_ */
