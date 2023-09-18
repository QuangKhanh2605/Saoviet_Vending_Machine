#include "user_app_electric.h"
#include "user_modbus_rtu.h"
#include "user_comm_vending_machine.h"
#include "user_app_pc_box.h"
#include "user_external_flash.h"
#include "user_app_relay.h"
#include "user_app_temperature.h"
/*============== Function static =============*/
static uint8_t fevent_electric_entry(uint8_t event);
static uint8_t fevent_electric_transmit_485(uint8_t event);
static uint8_t fevent_electric_receive_485(uint8_t event);
static uint8_t fevent_electric_handle_485(uint8_t event);
static uint8_t fevent_electric_send_meter(uint8_t event);
static uint8_t fevent_electric_off_power(uint8_t event);
static uint8_t fevent_electric_handle_power(uint8_t event);
/*============== Struct ===================*/
sEvent_struct               sEventAppElectric[] = 
{
  {_EVENT_ELECTRIC_ENTRY,           1, 5, TIME_ON_DCU,      fevent_electric_entry},
  {_EVENT_ELECTRIC_TRANSMIT_485,    1, 5, 1000,             fevent_electric_transmit_485},
  {_EVENT_ELECTRIC_RECEIVE_485,     0, 0, 5,                fevent_electric_receive_485},
  {_EVENT_ELECTRIC_HANDLE_485,      0, 0, 500,              fevent_electric_handle_485},
  
  {_EVENT_ELECTRIC_SEND_METER,      1, 5, TIME_SEND_METER,  fevent_electric_send_meter}, //TIME_SEND_METER
  
  {_EVENT_ELECTRIC_OFF_POWER,       0, 5, 3000,             fevent_electric_off_power},
  
  {_EVENT_ELECTRIC_HANDLE_POWER,    0, 5, 0,                fevent_electric_handle_power},
};

Struct_Electric_Current         sElectric=
{
  .ID           = DEFAULT_ID_SLAVE,
  .Voltage      = 0, 
  .Current      = 0, 
  .ScaleVolCur  = DEFAULT_ELECTRIC_VOL_CUR_SCALE,
  .ScalePowEne  = DEFAULT_ELECTRIC_POW_ENE_SCALE,
  .PowerPresent = POWER_ERROR,
  .PowerBefore  = POWER_ERROR,
};

/*============= Function Handle =============*/
static uint8_t fevent_electric_entry(uint8_t event)
{
    fevent_active(sEventAppElectric, _EVENT_ELECTRIC_TRANSMIT_485);
    fevent_enable(sEventAppElectric, _EVENT_ELECTRIC_OFF_POWER);
    return 1;
}

