
#ifndef USER_APP_SLAVE_H__
#define USER_APP_SLAVE_H__

#define USING_APP_SLAVE

#include "event_driven.h"
#include "user_util.h"
#include "user_uart.h"

#define FLASH_ENERGY_METER      0x08000000 + 1024*30
#define FLASH_ID_BAURATE        0x08000000 + 1024*31

#define DEFAUL_READ_FLASH       0xA5A5A5A5

#define TIME_WDG_RESET_MCU      60*60000
#define TIME_INIT_UART          15*60000

#define ASCII_NUMBER_VALUE      48

#define BAUDRATE_DEFAULT        9600
#define ID_DEFAULT              1

#define NUMBER_REGISTER_SLAVE   16

#define ERROR_CODE_CHECK_CRC            0x01
#define ERROR_CODE_FUNCTION_CODE        0x02
#define ERROR_CODE_ADDRESS_OR_QUANTITY  0x03
#define ERROR_CODE_I2C_OR_SENSOR        0x04

/*================== Define ====================*/

/*================= Struct Var ================*/
typedef enum
{
    _EVENT_SLAVE_ENTRY = 0,
    _EVENT_SLAVE_RECEIVE_HANDLE,
    _EVENT_SLAVE_COMPLETE_RECEIVE,
    _EVENT_INIT_UART,

    _EVENT_SLAVE_END,
}eKindEventSlave;

typedef struct 
{
    uint8_t  ID;
    uint32_t Baudrate;
}Struct_Infor_Slave;

extern sEvent_struct            sEventAppSlave[];
extern Struct_Infor_Slave       sInforSlave;

/*================== Function Handle ===============*/
uint8_t     AppSlave_Task(void);

uint32_t    Get_Uint_In_String(uint8_t aData[], uint16_t posStart, uint16_t posEnd);
void        Send_Data_Terminal(uint8_t data[], uint8_t Length);
void        Packing_Frame(uint8_t data_frame[], uint16_t addr_register, uint16_t length);
int8_t      Terminal_Receive(void);
void        Get_Length_Variable(uint8_t *length, uint32_t variable);
void        AT_Command_IF(void);
void        Change_Baudrate_AddrSlave_Calib_ATCommand(void);
uint8_t     ModbusRTU_Slave(void);

void        Uart485_Init(uint32_t baud_rate);
void        FLASH_WritePage(uint32_t address_flash, uint32_t data1, uint32_t data2);
uint32_t    FLASH_ReadData32(uint32_t addr);
void        Init_AppSlave(void);

uint8_t     Reset_Chip (void);
#endif


