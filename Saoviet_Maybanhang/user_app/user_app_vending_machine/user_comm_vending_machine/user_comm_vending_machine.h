


#ifndef USER_COMM_VENDING_MACHINE_H_
#define USER_COMM_VENDING_MACHINE_H_

//#include "user_define.h"

//#include "stm32f4xx_hal.h"
//#include "stm32f4xx.h"

#include "user_util.h"

#define DEVICE_ID                   "SVVDMC23000001" 
#define MAX_LENGTH_DCU_ID	        20
#define DEFAULT_READ_EXFLASH        0xAA
#define TIME_ONE_SECOND             1000
/*=============== Define Obis ===============*/
#define OBIS_TEMP_THRESHOLD         0x15
#define OBIS_PC_BOX_FIX_MOTOR       0x16

#define OBIS_PC_BOX_CTRL_MOTOR      0x20
#define OBIS_ON_OFF_RELAY_SCREEN    0x21
#define OBIS_ON_OFF_RELAY_LAMP      0x22
#define OBIS_ON_OFF_RELAY_WARM      0x23

#define OBIS_WARNING_VIB_SENSOR     0x30
#define OBIS_WARNING_DOOR_SENSOR    0x31

#define OBIS_ON_GOING_PUSH          0x40
#define OBIS_COMPLETE_PUSH          0x41

#define OBIS_TSVH_PC_BOX            0x0A
#define OBIS_PING_PC_BOX            0x0B

#define OBIS_GET_DCU_ID             0x0C
#define OBIS_SET_DCU_ID             0x0D
#define OBIS_RESET_DCU              0x0E
#define OBIS_WARNING_POWER          0x0F
/*=============== Define Using Debug ==============*/
#define USING_APP_CTRL_MOTOR_DEBUG
#define USING_APP_DOOR_SENSOR_DEBUG
#define USING_APP_PC_BOX_DEBUG
#define USING_APP_RELAY_DEBUG
#define USING_APP_TEMPERATURE_DEBUG
#define USING_APP_VIB_SENSOR_DEBUG

/*=============== Define Common ==============*/
#define TIME_ENTRY                  15000
/*=========== Define App PcBox ================*/
#define NUMBER_MAX_MOTOR            60
#define TIME_SEND_TSVH              60000
#define TIME_RESET_WDG              1000

#define TIME_RESPOND_PC_BOX         2000    
#define TIME_REFRESH_DCU            2*86400*TIME_ONE_SECOND
/*=========== Define App Motor =================*/

#define TIME_MOTOR_PUSH_EARLY           1000
#define TIME_MOTOR_PUSH_LATE            3000
#define TIME_MOTOR_RESPOND_PC_BOX       TIME_MOTOR_PUSH_LATE + 2000


/*============ Define App Relay =============*/

#define GPIO_PIN_ON_RELAY   GPIO_PIN_SET
#define GPIO_PIN_OFF_RELAY  GPIO_PIN_RESET

#define ON_RELAY            1
#define OFF_RELAY           0

#define NUMBER_RELAY        7
/*============ Define App Door Sensor ===========*/
#define INIT_STATUS_DOOR_SENSOR_INPUT   GPIO_PIN_RESET

/*=========== Define  App Electric ============*/
#define NUMBER_SPLG_PGOOD_SENSOR_INPUT  10
#define INIT_STATUS_PGOOD_SENSOR_INPUT  GPIO_PIN_RESET

#define DEFAULT_ELECTRIC_SCALE          0xFF

/*========== Define App Temperature ===========*/
#define ADC_RESOLUTION                  4095
#define DEFAULT_TEMP_SCALE              0xFF

#define CALIB_TEMP                      10


/*================== Struct Var ==================*/
typedef enum
{
    _AT_REQUEST_SERIAL = 1,
    _AT_REQUEST_SERVER = 2,
    _AT_REQUEST_LORA = 3,
}Kind_Request_AT_Cmd;


/*================== Function ===========================*/
uint8_t     Calculator_Crc_U16(uint16_t *crc, uint8_t* buf, uint16_t len);
uint8_t     Calculator_Crc_U8(uint8_t *crc, uint8_t* buf, uint16_t len);
void        Respond_PcBox(uint8_t *pData, uint16_t Length);
uint32_t    Scale_To_Int(uint8_t Scale);
int16_t     Calculator_Scale(int16_t Value, uint8_t Scale);

uint8_t     Reset_Chip(void);
uint8_t     Convert_Int_To_String(char cData[], int var);
uint8_t     Convert_Int_To_String_Scale(char cData[], int var, uint8_t Scale);
#endif