static uint8_t fevent_electric_transmit_485(uint8_t event)
{
    uint8_t Frame[8];
    sData sFrame = {&Frame[0], 0};
    
    ModRTU_Master_Read_Frame(&sFrame, sElectric.ID, 0x03, 0x02, 12);
    HAL_GPIO_WritePin(NET485IO_GPIO_Port, NET485IO_Pin, GPIO_PIN_SET);
    HAL_Delay(5);
    UTIL_MEM_set(sUart485.Data_a8 , 0x00, sUart485.Length_u16);
    sUart485.Length_u16 = 0;
    
    HAL_UART_Transmit(&uart_485, sFrame.Data_a8, sFrame.Length_u16, 1000);
    HAL_GPIO_WritePin(NET485IO_GPIO_Port, NET485IO_Pin, GPIO_PIN_RESET);
    
    fevent_active(sEventAppElectric, _EVENT_ELECTRIC_RECEIVE_485);
    fevent_enable(sEventAppElectric, _EVENT_ELECTRIC_HANDLE_485);
    
    fevent_enable(sEventAppElectric, event);
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

static uint8_t fevent_electric_handle_485(uint8_t event)
{
    static uint8_t CountStatePower   = 0;
    static uint8_t CountStateConnect = 0;
  
    uint16_t Crc_Check = 0;
    uint16_t Crc_Recv  = 0;

    Crc_Recv = (sUart485.Data_a8[sUart485.Length_u16-1] << 8) |
               (sUart485.Data_a8[sUart485.Length_u16-2]);
    Crc_Check = ModRTU_CRC(sUart485.Data_a8, sUart485.Length_u16 - 2);
    if(Crc_Check == Crc_Recv)
    {
        if(sUart485.Data_a8[0] == sElectric.ID && sUart485.Data_a8[1] == 0x03)
        {
            sElectric.Voltage = sUart485.Data_a8[3]<<8 | sUart485.Data_a8[4];
            sElectric.Current = sUart485.Data_a8[9]<<8 | sUart485.Data_a8[10];
            sElectric.Current = sElectric.Current/100;
            
            sElectric.Power   = sUart485.Data_a8[15]<<24 | sUart485.Data_a8[16]<<16 
                              | sUart485.Data_a8[17]<<8  | sUart485.Data_a8[18];
            
            sElectric.Energy  = sUart485.Data_a8[23]<<24 | sUart485.Data_a8[24]<<16 
                              | sUart485.Data_a8[25]<<8  | sUart485.Data_a8[26];
            
            ConnectSlave = CONNECT_SLAVE;
            if(CountStateConnect > 0) CountStateConnect--;
            else CountStateConnect=0;
        }  
    }
    else
    {
        sElectric.Voltage = 0xFFFF;
        sElectric.Current = 0xFFFF;
        ConnectSlave = DISCONNECT_SLAVE;
        CountStateConnect++;
        
    }
    if(sUart485.Length_u16 == 0)
    {
        sElectric.Voltage = 0xFFFF;
        sElectric.Current = 0xFFFF;
        ConnectSlave = DISCONNECT_SLAVE;
        CountStateConnect++;
    }
    
    if(CountStateConnect == 0)
    {
        if(sElectric.Voltage < MACHINE_VOLTAGE_MIN) 
        {
          if(CountStatePower < NUMBER_POWER_ON_OFF) CountStatePower++;
          else CountStatePower = NUMBER_POWER_ON_OFF;
        }
        else                                        
        {
          if(CountStatePower > 0) CountStatePower--;
          else CountStatePower=0;
        }
        
        if(CountStatePower >= NUMBER_POWER_ON_OFF)
        {
            sElectric.PowerPresent = POWER_OFF;
        }
        else if(CountStatePower == 0)
        {
            sElectric.PowerPresent = POWER_ON;
        }
    }
    else
    {
        if(CountStateConnect >= NUMBER_POWER_ON_OFF)
        {
            CountStateConnect = NUMBER_POWER_ON_OFF;
            sElectric.PowerPresent = POWER_ERROR;
        }
    }
    
    fevent_disable(sEventAppElectric, _EVENT_ELECTRIC_RECEIVE_485);
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
        sElectric.PowerBefore = sElectric.PowerPresent;
        Write_Status_Electric_ExFlash();
        
        if(sElectric.PowerPresent == POWER_OFF)
        {
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"app_electric: POWER OFF", sizeof("app_electric: POWER OFF"));
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n"));
        }
        else if(sElectric.PowerPresent == POWER_ON)
        {
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"app_electric: POWER ON", sizeof("app_electric: POWER ON"));
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n"));
        }
        else
        {
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"app_electric: POWER DISCONNECT", sizeof("app_electric: POWER DISCONNECT"));
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n"));
        }
        fevent_active(sEventAppElectric, _EVENT_ELECTRIC_HANDLE_POWER);
    }
    
    fevent_enable(sEventAppElectric, event);
    return 1;
}

