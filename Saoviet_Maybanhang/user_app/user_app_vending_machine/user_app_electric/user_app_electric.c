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
static uint8_t fevent_electric_change_status_power(uint8_t event);
static uint8_t fevent_electric_init_uart(uint8_t event);
/*============== Struct ===================*/
sEvent_struct               sEventAppElectric[] = 
{
  {_EVENT_ELECTRIC_ENTRY,               1, 5, TIME_ON_DCU,      fevent_electric_entry},
  {_EVENT_ELECTRIC_TRANSMIT_485,        1, 5, 1000,             fevent_electric_transmit_485},
  {_EVENT_ELECTRIC_RECEIVE_485,         0, 0, 5,                fevent_electric_receive_485},
  {_EVENT_ELECTRIC_HANDLE_485,          0, 0, 500,              fevent_electric_handle_485},
  
  {_EVENT_ELECTRIC_SEND_METER,          1, 5, TIME_SEND_METER,  fevent_electric_send_meter}, //Debug TIME_SEND_METER
  
  {_EVENT_ELECTRIC_CHANGE_STATUS_POWER, 0, 5, 3000,             fevent_electric_change_status_power},
  
  {_EVENT_ELECTRIC_INIT_UART,           1, 5, TIME_INIT_UART,   fevent_electric_init_uart},
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
    fevent_enable(sEventAppElectric, _EVENT_ELECTRIC_CHANGE_STATUS_POWER);
    return 1;
}

static uint8_t fevent_electric_transmit_485(uint8_t event)
{
/*--------------------Hoi du lieu tu Slave-------------------*/
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
/*-----------------Kiem tra da nhan xong tu 485------------*/
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
/*------------------Xu ly chuoi nhan duoc----------------*/
    static uint8_t CountStatePower   = 0;
    static uint8_t CountStateConnect = 0;
  
    uint16_t Crc_Check = 0;
    uint16_t Crc_Recv  = 0;

    Crc_Recv = (sUart485.Data_a8[sUart485.Length_u16-1] << 8) |
               (sUart485.Data_a8[sUart485.Length_u16-2]);
    Crc_Check = ModRTU_CRC(sUart485.Data_a8, sUart485.Length_u16 - 2);
    if(Crc_Check == Crc_Recv)
    {
        fevent_enable(sEventAppElectric, _EVENT_ELECTRIC_INIT_UART);
        if(sUart485.Data_a8[0] == sElectric.ID && sUart485.Data_a8[1] == 0x03)
        {
            sElectric.Voltage = sUart485.Data_a8[3]<<8 | sUart485.Data_a8[4];
            sElectric.Current = sUart485.Data_a8[9]<<8 | sUart485.Data_a8[10];
            sElectric.Current = sElectric.Current/10;
            
            if(sElectric.Current%10 >= 5)
            {
                sElectric.Current = sElectric.Current/10 + 1;
            }
            else sElectric.Current = sElectric.Current/10;
            
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
        sElectric.Power   = 0xFFFF;
        ConnectSlave = DISCONNECT_SLAVE;
        CountStateConnect++;
        
    }
    if(sUart485.Length_u16 == 0)
    {
        sElectric.Voltage = 0xFFFF;
        sElectric.Current = 0xFFFF;
        sElectric.Power   = 0xFFFF;
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

static uint8_t fevent_electric_send_meter(uint8_t event)
{
/*-------------Gui debug------------*/
    AppElectric_Debug();
    fevent_enable(sEventAppElectric, event);
    return 1;
}

static uint8_t fevent_electric_change_status_power(uint8_t event)
{
/*------------------Kiem tra thay doi trang thai nguon---------------*/
    if(sElectric.PowerPresent != sElectric.PowerBefore)
    {
        Status_Power_Respond_PcBox();

        sElectric.PowerBefore = sElectric.PowerPresent;
        Write_Status_Electric_ExFlash();
        
        if(sElectric.PowerPresent == POWER_OFF)
        {
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"app_electric: POWER OFF", sizeof("app_electric: POWER OFF")-1);
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
        }
        else if(sElectric.PowerPresent == POWER_ON)
        {
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"app_electric: POWER ON", sizeof("app_electric: POWER ON")-1);
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
        }
        else
        {
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"app_electric: POWER DISCONNECT", sizeof("app_electric: POWER DISCONNECT")-1);
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
        }
        Handle_State_Power();
    }
    
    fevent_enable(sEventAppElectric, event);
    return 1;
}

