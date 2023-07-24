#include "user_app_electric.h"
#include "user_modbus_rtu.h"
/*============== Function static =============*/
static uint8_t fevent_electric_pgood(uint8_t event);
static uint8_t fevent_electric_2(uint8_t event);

/*============== Struct ===================*/
sEvent_struct               sEventAppElectric[] = 
{
  {_EVENT_ELECTRIC_PGOOD,        0, 0, 5,        fevent_electric_pgood},
  {_EVENT_ELECTRIC_2,            0, 0, 5,        fevent_electric_2},
};

uint8_t Status_Supply_Power = 0;
/*============= Function Handle =============*/

static uint8_t fevent_electric_pgood(uint8_t event)
{
    if(HAL_GPIO_ReadPin(PGOOD_GPIO_Port, PGOOD_Pin) == INIT_STATUS_PGOOD_SENSOR_INPUT)
    {
        Status_Supply_Power = 1;
    }
    else
    {
        Status_Supply_Power = 0;
    }
    
    fevent_enable(sEventAppElectric, event);
    return 1;
}

static uint8_t fevent_electric_2(uint8_t event)
{
    return 1;
}

void Send_Command_IVT (uint8_t SlaveID, uint8_t Func_Code, uint16_t Addr_Register, uint16_t Infor_Register, void (*pFuncResetRecvData) (void)) 
{
    uint8_t aFrame[48] = {0};
    sData   strFrame = {(uint8_t *) &aFrame[0], 0};
    
    ModRTU_Master_Read_Frame(&strFrame, SlaveID, Func_Code, Addr_Register, Infor_Register);

    HAL_GPIO_WritePin(NET485IO_GPIO_Port, NET485IO_Pin, GPIO_PIN_SET);  
    HAL_Delay(10);
    // Send
    pFuncResetRecvData();
    
    HAL_UART_Transmit(&uart_485, strFrame.Data_a8, strFrame.Length_u16, 1000); 
    
    //Dua DE ve Receive
    HAL_GPIO_WritePin(NET485IO_GPIO_Port, NET485IO_Pin, GPIO_PIN_RESET);
}

void AppIVT_Clear_Before_Recv (void)
{
    Reset_Buff(&sUart485);
}

uint8_t  AppElectric_Task(void)
{
  
    uint8_t i = 0;
    uint8_t Result = false;
    for(i = 0; i < _EVENT_ELECTRIC_END; i++)
    {
        if(sEventAppElectric[i].e_status == 1)
        {
            Result = true;
            
            if((sEventAppElectric[i].e_systick == 0) ||
               ((HAL_GetTick() - sEventAppElectric[i].e_systick) >= sEventAppElectric[i].e_period))
            {
                sEventAppElectric[i].e_status = 0;  //Disable event
                sEventAppElectric[i].e_systick= HAL_GetTick();
                sEventAppElectric[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}
