

#ifndef USER_UART_H_
#define USER_UART_H_

#include "user_define.h"
#include "user_util.h"
#include "usart.h"

#define uart_pcbox     huart6
#define uart_debug     huart3 
#define uart_485       huart4


/*================= Extern =================*/
extern uint8_t UartDebugBuff[1200];
extern sData   sUartDebug;

extern uint8_t UartPcBoxBuff[100];
extern sData   sUartPcBox;

extern uint8_t Uart485Buff[100];
extern sData   sUart485;

/*================== Function ==================*/
void Init_Uart_PcBox_IT(void);
void Init_Uart_Debug_IT(void);
void Init_Uart_485_IT(void);
void Init_Uart_Module(void);


#endif
