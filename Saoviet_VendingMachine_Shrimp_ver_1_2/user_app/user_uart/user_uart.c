
/*
 * myUart.c
 *
 *  Created on: Dec 7, 2021
 *      Author: lenovo
 */

#include "user_uart.h"
#include "user_define.h"

/*==================var struct======================*/

#define RxBuf_SIZE      100

uint8_t UartDebugBuff[1200] = {0};
sData 	sUartDebug = {&UartDebugBuff[0], 0};

uint8_t UartPcBoxBuff[100] = {0};
sData   sUartPcBox = {&UartPcBoxBuff[0] , 0};

uint8_t Uart485Buff[100] = {0};
sData   sUart485 = {&Uart485Buff[0], 0};

uint8_t UartDebugRxByte = 0;
uint8_t Uart485RxByte = 0;
uint8_t UartPcBoxRxByte = 0;

//extern DMA_HandleTypeDef hdma_usart1_rx;
//extern DMA_HandleTypeDef hdma_usart2_rx;
//extern DMA_HandleTypeDef hdma_usart3_rx;

extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart3_rx;
//extern DMA_HandleTypeDef hdma_usart6_rx;

//static uint8_t aUART_RX_DMA_1 [RxBuf_SIZE];
//static uint8_t aUART_RX_DMA_2 [RxBuf_SIZE];
//static uint8_t aUART_RX_DMA_3 [RxBuf_SIZE];

     
volatile uint8_t IsUart1IDLEInterupt = false;
volatile uint8_t IsUart2IDLEInterupt = false;
volatile uint8_t IsUart3IDLEInterupt = false;


volatile static uint8_t IndexUartError  = 0;

//static uint16_t OldPosDMADebug = 0;
//static uint16_t OldPosDMASim = 0;
//static uint16_t OldPosDMA485 = 0;

/*==================Function======================*/

void Init_RX_Mode_Uart_Debug (void)
{
#if (UART_RX_MODE == UART_RX_IT)
//    __HAL_UART_ENABLE_IT(&uart_debug, UART_IT_RXNE);
  
    HAL_UART_Receive_IT(&uart_debug, &UartDebugRxByte, 1);
#else
    HAL_UARTEx_ReceiveToIdle_DMA( &uart_debug, aUART_RX_DMA_3, sizeof(aUART_RX_DMA_3) );
    __HAL_DMA_DISABLE_IT(&uart_dma_debug, DMA_IT_HT);
    OldPosDMADebug = 0;
#endif
}


void Init_RX_Mode_Uart_485 (void)
{
#if (UART_RX_MODE == UART_RX_IT)
//    __HAL_UART_ENABLE_IT(&uart_485, UART_IT_RXNE);
    HAL_UART_Receive_IT(&uart_485, &Uart485RxByte, 1);
#else
    HAL_UARTEx_ReceiveToIdle_DMA(&uart_485, aUART_RX_DMA_1, sizeof(aUART_RX_DMA_1));
    __HAL_DMA_DISABLE_IT(&uart_dma_485, DMA_IT_HT); 
    OldPosDMA485 = 0;
#endif
}

void Init_RX_Mode_Uart_PcBox (void)
{
#if (UART_RX_MODE == UART_RX_IT)
//    __HAL_UART_ENABLE_IT(&uart_485, UART_IT_RXNE);
    HAL_UART_Receive_IT(&uart_pcbox, &UartPcBoxRxByte, 1);
#else
    HAL_UARTEx_ReceiveToIdle_DMA(&uart_pcbox, aUART_RX_DMA_1, sizeof(aUART_RX_DMA_1));
    __HAL_DMA_DISABLE_IT(&uart_dma_pcbox, DMA_IT_HT); 
    OldPosDMA485 = 0;
#endif
}

void DeInit_RX_Mode_Uart_Debug (void)
{
#if (UART_RX_MODE == UART_RX_IT)
//    __HAL_UART_DISABLE_IT(&uart_debug, UART_IT_RXNE);   
#else
    HAL_UART_DMAStop(&uart_debug);
#endif
}


