#include "user_app_electric.h"
#include "user_modbus_rtu.h"
#include "user_comm_vending_machine.h"
#include "user_app_pc_box.h"
#include "user_external_flash.h"
/*============== Function static =============*/
static uint8_t fevent_electric_entry(uint8_t event);
static uint8_t fevent_electric_pgood(uint8_t event);
static uint8_t fevent_electric_transmit_485(uint8_t event);
static uint8_t fevent_electric_receive_485(uint8_t event);
static uint8_t fevent_electric_handle(uint8_t event);
static uint8_t fevent_electric_off_power(uint8_t event);
/*============== Struct ===================*/
sEvent_struct               sEventAppElectric[] = 
{
  {_EVENT_ELECTRIC_ENTRY,           1, 5, TIME_ENTRY,  fevent_electric_entry},
  {_EVENT_ELECTRIC_PGOOD,           1, 0, 10,          fevent_electric_pgood},
  {_EVENT_ELECTRIC_TRANSMIT_485,    0, 0, 5,           fevent_electric_transmit_485},
  {_EVENT_ELECTRIC_RECEIVE_485,     1, 0, 5,           fevent_electric_receive_485},
  {_EVENT_ELECTRIC_HANDLE_485,      0, 0, 5,           fevent_electric_handle},
  
  {_EVENT_ELECTRIC_OFF_POWER,       1, 0, 100,         fevent_electric_off_power},
};

Struct_Electric_Current         sElectric=
{
  .Voltage = 0, 
  .Current = 0, 
  .Scale = DEFAULT_ELECTRIC_SCALE,
  .PowerPresent = POWER_ON,
  .PowerBefore  = POWER_ON,
};
/*============= Function Handle =============*/
static uint8_t fevent_electric_entry(uint8_t event)
{
    fevent_active(sEventAppElectric, _EVENT_ELECTRIC_PGOOD);
    return 1;
}

static uint8_t fevent_electric_pgood(uint8_t event)
{
//    if(HAL_GPIO_ReadPin(PGOOD_GPIO_Port, PGOOD_Pin) == INIT_STATUS_PGOOD_SENSOR_INPUT)
//    {
//        Status_Supply_Power = 1;
//    }
//    else
//    {
//        Status_Supply_Power = 0;
//    }
    
    static uint8_t count_handle  = 0;
    static uint8_t status_current= 0;
    static uint8_t status_before = INIT_STATUS_PGOOD_SENSOR_INPUT;
    
    status_current = HAL_GPIO_ReadPin(PGOOD_GPIO_Port, PGOOD_Pin);
    if(status_current == status_before)
    {
        count_handle++;
        if(count_handle >= NUMBER_SPLG_PGOOD_SENSOR_INPUT)
        {
            count_handle = NUMBER_SPLG_PGOOD_SENSOR_INPUT;
            if(status_current == INIT_STATUS_PGOOD_SENSOR_INPUT)
            {
                sElectric.PowerPresent = 1;
            }
            else
            {
                sElectric.PowerPresent = 0;
            }
        }
    }
    else
    {
        count_handle = 0;
    }
    status_before = HAL_GPIO_ReadPin(PGOOD_GPIO_Port, PGOOD_Pin);
    
    fevent_enable(sEventAppElectric, event);
    return 1;
}

static uint8_t fevent_electric_transmit_485(uint8_t event)
{
    return 1;
}

static uint8_t fevent_electric_receive_485(uint8_t event)
{
    static uint16_t countBuffer_uart = 0;

    if(sUart485.Length_u16 != 0)
    {
        if(countBuffer_uart == sUart485.Length_u16)
        {
            countBuffer_uart = 0;
            fevent_active(sEventAppElectric, _EVENT_ELECTRIC_HANDLE_485);
            return 1;
        }
        else
        {
            countBuffer_uart = sUart485.Length_u16;
        }
    }
    
    fevent_enable(sEventAppElectric, event);
    return 1;
}

static uint8_t fevent_electric_handle(uint8_t event)
{
    UTIL_MEM_set(sUart485.Data_a8 , 0x00, sUart485.Length_u16);
    sUart485.Length_u16 = 0;
    fevent_active(sEventAppElectric, _EVENT_ELECTRIC_RECEIVE_485);
    return 1;
}

static uint8_t fevent_electric_off_power(uint8_t event)
{
    if(sElectric.PowerPresent != sElectric.PowerBefore)
    {
        uint8_t aData[5]={0};
        uint8_t length = 0;
        length = Status_Power_Respond_PcBox(aData);
        Write_Queue_Repond_PcBox(aData, length);
        Write_Status_Electric_ExFlash();
    }
       
    sElectric.PowerBefore = sElectric.PowerPresent;
    
    fevent_enable(sEventAppElectric, event);
    return 1;
}

uint8_t  Status_Power_Respond_PcBox(uint8_t aData[])
{
    uint8_t length = 0;
    uint16_t TempCrc = 0;
    aData[length++] = OBIS_WARNING_POWER;
    aData[length++] = 0x01;
    aData[length++] = sElectric.PowerPresent;
    
    Calculator_Crc_U16(&TempCrc, aData, length);
    
    aData[length++] = TempCrc;
    aData[length++] = TempCrc >> 8;
    
    return length;
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

/*============== Function Handle ==============*/ 
void Write_Status_Electric_ExFlash(void)
{
    uint8_t aWrite[9]={0};
    aWrite[0] = DEFAULT_READ_EXFLASH;
    aWrite[1] = NUMBER_RELAY;
    aWrite[2] = sElectric.PowerPresent;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_STATUS_ELECTRIC);
    HAL_Delay(1);
    eFlash_S25FL_BufferWrite(aWrite, EX_FLASH_ADDR_STATUS_ELECTRIC, 3);
}

void Read_Status_Electric_ExFlash(void)
{
    uint8_t aRead[9] = {0};
    eFlash_S25FL_BufferRead(aRead, EX_FLASH_ADDR_STATUS_ELECTRIC, 3);
    if( aRead[0] == DEFAULT_READ_EXFLASH && aRead[1] == NUMBER_RELAY)
    {
        if(aRead[2] <= 0x01) 
        {
          sElectric.PowerPresent  = aRead[2];
          sElectric.PowerBefore   = aRead[2];
        }
    }
}

void Init_AppElectric(void)
{
    Read_Status_Electric_ExFlash();
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
