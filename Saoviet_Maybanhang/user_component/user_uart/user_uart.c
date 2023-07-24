#include "user_uart.h"

/*==================var struct======================*/
uint8_t UartDebugBuff[1200] = {0};
sData 	sUartDebug = {&UartDebugBuff[0], 0};

uint8_t UartPcBoxBuff[100] = {0};
sData  sUartPcBox = {&UartPcBoxBuff[0], 0};

uint8_t Uart485Buff[100] = {0};
sData  sUart485 = {&Uart485Buff[0] , 0};

uint8_t UartPcBoxRxByte = 0;
uint8_t UartDebugRxByte = 0;
uint8_t Uart485RxByte   = 0;

/*==================Function======================*/

void Init_Uart_PcBox_IT(void)
{
    HAL_UART_Receive_IT(&uart_pcbox, &UartPcBoxRxByte, 1);
}

void Init_Uart_Debug_IT(void)
{
    HAL_UART_Receive_IT(&uart_debug, &UartDebugRxByte, 1);
}

void Init_Uart_485_IT(void)
{
    HAL_UART_Receive_IT(&uart_485, &Uart485RxByte,1);
}

void Init_Uart_Module(void)
{
    Init_Uart_PcBox_IT();
    Init_Uart_Debug_IT();
    Init_Uart_485_IT();
}
/*================ Function Callback ==============*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == uart_pcbox.Instance)
	{
        *(sUartPcBox.Data_a8 + sUartPcBox.Length_u16++) = UartPcBoxRxByte;

        if (sUartPcBox.Length_u16 >= (sizeof(UartPcBoxBuff) - 1))
            sUartPcBox.Length_u16 = 0;

        HAL_UART_Receive_IT(&uart_pcbox, &UartPcBoxRxByte, 1);
        
        //fevent_enable(sEventSim, _EVENT_SIM_UART_RECEIVE);   
    }
    
    if (huart->Instance == uart_pcbox.Instance)
	{
        *(sUartDebug.Data_a8 + sUartDebug.Length_u16++) = UartDebugRxByte;

        if (sUartDebug.Length_u16 >= (sizeof(UartDebugBuff) - 1))
            sUartDebug.Length_u16 = 0;

        HAL_UART_Receive_IT(&uart_pcbox, &UartDebugRxByte, 1);
        
        //fevent_enable(sEventSim, _EVENT_SIM_UART_RECEIVE);   
    }
    
    if (huart->Instance == uart_485.Instance)
	{
        *(sUart485.Data_a8 + sUart485.Length_u16++) = Uart485RxByte;

        if (sUart485.Length_u16 >= (sizeof(Uart485Buff) - 1))
            sUart485.Length_u16 = 0;

        HAL_UART_Receive_IT(&uart_485, &Uart485RxByte, 1);
        
        //fevent_enable(sEventSim, _EVENT_SIM_UART_RECEIVE);   
    }
}