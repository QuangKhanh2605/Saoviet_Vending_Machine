

#ifndef USER_UART_H_
#define USER_UART_H_

#include "user_define.h"
#include "user_util.h"
#include "usart.h"

#define uart_pcbox     huart6
#define uart_debug     huart3 


/*================= Extern =================*/
extern uint8_t UartDebugBuff[1200];
extern sData 	sUartDebug;

extern uint8_t UartPcBoxBuff[100];
extern sData  sUartPcBox;


/*================== Function ==================*/
void Init_Uart_PcBox_IT(void);
void Init_Uart_PcBox_IT(void);
void Init_Uart_Module(void);


#endif
