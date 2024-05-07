
/*
 * myUart.h
 *
 *  Created on: Dec 7, 2021
 *      Author: lenovo
 */

#ifndef USER_UART_H_
#define USER_UART_H_

#include "user_util.h"
#include "usart.h"

/*======Define Uart ==============*/
/*-- Rx uart: DMA with IDLE line ----*/
#define UART_RX_DMA      0
/*-- Rx uart: Interupt line ----*/
#define UART_RX_IT       1

#define UART_RX_MODE     UART_RX_IT

#define uart_debug	    huart3
#define uart_485        huart1
#define uart_pcbox      huart2

#define uart_dma_debug	hdma_usart3_rx
#define uart_dma_485	hdma_usart1_rx
#define uart_dme_pcbox  hdma_usart2_rx

/*===========Var struct=============*/

typedef enum
{
    _UART_DEBUG = 0,
    _UART_485,
    _UART_PCBOX,
}eTypeUart;

extern uint8_t UartDebugBuff[1200];
extern sData sUartDebug;

extern uint8_t Uart485Buff[100];
extern sData sUart485;

extern uint8_t UartPcBoxBuff[100];
extern sData   sUartPcBox;

extern volatile uint8_t IsUart1IDLEInterupt;
extern volatile uint8_t IsUart2IDLEInterupt;
extern volatile uint8_t IsUart3IDLEInterupt;


/*==================Function==================*/
void Init_Uart_Module (void);

void Init_RX_Mode_Uart_Debug (void);
void Init_RX_Mode_Uart_485 (void);
void Init_RX_Mode_Uart_PcBox (void);

void DeInit_RX_Mode_Uart_Debug (void);
void DeInit_RX_Mode_Uart_485 (void);
void DeInit_RX_Mode_Uart_PcBox (void);

void UART_Get_Data_Debug (uint8_t ModeUartDma, sData *sTaget, uint16_t MAX_SIZE,
                          uint8_t *pData, uint16_t *OldPos, uint16_t NewPos, uint16_t EndPos) ;

void ReInit_Rx_Mode_Uart_Error (void);

#endif /* USER_UART_H_ */