void DeInit_RX_Mode_Uart_485 (void)
{
#if (UART_RX_MODE == UART_RX_IT)
//    __HAL_UART_DISABLE_IT(&uart_485, UART_IT_RXNE);   
#else
    HAL_UART_DMAStop(&uart_485);
#endif
}

void DeInit_RX_Mode_Uart_PcBox (void)
{
#if (UART_RX_MODE == UART_RX_IT)
//    __HAL_UART_DISABLE_IT(&uart_485, UART_IT_RXNE);   
#else
    HAL_UART_DMAStop(&uart_485);
#endif
}



void Init_Uart_Module (void)
{   
    Init_RX_Mode_Uart_485();
    Init_RX_Mode_Uart_Debug();
    Init_RX_Mode_Uart_PcBox();
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{ 
    
	if (huart->Instance == uart_pcbox.Instance)
	{
        *(sUartPcBox.Data_a8 + sUartPcBox.Length_u16++) = UartPcBoxRxByte;

        if (sUartPcBox.Length_u16 >= (sizeof(UartPcBoxBuff) - 1))
            sUartPcBox.Length_u16 = 0;

        HAL_UART_Receive_IT(&uart_pcbox, &UartPcBoxRxByte, 1);
        
//        fevent_active(sEventSim, _EVENT_SIM_UART_RECEIVE);        
	} 

	if (huart->Instance == uart_debug.Instance)
	{
        *(sUartDebug.Data_a8 + sUartDebug.Length_u16++) = UartDebugRxByte;

        if (sUartDebug.Length_u16 > (sizeof (UartDebugBuff) - 1))
            sUartDebug.Length_u16 = 0;

        HAL_UART_Receive_IT(&uart_debug, &UartDebugRxByte, 1);
        
        fevent_active(sEventAppComm, _EVENT_UART_RX_DEBUG);
	}
    
    if (huart->Instance == uart_485.Instance)
	{
        *(sUart485.Data_a8 + sUart485.Length_u16++) = Uart485RxByte;

        if (sUart485.Length_u16 >= (sizeof(Uart485Buff) - 1))
            sUart485.Length_u16 = 0;
        
        HAL_UART_Receive_IT(&uart_485, &Uart485RxByte, 1);
        
//        sTempHumi.ModBusStatus_u8 = TRUE;
    }
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
#if (UART_RX_MODE == UART_RX_DMA)
    if (huart->Instance == uart_debug.Instance)
    {
        UART_Get_Data_Debug(uart_dma_debug.Init.Mode, &sUartDebug, sizeof(UartDebugBuff), 
                            aUART_RX_DMA_3, &OldPosDMADebug, Size, sizeof(aUART_RX_DMA_3) );
        
        if (uart_dma_debug.Init.Mode == DMA_NORMAL)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(&uart_debug, aUART_RX_DMA_3, sizeof(aUART_RX_DMA_3));
            __HAL_DMA_DISABLE_IT(&uart_dma_debug, DMA_IT_HT);
        }
        
        fevent_active(sEventAppComm, _EVENT_UART_RX_DEBUG);
    }
    
//    if (huart->Instance == uart_sim.Instance)
//    {
//        UART_Get_Data_Debug(uart_dma_sim.Init.Mode, &sUartSim, sizeof(uartSimBuffReceive), 
//                            aUART_RX_DMA_2, &OldPosDMASim, Size, sizeof(aUART_RX_DMA_2) );
//          
//        if (uart_dma_sim.Init.Mode == DMA_NORMAL)
//        {
//            HAL_UARTEx_ReceiveToIdle_DMA(&uart_sim, aUART_RX_DMA_2, sizeof(aUART_RX_DMA_2));
//            __HAL_DMA_DISABLE_IT(&uart_dma_sim, DMA_IT_HT);
//        }
//        
//        fevent_active(sEventSim, _EVENT_SIM_UART_RECEIVE); 
//    }
    
    if (huart->Instance == uart_485.Instance)
    {
        UART_Get_Data_Debug(uart_dma_485.Init.Mode, &sUart485, sizeof(aUART_485_DATA), 
                            aUART_RX_DMA_1, &OldPosDMA485, Size, sizeof(aUART_RX_DMA_1) );
        
        if (uart_dma_485.Init.Mode == DMA_NORMAL)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(&uart_485, aUART_RX_DMA_1, sizeof(aUART_RX_DMA_1));
            __HAL_DMA_DISABLE_IT(&uart_dma_485, DMA_IT_HT);
        }

//        sTempHumi.ModBusStatus_u8 = TRUE;
    }
