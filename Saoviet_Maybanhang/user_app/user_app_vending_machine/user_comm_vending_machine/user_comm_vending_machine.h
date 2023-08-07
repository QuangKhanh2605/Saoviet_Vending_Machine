


#ifndef USER_COMM_VENDING_MACHINE_H_
#define USER_COMM_VENDING_MACHINE_H_

//#include "user_define.h"

//#include "stm32f4xx_hal.h"
//#include "stm32f4xx.h"

#include "user_util.h"

/*=============== Define Obis ===============*/
#define OBIS_PC_BOX_FIX_MOTOR       0x19

#define OBIS_PC_BOX_CTRL_MOTOR      0x20
#define OBIS_ON_OFF_RELAY_PC        0x21
#define OBIS_ON_OFF_RELAY_SCREEN    0x22
#define OBIS_ON_OFF_RELAY_LAMP      0x23
#define OBIS_ON_OFF_RELAY_WARM      0x24

#define OBIS_WARNING_VIB_SENSOR     0x30
#define OBIS_WARNING_DOOR_SENSOR    0x31
#define OBIS_TEMP_THRESHOLD         0x32

#define OBIS_TSVH_PC_BOX            0x0A
#define OBIS_PING_PC_BOX            0x0B

/*=============== Define Common ==============*/
#define TIME_ENTRY                  15000
/*=========== Define App PcBox ================*/
#define NUMBER_MAX_MOTOR            60

/*=========== Define App Motor =================*/
#define MOTOR_PUSH_OFF                  0
#define MOTOR_PUSH_ON                   1

#define TIME_MOTOR_PUSH_EARLY           1000
#define TIME_MOTOR_PUSH_LATE            3000
#define TIME_MOTOR_RESPOND_PC_BOX       TIME_MOTOR_PUSH_LATE + 3000


/*============ Define App Relay =============*/

#define GPIO_PIN_ON_RELAY   GPIO_PIN_SET
#define GPIO_PIN_OFF_RELAY  GPIO_PIN_RESET

#define ON_RELAY            1
#define OFF_RELAY           0

/*============ Define App Door Sensor ===========*/
#define INIT_STATUS_DOOR_SENSOR_INPUT   GPIO_PIN_RESET
#define TIME_GET_DOOR_SENSOR            60000    

/*=========== Define  App Electric ============*/
#define NUMBER_SPLG_PGOOD_SENSOR_INPUT  10
#define INIT_STATUS_PGOOD_SENSOR_INPUT  GPIO_PIN_RESET

#define DEFAULT_ELECTRIC_SCALE          0xFF

/*========== Define App Temperature ===========*/
#define ADC_RESOLUTION                  4095
#define DEFAULT_TEMP_SCALE              0xFF

#define CALIB_TEMP                      10

/*================== Function ===========================*/
uint8_t     Calculator_Crc_U16(uint16_t *crc, uint8_t* buf, uint16_t len);
uint8_t     Calculator_Crc_U8(uint8_t *crc, uint8_t* buf, uint16_t len);
void        Respond_PcBox(uint8_t *pData, uint16_t Length);
uint32_t    Scale_To_Int(uint8_t Scale);
int16_t     Calculator_Scale(int16_t Value, uint8_t Scale);
#endif

