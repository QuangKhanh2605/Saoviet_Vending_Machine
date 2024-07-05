/*
    8/2021
    Thu vien Quan ly cac function convert
*/
#if defined (STM32F405xx)

    #include "stm32f4xx_hal.h"
    #include "stm32f4xx.h"
    #define FLASH_BYTE_WRTIE    4
    #define FLASH_BYTE_EMPTY    0xFF   
#endif

#if defined (STM32L151xC)  
    #include "stm32l1xx_hal.h"
    #include "stm32l1xx.h"

    #define FLASH_BYTE_WRTIE    4
    #define FLASH_BYTE_EMPTY    0x00   
#endif

#if defined (STM32L433xx) 
    #include "stm32l4xx_hal.h"
    #include "stm32l4xx.h"
    
    #define FLASH_BYTE_EMPTY    0xFF   
    #define FLASH_BYTE_WRTIE    8
#endif

#if defined (STM32L072xx) || defined (STM32L082xx)
    #include "stm32l0xx_hal.h"
    #include "stm32l0xx.h"

    #define FLASH_BYTE_WRTIE    4
    #define FLASH_BYTE_EMPTY    0x00   
#endif

#if defined (STM32L011xx) || defined (STM32L031xx)
    #include "stm32l0xx_hal.h"
    #include "stm32l0xx.h"

    #define FLASH_BYTE_WRTIE    4
    #define FLASH_BYTE_EMPTY    0x00   
#endif

#include "user_uart.h"


#ifndef USER_UTIL_H
#define USER_UTIL_H


/*===================Define=========================*/
#define TRUE 	        1
#define FALSE 	        0
#define PENDING	        2
#define ERROR           3

#define true			1
#define false			0
#define pending			2
#define error			3

#define VLEVEL_ENABLE   1       //Cho phep DEBUG printf

#define DBLEVEL_L        1       /*!< just essential traces */
#define DBLEVEL_M        2       /*!< functional traces */
#define DBLEVEL_H        3       /*!< all traces */

#define _VLEVEL_DEBUG   DBLEVEL_M


#define pi 3.14159265358979323846


#define UTILS_INIT_CRITICAL_SECTION()

/**
  * @brief macro used to enter the critical section
  */
#define UTILS_ENTER_CRITICAL_SECTION() uint32_t primask_bit= __get_PRIMASK();\
  __disable_irq()

/**
  * @brief macro used to exit the critical section
  */
#define UTILS_EXIT_CRITICAL_SECTION()  __set_PRIMASK(primask_bit)


/*===================Struct, Var=========================*/
typedef struct 
{
	uint8_t			*Data_a8;					// string
	uint16_t		Length_u16;
} sData;

typedef struct 
{
	uint8_t hour;  // read hour
	uint8_t min;   // read minute
	uint8_t sec;   // read second
    uint8_t day;   // read day
    uint8_t date;  // read date
    uint8_t month; // read month
    uint8_t year;  // read year
    uint32_t SubSeconds;
} ST_TIME_FORMAT;


typedef enum
{
    _POWER_MODE_SAVE = 0,
    _POWER_MODE_ONLINE,
}sKind_Mode_Modem;


extern uint8_t (*pFunc_Log_To_Mem) (uint8_t *pData, uint16_t Length);

/*===================extern struct, Var=========================*/
extern uint32_t        RtCountSystick_u32;
extern ST_TIME_FORMAT  sRTC;
/*===================Function=========================*/
uint32_t        Cal_Time(uint32_t Millstone_Time, uint32_t Systick_now);
uint32_t        Cal_Time_s(uint32_t Millstone_Time, uint32_t Systick_now);
uint8_t         Check_Time_Out(uint32_t Millstone_Time, uint32_t Time_Period_ms);
uint8_t         Check_Time_Out_2(uint32_t Millstone_Time, uint32_t Time_Period_ms);

float           Convert_FloatPoint_2Float (uint32_t Float_Point);
int32_t         Convert_float_2int (uint32_t Float_Point_IEEE, uint8_t scale);
int32_t         Convert_uint_2int (uint32_t Num);

void            Scale_String_Dec (sData *StrDec, uint8_t Scale);
int16_t         Convert_uint16_2int16 (uint16_t Num);
int64_t         Convert_uint64_2int64 (uint64_t Num);
uint8_t 		BBC_Cacul (uint8_t* Buff, uint16_t length);
uint8_t 		Check_BBC (sData* Str);

void            Reset_Buff(sData *str);
void            Convert_Int64_To_StringDec (sData* Payload, int64_t Data, uint8_t Scale);
void            Convert_Uint64_To_StringDec (sData* Payload, uint64_t Data, uint8_t Scale);
void 			Convert_Hex_To_String_Hex(sData *sTaget, sData *sSource);
void            Convert_String_Hex_To_Hex (sData *sSource, int Pos_Begin, sData *sTarget, uint16_t MaxLength);
uint8_t         Convert_String_To_Hex (uint8_t NumString);
uint32_t        Convert_String_To_Dec(uint8_t *pData, uint8_t lenData);

void            UTIL_MEM_set( void *dst, uint8_t value, uint16_t size );
uint8_t         Convert_Hex_To_StringHex (uint8_t Hex);

void            UTIL_Log (uint8_t Level, uint8_t *pData, uint16_t Length);
void            UTIL_Log_Str (uint8_t Level, const char *str);

void            UTIL_Printf (uint8_t Level, uint8_t *pData, uint16_t Length);
void            UTIL_Printf_Str (uint8_t Level, const char *str);
void            UTIL_Printf_Hex (uint8_t Level, uint8_t *pData, uint16_t Length);
void            UTIL_Printf_Dec (uint8_t Level, uint32_t Number);


double          UTIL_Rad_2Deg(double rad); 
double          UTIL_Deg_2Rad (double deg);
double          UTIL_Cacul_Distance (double lat1, double lon1, double lat2, double lon2, char unit); 





#endif
