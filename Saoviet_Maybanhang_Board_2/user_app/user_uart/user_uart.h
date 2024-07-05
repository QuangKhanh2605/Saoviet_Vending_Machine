
#ifndef USER_UART_H_
#define USER_UART_H_

#include "user_util.h"
#include "usart.h"

/*======Define Uart ==============*/
//#define uart_debug	    huart3
//#define uart_sim	    huart2
//#define uart_pcbox      huart6
#define uart_485        huart2
/*===========Var struct=============*/
extern uint8_t UartDebugBuff[100];
extern sData sUartDebug;

extern uint8_t Uart485Buff[50];
extern sData sUart485;

extern uint8_t UartPcBoxBuff[100];
extern sData sUartPcBox;

/*==================Function==================*/
void Init_Uart_Module (void);

void Init_Uart_Sim_Rx_IT (void);
void Init_Uart_Debug_Rx_IT (void);
void Init_Uart_485_Rx_IT (void);
void Init_Uart_PcBox_Rx_IT (void);
void DeInit_Uart_Sim_Rx_IT (void);


#endif /* USER_UART_H_ */
