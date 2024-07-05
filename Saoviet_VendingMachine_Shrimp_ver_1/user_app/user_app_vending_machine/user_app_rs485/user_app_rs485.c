#include "user_app_rs485.h"
#include "user_modbus_rtu.h"
#include "user_external_flash.h"
#include "user_app_electric.h"
#include "user_inc_vending_machine.h"
/*=========================Fucntion Static=========================*/
static uint8_t fevent_rs485_entry(uint8_t event);
static uint8_t fevent_rs485_transmit(uint8_t event);
static uint8_t fevent_rs485_receive_handle(uint8_t event);
static uint8_t fevent_rs485_receive_complete(uint8_t event);
static uint8_t fevent_rs485_init_uart(uint8_t event);
/*==============================Struct=============================*/
sEvent_struct               sEventAppRs485[]=
{
  {_EVENT_RS485_ENTRY,              1, 5, 10000,            fevent_rs485_entry},            
  {_EVENT_RS485_TRANSMIT,           1, 0, 1000,             fevent_rs485_transmit},
  {_EVENT_RS485_RECEIVE_HANDLE,     0, 5, 5,                fevent_rs485_receive_handle},
  {_EVENT_RS485_RECEIVE_COMPLETE,   0, 5, 500,              fevent_rs485_receive_complete},
  
  {_EVENT_RS485_INIT_UART,          0, 5, 60000,            fevent_rs485_init_uart},
};
uint16_t CountBufferHandleRecv = 0;

Struct_IdSlave  sIdSlave485=
{
    .Electric = ID_DEFAULT_ELECTRIC,
    .Weighing = ID_DEFAULT_WEIGHING,
};

Struct_KindMode485  sKindMode485=
{
    .Trans = _RS485_OPERA_ELECTRIC,
};

Struct_State_Slave  sStateSlave485=
{
    .Electric = DISCONNECT_SLAVE,
};

/*========================Function Handle========================*/

static uint8_t fevent_rs485_entry(uint8_t event)
{
    fevent_enable(sEventAppRs485, _EVENT_RS485_INIT_UART);
    return 1;
}

static uint8_t fevent_rs485_transmit(uint8_t event)
{
/*--------------------Hoi du lieu tu Slave-------------------*/

    uint8_t Frame[20]={0};
//    uint8_t aData[4]={0};
    sData sFrame = {&Frame[0], 0};
    
    switch(sKindMode485.Trans)
    {
        case _RS485_OPERA_ELECTRIC:
          ModRTU_Master_Read_Frame(&sFrame, sIdSlave485.Electric, 0x03, 0x02, 12);
          break;
          
        case _RS485_OPERA_WEIGHING:
          ModRTU_Master_Read_Frame(&sFrame, sIdSlave485.Weighing, 0x03, 0x00, 1);
          break;
          
        default:
          break;
    }
    
    sKindMode485.Recv = sKindMode485.Trans;
    //Transmit RS485
    HAL_GPIO_WritePin(RS485_TXDE_GPIO_Port, RS485_TXDE_Pin, GPIO_PIN_SET);
    HAL_Delay(5);
    UTIL_MEM_set(sUart485.Data_a8 , 0x00, sUart485.Length_u16);
    sUart485.Length_u16 = 0;
    CountBufferHandleRecv = 0;
    
    HAL_UART_Transmit(&uart_485, sFrame.Data_a8, sFrame.Length_u16, 1000);
    HAL_GPIO_WritePin(RS485_TXDE_GPIO_Port, RS485_TXDE_Pin, GPIO_PIN_RESET);
    
    if(sKindMode485.Trans == _RS485_OPERA_ELECTRIC)
      sKindMode485.Trans = _RS485_OPERA_WEIGHING;
    else if(sKindMode485.Trans == _RS485_OPERA_WEIGHING)
      sKindMode485.Trans = _RS485_OPERA_ELECTRIC;
       
    fevent_active(sEventAppRs485, _EVENT_RS485_RECEIVE_HANDLE);
    fevent_enable(sEventAppRs485, _EVENT_RS485_RECEIVE_COMPLETE);
    fevent_enable(sEventAppRs485, event);
    return 1;
}