static uint8_t fevent_electric_handle_power(uint8_t event)
{
    if(sElectric.PowerPresent == POWER_OFF)
    {
        if(sStatusRelay.Lamp == ON_RELAY)
        ControlRelay(RELAY_LAMP, OFF_RELAY, _RL_RESPOND, _RL_DEBUG);
        
        if(sStatusRelay.FridgeCool == ON_RELAY)
        fevent_active(sEventAppTemperature, _EVENT_TEMP_OFF_FRIGE_FROZEN);
        
        if(sStatusRelay.FridgeHeat == ON_RELAY)
        ControlRelay(RELAY_FRIDGE_HEAT, OFF_RELAY, _RL_RESPOND, _RL_DEBUG);
        
        if(sStatusRelay.Warm == ON_RELAY)
        fevent_active(sEventAppRelay, _EVENT_RELAY_WARM_OFF);
        fevent_disable(sEventAppRelay,_EVENT_RELAY_WARM_ON);
    }
    else 
    {
        if(sStatusRelay.Lamp == OFF_RELAY)
        ControlRelay(RELAY_LAMP, ON_RELAY, _RL_RESPOND, _RL_DEBUG);
    }
    return 1;
}

static uint8_t fevent_electric_send_meter(uint8_t event)
{
    AppElectric_Debug();
    fevent_enable(sEventAppElectric, event);
    return 1;
}

/*=================== Function Handle =====================*/
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

void Write_Status_Electric_ExFlash(void)
{
    uint8_t aWrite[3]={0};
    aWrite[0] = DEFAULT_READ_EXFLASH;
    aWrite[1] = 0x01;
    aWrite[2] = sElectric.PowerPresent;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_STATUS_ELECTRIC);
    HAL_Delay(1);
    eFlash_S25FL_BufferWrite(aWrite, EX_FLASH_ADDR_STATUS_ELECTRIC, 3);
}

void Read_Status_Electric_ExFlash(void)
{
    uint8_t aRead[3] = {0};
    eFlash_S25FL_BufferRead(aRead, EX_FLASH_ADDR_STATUS_ELECTRIC, 3);
    if(aRead[0] == DEFAULT_READ_EXFLASH && aRead[1] == 0x01)
    {
        if(aRead[2] <= POWER_ERROR) 
        {
          sElectric.PowerPresent  = aRead[2];
          sElectric.PowerBefore   = aRead[2];
        }
    }
}

void Write_IdSlave_Electric_ExFlash(void)
{
    uint8_t aWrite[3]={0};
    aWrite[0] = DEFAULT_READ_EXFLASH;
    aWrite[1] = 0x01;
    aWrite[2] = sElectric.ID;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_IDSLAVE_ELECTRIC);
    HAL_Delay(1);
    eFlash_S25FL_BufferWrite(aWrite, EX_FLASH_ADDR_IDSLAVE_ELECTRIC, 3);
}

void Read_IdSlave_Electric_ExFlash(void)
{
    uint8_t aRead[3] = {0};
    eFlash_S25FL_BufferRead(aRead, EX_FLASH_ADDR_IDSLAVE_ELECTRIC, 3);
    if( aRead[0] == DEFAULT_READ_EXFLASH && aRead[1] == 0x01)
    {
        sElectric.ID  = aRead[2];
    }
}

void Init_AppElectric(void)
{
    Read_IdSlave_Electric_ExFlash();
    Read_Status_Electric_ExFlash();
}

void AppElectric_Debug(void)
{
#ifdef USING_APP_ELECTRIC_DEBUG
    char cData[5]={0};
    uint8_t length = 0;
    length = Convert_Int_To_String_Scale(cData, (int)sElectric.Voltage, sElectric.ScaleVolCur);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_electric: V: ", sizeof("app_electric: V: "));
    UTIL_Printf(DBLEVEL_M, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" A: ", sizeof(" A: "));
    length = Convert_Int_To_String_Scale(cData, (int)sElectric.Current , sElectric.ScaleVolCur);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n"));
#endif
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
