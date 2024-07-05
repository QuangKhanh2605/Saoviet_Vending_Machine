
#ifndef USER_COMM_VENDING_MACHINE_H_
#define USER_COMM_VENDING_MACHINE_H_

#define USING_APP_VENDING_MACHINE

//#include "user_define.h"

//#include "stm32f4xx_hal.h"
//#include "stm32f4xx.h"

#include "user_util.h"

//#define DEVICE_ID                   "SVVDMC23000001" 
#define MAX_LENGTH_DCU_ID	        20
#define TIME_ONE_SECOND             1000
#define TIME_ONE_MINUTES            60000
/*=============== Define Obis ===============*/
#define OBIS_SETUP_TEMP             0x15
#define OBIS_PC_BOX_FIX_MOTOR       0x16

#define OBIS_PC_BOX_CTRL_MOTOR      0x20
#define OBIS_ON_OFF_RELAY_SCREEN    0x21
#define OBIS_ON_OFF_RELAY_LAMP      0x22
#define OBIS_ON_OFF_RELAY_WARM      0x23

#define OBIS_WARNING_VIB_SENSOR     0x30
#define OBIS_WARNING_DOOR_SENSOR    0x31
#define OBIS_WARNING_ERROR_TEMP     0x32

#define OBIS_ON_GOING_PUSH          0x40
#define OBIS_COMPLETE_PUSH          0x41

#define OBIS_TSVH_PC_BOX            0x0A
#define OBIS_PC_BOX_PING_DCU        0x0B
#define OBIS_DCU_PING_PC_BOX        0x1B
#define OBIS_DCU_GET_REAL_TIME      0x1C

#define OBIS_GET_DCU_ID             0x0C
#define OBIS_SET_DCU_ID             0x0D
#define OBIS_RESET_DCU              0x0E
#define OBIS_WARNING_POWER          0x0F

#define OBIS_RESPOND_HANDLE_RELAY   0x50

#define OBIS_TIME_LOG_TSVH          0x60
#define OBIS_TIME_WARM_RUN          0x61
#define OBIS_TIME_WARM_WAIT         0x62
#define OBIS_ON_OFF_CYCLE_PC_BOX    0x63
#define OBIS_TIME_OFF_PC_BOX        0x64
#define OBIS_TIME_ON_PC_BOX         0x65
/*=============== Define Using Debug ==============*/
#define USING_APP_DELIVERY_DEBUG
#define USING_APP_DOOR_SENSOR_DEBUG
#define USING_APP_PC_BOX_DEBUG
#define USING_APP_RELAY_DEBUG
#define USING_APP_TEMPERATURE_DEBUG
#define USING_APP_VIB_SENSOR_DEBUG
#define USING_APP_ELECTRIC_DEBUG

#define  USING_REFRESH_WARM    
/*=============== Define Common ==============*/
#define TIME_ON_DCU                 5000
#define TIME_ENTRY                  15000
/*================== Struct Var ==================*/
//typedef enum
//{
//    _AT_REQUEST_SERIAL = 1,
//    _AT_REQUEST_SERVER = 2,
//    _AT_REQUEST_LORA = 3,
//}Kind_Request_AT_Cmd;


/*================== Function ===========================*/
void        Init_AppVendingMachine(void);
uint8_t     AppVendingMachine_Task(void);

uint8_t     Calculator_Crc_U16(uint16_t *crc, uint8_t* buf, uint16_t len);
uint8_t     Calculator_Crc_U8(uint8_t *crc, uint8_t* buf, uint16_t len);
void        Respond_PcBox(uint8_t *pData, uint16_t Length);
uint32_t    Scale_To_Int(uint8_t Scale);
int16_t     Calculator_Scale_Int(int16_t Value, uint8_t Scale);

#endif