static uint8_t fevent_rs485_receive_handle(uint8_t event)
{
/*-----------------Kiem tra da nhan xong tu 485------------*/
    if(sUart485.Length_u16 != 0)
    {
        if(CountBufferHandleRecv == sUart485.Length_u16)
        {
            CountBufferHandleRecv = 0;
            fevent_active(sEventAppRs485, _EVENT_RS485_RECEIVE_COMPLETE);
            return 1;
        }
        else
        {
            CountBufferHandleRecv = sUart485.Length_u16;
        }
    }
    
    fevent_enable(sEventAppRs485, event);
    return 1;
}

static uint8_t fevent_rs485_receive_complete(uint8_t event)
{
/*------------------Xu ly chuoi nhan duoc----------------*/
    static uint8_t CountStatePower   = 0;
    static uint8_t CountPowerDisconnect = 0;
  
    uint16_t Crc_Check = 0;
    uint16_t Crc_Recv  = 0;

    Crc_Recv = (sUart485.Data_a8[sUart485.Length_u16-1] << 8) |
               (sUart485.Data_a8[sUart485.Length_u16-2]);
    Crc_Check = ModRTU_CRC(sUart485.Data_a8, sUart485.Length_u16 - 2);
    if(Crc_Check == Crc_Recv)
    {
        fevent_enable(sEventAppRs485, _EVENT_RS485_INIT_UART);
        switch(sKindMode485.Recv)
        {
            case _RS485_OPERA_ELECTRIC:
                if(sUart485.Data_a8[0] == sElectric.ID && sUart485.Data_a8[1] == 0x03)
                {
                    sElectric.Voltage = sUart485.Data_a8[3]<<8 | sUart485.Data_a8[4];
                    sElectric.Current = sUart485.Data_a8[9]<<8 | sUart485.Data_a8[10];
                    sElectric.Current = sElectric.Current/10;
                    
                    if(sElectric.Current%10 >= 5)
                        sElectric.Current = sElectric.Current/10 + 1;
                    else 
                        sElectric.Current = sElectric.Current/10;
                    
                    sElectric.Power   = sUart485.Data_a8[15]<<24 | sUart485.Data_a8[16]<<16 
                                      | sUart485.Data_a8[17]<<8  | sUart485.Data_a8[18];
                    
                    sElectric.Energy  = sUart485.Data_a8[23]<<24 | sUart485.Data_a8[24]<<16 
                                      | sUart485.Data_a8[25]<<8  | sUart485.Data_a8[26];

                    sStateSlave485.Electric = CONNECT_SLAVE;
                    if(CountPowerDisconnect > 0) CountPowerDisconnect--;
                    else CountPowerDisconnect=0;
                }
                break;
                
            case _RS485_OPERA_WEIGHING:
              sParamDelivery.Weighing = sUart485.Data_a8[3]<<8 | sUart485.Data_a8[4];
              sParamDelivery.StateConnectWeight = _WEIGHT_485_CONNECT;
              break;

            default:
                break;
        }
    }
    else
    {
        switch(sKindMode485.Recv)
        {
            case _RS485_OPERA_ELECTRIC:
              CountPowerDisconnect++;
              break;
              
            case _RS485_OPERA_WEIGHING:
              sParamDelivery.Weighing = 0;
              sParamDelivery.StateConnectWeight = _WEIGHT_485_DISCONNECT;
              break;
              
            default:
              break;
        }
    }
    
    if(sUart485.Length_u16 == 0)
    {
        switch(sKindMode485.Recv)
        {
            case _RS485_OPERA_ELECTRIC:
              CountPowerDisconnect++;
              break;
              
            case _RS485_OPERA_WEIGHING:
              sParamDelivery.Weighing = 0;
              sParamDelivery.StateConnectWeight = _WEIGHT_485_DISCONNECT;
              break;
              
            default:
              break;
        }
    }
    
    if(CountPowerDisconnect == 0)
    {
        if(sElectric.Voltage < MACHINE_VOLTAGE_MIN) 
        {
          if(CountStatePower < NUMBER_POWER_ON_OFF) 
              CountStatePower++;
          else 
              CountStatePower = NUMBER_POWER_ON_OFF;
        }
        else                                        
        {
          if(CountStatePower > 0) 
              CountStatePower--;
          else 
              CountStatePower=0;
        }
        
        if(CountStatePower >= NUMBER_POWER_ON_OFF)
            sElectric.PowerPresent = POWER_OFF;
        else if(CountStatePower == 0)
            sElectric.PowerPresent = POWER_ON;
    }
    else
    {
        sElectric.Voltage = 0xFFFF;
        sElectric.Current = 0xFFFF;
        sElectric.Power   = 0xFFFF;
        sStateSlave485.Electric = DISCONNECT_SLAVE;
        
        if(CountPowerDisconnect >= NUMBER_POWER_ON_OFF)
        {
            CountPowerDisconnect = NUMBER_POWER_ON_OFF;
            sElectric.PowerPresent = POWER_ERROR;
        }
    }
    
    sElectric.PowerPresent = POWER_ON;  //Luon co dien de k bi gian doan chuong trinh
    
    fevent_disable(sEventAppRs485, _EVENT_RS485_RECEIVE_HANDLE);
    return 1;
}