#endif
}


/*
    Func: Get data from buff DMA: circular (ring buffer dma)
        * Mode Circular
            + sTarget: sData mainbuff (Prcess recv string)
            + MAX_SIZE: Max size of mainbuff (avoid overflow buff)
            + pData   : data DMABuff after recev
            + OldPos: Last Position of array DMABuff
            + NewPos: New Position in DMABuff
            + EndPos: Max size DMABuff and it's Length byte regis of init DMA
        *Mode Normal:
            Get all data form DMABuff 0 -> newpos
*/
void UART_Get_Data_Debug (uint8_t ModeUartDma, sData *sTarget, uint16_t MAX_SIZE,
                          uint8_t *pData, uint16_t *OldPos, uint16_t NewPos, uint16_t EndPos )
{    
    uint16_t NbByteRec = 0, i = 0;
    uint16_t OldPosData = *OldPos;
    
    if (ModeUartDma == DMA_NORMAL)   //sua o day
    {
        if ( (sTarget->Length_u16 + NewPos) >= MAX_SIZE )
                sTarget->Length_u16 = 0;
        
        for (i = 0; i < NewPos; i++)
            *(sTarget->Data_a8 + sTarget->Length_u16 + i) = pData[i]; 

        sTarget->Length_u16 += NewPos ;
    } else
    {
        //Chia 2 truong hop
        if (NewPos > OldPosData)
        {
            NbByteRec = NewPos - OldPosData;
            
            //Kiem tra length cua buff nhan
            if ( (sTarget->Length_u16 + NbByteRec) >= MAX_SIZE )
                sTarget->Length_u16 = 0;
        
            for (i = 0; i < NbByteRec; i++)
                *(sTarget->Data_a8 + sTarget->Length_u16 + i) = pData[OldPosData + i]; 
            
            sTarget->Length_u16 += NbByteRec;
        } else
        {
            NbByteRec = EndPos - OldPosData;

            //Kiem tra length cua buff nhan
            if ( (sTarget->Length_u16 + NewPos + NbByteRec) >= MAX_SIZE )
                sTarget->Length_u16 = 0;
            
            for (i = 0; i < NbByteRec; i++)
                *(sTarget->Data_a8 + sTarget->Length_u16 + i) = pData[OldPosData + i]; 
            
            for (i = 0; i < NewPos; i++)
                *(sTarget->Data_a8 + sTarget->Length_u16 + i) = pData[i]; 
            
            sTarget->Length_u16 += (NewPos + NbByteRec) ;
        }
        
        //Cap nhat lai Old Position
        *OldPos = NewPos;
    }
}
  

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{    
    if (huart->Instance == uart_debug.Instance)
    {
        IndexUartError = _UART_DEBUG;
    }
    
    if (huart->Instance == uart_485.Instance)
    {
        IndexUartError =_UART_485;
    }
    
    if (huart->Instance == uart_pcbox.Instance)
    {
        IndexUartError =_UART_PCBOX;
    }
    
    fevent_active(sEventAppComm, _EVENT_UART_ERROR); 
}


void ReInit_Rx_Mode_Uart_Error (void)
{
    if (IndexUartError == _UART_DEBUG)
    {
        Init_RX_Mode_Uart_Debug();
    }  
    
    if (IndexUartError == _UART_485)
    {
        Init_RX_Mode_Uart_485();
    }
    
    if (IndexUartError == _UART_PCBOX)
    {
        Init_RX_Mode_Uart_PcBox();
    }
}









