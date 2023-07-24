

#ifndef USER_UTIL_H_
#define USER_UTIL_H_

//#include "user_define.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

#define false 0
#define true  0

#define NUMBER_SPLG_COUNT_INPUT         10
#define INIT_STATUS_COUNT_INPUT         GPIO_PIN_RESET

#define INIT_STATUS_DOOR_SENSOR_INPUT   GPIO_PIN_RESET

#define NUMBER_SPLG_VIB_SENSOR_INPUT    10
#define INIT_STATUS_VIB_SENSOR_INPUT    GPIO_PIN_RESET

#define INIT_STATUS_PGOOD_SENSOR_INPUT  GPIO_PIN_RESET

#define NUMBER_SPLG_IR_SENSOR_INPUT     10
#define INIT_STATUS_IR_SENSOR_INPUT     GPIO_PIN_RESET

/*===================Struct, Var=========================*/
typedef struct 
{
	uint8_t			*Data_a8;					// string
	uint16_t		Length_u16;
} sData;

/*================== Function ===========================*/
uint8_t     Calculator_Crc_U16(uint16_t *crc, uint8_t* buf, uint16_t len);
uint8_t     Calculator_Crc_U8(uint8_t *crc, uint8_t* buf, uint16_t len);
void        UTIL_MEM_set( void *dst, uint8_t value, uint16_t size );
void        Reset_Buff(sData *str);

#endif