static uint8_t fevent_rs485_init_uart(uint8_t event)
{
    MX_USART1_UART_Init();
    Init_RX_Mode_Uart_485();

    sKindMode485.Trans = _RS485_OPERA_ELECTRIC;
    fevent_disable(sEventAppRs485, _EVENT_RS485_RECEIVE_HANDLE);
    fevent_disable(sEventAppRs485, _EVENT_RS485_RECEIVE_COMPLETE);
    fevent_active(sEventAppRs485, _EVENT_RS485_TRANSMIT);
    
    
    fevent_enable(sEventAppRs485, event);
    return 1;
}

/*================== Function Handle ====================*/

uint8_t        Save_IdSlave(uint8_t ID_Electric)
{
    uint8_t write[2]={0};
    sIdSlave485.Electric = ID_Electric;
    write[0] = BYTE_TEMP_FIRST_EXFLASH;
    write[1] = sIdSlave485.Electric;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_ID_SLAVE_485);
    eFlash_S25FL_BufferWrite(write, EX_FLASH_ADDR_ID_SLAVE_485, 2);
    return 1;
}

void        Init_IdSlave(void)
{
    uint8_t read[2] = {0};
    eFlash_S25FL_BufferRead(read, EX_FLASH_ADDR_ID_SLAVE_485 , 2);
    if(read[0] == BYTE_TEMP_FIRST_EXFLASH)
    {
        sIdSlave485.Electric = read[1];
    }
}

/*==========================Handle Task==========================*/
void       Init_AppRs485(void)
{
    Init_IdSlave();
}

uint8_t        AppRs485_Task(void)
{
    uint8_t i = 0;
    uint8_t Result =  false;
    
    for(i = 0; i < _EVENT_RS485_END; i++)
    {
        if(sEventAppRs485[i].e_status == 1)
        {
            Result = true; 
            
            if((sEventAppRs485[i].e_systick == 0) ||
               ((HAL_GetTick() - sEventAppRs485[i].e_systick) >= sEventAppRs485[i].e_period))
            {
                sEventAppRs485[i].e_status = 0; //Disable event
                sEventAppRs485[i].e_systick= HAL_GetTick();
                sEventAppRs485[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}