void Handle_State_Power(void)
{
/*-------------------Xu ly truong hop mat va co dien--------------*/
    if(sElectric.PowerPresent == POWER_OFF)
    {
        if(sStatusRelay.Lamp == ON_RELAY)
        ControlRelay(RELAY_LAMP, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_UNCTRL);
        
        if(sStatusRelay.FridgeCool == ON_RELAY)
        fevent_active(sEventAppTemperature, _EVENT_TEMP_OFF_FRIGE_FROZEN);
        
        if(sStatusRelay.FridgeHeat == ON_RELAY)
        ControlRelay(RELAY_FRIDGE_HEAT, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_UNCTRL);
        
        if(sStatusRelay.Warm == ON_RELAY)
        fevent_active(sEventAppRelay, _EVENT_RELAY_WARM_OFF);
        fevent_disable(sEventAppRelay,_EVENT_RELAY_WARM_ON);
    }
    else 
    {
        if(sStatusRelay.Lamp == OFF_RELAY)
        {
            if(sStatusRelay.Lamp_Ctrl == ON_RELAY)
            {
                ControlRelay(RELAY_LAMP, ON_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_UNCTRL);
            }
        }
    }
}

static uint8_t fevent_electric_init_uart(uint8_t event)
{
    MX_UART4_Init();
    Init_Uart_485_Rx_IT();
    fevent_enable(sEventAppElectric, event);
    return 1;
}

/*=================== Function Handle =====================*/
/*
    @brief  Gui trang thai canh bao nguon len PcBox
*/
void  Status_Power_Respond_PcBox(void)
{
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_WARNING_POWER;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.PowerPresent;
    
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
}

/*
    @brief  Dong goi lenh gui Slave
*/
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

/*
    @brief  Luu trang thai nguon vao Flash
*/
void Write_Status_Electric_ExFlash(void)
{
    uint8_t aWrite[2]={0};
    aWrite[0] = DEFAULT_READ_EXFLASH;
    aWrite[1] = sElectric.PowerPresent;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_STATUS_ELECTRIC);
    HAL_Delay(1);
    eFlash_S25FL_BufferWrite(aWrite, EX_FLASH_ADDR_STATUS_ELECTRIC, 2);
}

/*
    @brief  Doc trang thai nguon tu Flash
*/
void Read_Status_Electric_ExFlash(void)
{
    uint8_t aRead[2] = {0};
    eFlash_S25FL_BufferRead(aRead, EX_FLASH_ADDR_STATUS_ELECTRIC, 2);
    if(aRead[0] == DEFAULT_READ_EXFLASH)
    {
        if(aRead[1] <= POWER_ERROR) 
        {
          sElectric.PowerPresent  = aRead[1];
          sElectric.PowerBefore   = aRead[1];
        }
    }
}

/*
    @brief  Viet Id Slave vao Flash
*/
void Write_IdSlave_Electric_ExFlash(void)
{
    uint8_t aWrite[2]={0};
    aWrite[0] = DEFAULT_READ_EXFLASH;
    aWrite[1] = sElectric.ID;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_IDSLAVE_ELECTRIC);
    HAL_Delay(1);
    eFlash_S25FL_BufferWrite(aWrite, EX_FLASH_ADDR_IDSLAVE_ELECTRIC, 2);
}

/*
    @brief  Doc ID slave tu Flash
*/
void Read_IdSlave_Electric_ExFlash(void)
{
    uint8_t aRead[2] = {0};
    eFlash_S25FL_BufferRead(aRead, EX_FLASH_ADDR_IDSLAVE_ELECTRIC, 2);
    if( aRead[0] == DEFAULT_READ_EXFLASH )
    {
        sElectric.ID  = aRead[1];
    }
}

void Init_AppElectric(void)
{
    Read_IdSlave_Electric_ExFlash();
    Read_Status_Electric_ExFlash();
}

/*
    @brief Debug app Electric
*/
void AppElectric_Debug(void)
{
#ifdef USING_APP_ELECTRIC_DEBUG
    char cData[12]={0};
    uint8_t length = 0;
    length = Convert_Int_To_String_Scale(cData, (int)sElectric.Voltage, sElectric.ScaleVolCur);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_electric: V: ", sizeof("app_electric: V: ")-1);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)cData, length);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" A: ", sizeof(" A: ")-1);
    length = Convert_Int_To_String_Scale(cData, (int)sElectric.Current , sElectric.ScaleVolCur);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)cData, length);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" P: ", sizeof(" P: ")-1);
    length = Convert_Int_To_String_Scale(cData, (int)sElectric.Power , sElectric.ScalePowEne);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)cData, length);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" E: ", sizeof(" E: ")-1);
    length = Convert_Int_To_String_Scale(cData, (int)sElectric.Energy , sElectric.ScalePowEne);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
    
    
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
