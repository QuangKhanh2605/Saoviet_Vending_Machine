


#ifndef USER_APP_MEM_H_
#define USER_APP_MEM_H_


#include "user_util.h"
#include "user_define.h"

#include "user_internal_mem.h"
//#include "user_external_eeprom.h"
#include "user_external_flash.h"



#define USING_APP_MEM
/*
    define Memory use save record:
    + MEM_ON_FLASH:     on chip flash
    + EX_MEM_EEPROM:    external eeprom
    + EX_MEM_FLASH:     external flash

//#define MEMORY_ON_FLASH     
//#define EX_MEM_EEPROM   
//#define EX_MEM_FLASH    

*/
#define PARAM_DCU_EX_MEM_FLASH
//#define PARAM_DCU_MEMORY_ON_FLASH
      
#define EX_MEM_FLASH   
      

#ifdef MEMORY_ON_FLASH
    #define AppMem_Status               OnFlash_Status
    #define AppMem_Setup_Init           OnFlash_Setup_Init
    #define AppMem_Test_Write           OnFlash_Test_Write
    #define AppMem_Queue_Read_Empty     OnFlash_Is_Queue_Read_Empty
    #define AppMem_Queue_Write_Empty    OnFlash_Is_Queue_Write_Empty
    #define AppMem_Send_To_Queue_Read   OnFlash_Send_To_Queue_Read    
    #define AppMem_Is_Write_To_PartAB() false
    #define AppMem_sData_Read           sOnFlash.sHRead.aData 
    #define AppMem_Send_To_Queue_Write  OnFlash_Send_To_Queue_Write
#endif

#ifdef EX_MEM_EEPROM
    #define AppMem_Status               ExEEPROM_Status
    #define AppMem_Setup_Init           ExEEPROM_Setup_Init
    #define AppMem_Test_Write           ExEEPROM_Test_Write
    #define AppMem_Queue_Read_Empty     ExEEPROM_Is_Queue_Read_Empty
    #define AppMem_Queue_Write_Empty    ExEEPROM_Is_Queue_Write_Empty
    #define AppMem_Send_To_Queue_Read   ExEEPROM_Send_To_Queue_Read   
    #define AppMem_Is_Write_To_PartAB() false
    #define AppMem_sData_Read           sExEEPROM.sHRead.aData
    #define AppMem_Send_To_Queue_Write  ExEEPROM_Send_To_Queue_Write
#endif

#ifdef EX_MEM_FLASH
    #define AppMem_Status               eFlash_Status
    #define AppMem_Setup_Init           eFlash_Setup_Init
    #define AppMem_Test_Write           eFlash_Test_Write
    #define AppMem_Queue_Read_Empty     eFlash_Is_Queue_Read_Empty
    #define AppMem_Queue_Write_Empty    eFlash_Is_Queue_Write_Empty
    #define AppMem_Send_To_Queue_Read   eFlash_Send_To_Queue_Read
    #define AppMem_Is_Write_To_PartAB() eFlash_Check_Write_To_PartAB()
    #define AppMem_sData_Read           sExFlash.sHRead.aData        
    #define AppMem_Send_To_Queue_Write  eFlash_Send_To_Queue_Write
#endif



/*=== Time Read again mess from flash if: read before not success (not increase index)*/
#define MAX_TIME_RETRY_READ             10000   

//#define FLASH_POWER_OFF     HAL_GPIO_WritePin(FLASH_ON_OFF_GPIO_Port, FLASH_ON_OFF_Pin, GPIO_PIN_SET)    
//#define FLASH_POWER_ON      HAL_GPIO_WritePin(FLASH_ON_OFF_GPIO_Port, FLASH_ON_OFF_Pin, GPIO_PIN_RESET)      


#define MAX_BYTE_CUT_GPS    18    
/*================ Var struct =================*/
typedef enum
{
    _EVENT_MEM_POWER_ON,
    _EVENT_MEM_TEST_WRITE,
    _EVENT_MEM_ERASE,
    _EVENT_MEM_INIT_POS_REC,
    
    _EVENT_MEM_CHECK_NEW_TSVH,  
    _EVENT_MEM_CHECK_NEW_EVENT,  
    _EVENT_MEM_CHECK_NEW_GPS,  

    _EVENT_MEM_CHECK_RQ_AT,     
	_EVENT_END_MEM,
}eKindsEventAppMem;

typedef enum
{
    _MEM_MQTT_INFOR,
    
}eKindSubParam;


typedef struct
{
    uint8_t     Port_u8;
    uint16_t    IndexStart_u16;
    uint16_t    IndexStop_u16;
    uint16_t    MaxIndex_u16;
    
    uint8_t     TypeData_u8;
    uint8_t     Status_u8;
}sRequestATcmd;



typedef struct
{
    uint8_t     Status_u8;
    uint8_t     PendingNewMess_u8;
    uint8_t     PendingInit_u8;
    
    sRequestATcmd   sReqATcmd;
    uint8_t     CountPendingNewRec;
}sAppMemVariable;


typedef void (*pMem_Wrtie_OK) (void);
typedef void (*pMem_Wrtie_Fail) (void);
typedef void (*pMem_Read_OK) (uint8_t *pData, uint16_t Length);
typedef void (*pMem_Read_Fail) (void);

typedef struct
{
    uint8_t         Type_u8;
    uint32_t        Addr_u32;
    
    pMem_Wrtie_OK   pWriteOK;
    pMem_Wrtie_Fail pWriteFail;
    pMem_Read_OK    pReadOK;
    pMem_Read_Fail  pReadFail;
}sMemoryParameter;


extern sEvent_struct        sEventAppMem []; 
extern sAppMemVariable      sAppMem;

//extern sMemoryParameter sAppMemSubParam[]; 

extern sRecordMemoryManager     sRecTSVH;
extern sRecordMemoryManager     sRecEvent;
extern sRecordMemoryManager     sRecLog;
extern sRecordMemoryManager     sRecGPS;

/*================ Func =================*/
void        AppMem_Init(void);
uint8_t     AppMem_Task(void);

void        Appmem_Check_Rec_Init_Data (uint32_t Addr, uint8_t *pData, uint16_t Length);

void        AppMem_Erase_New_Sector (uint32_t Addr);
void        AppMem_Write_Data (uint8_t TypeData, uint8_t *pData, uint16_t Length, uint16_t MAX_LENGTH);
void        AppMem_Read_Data (uint8_t TypeData, uint8_t TypeReq, uint32_t Addr, uint16_t Length);

void        AppMem_Cb_Write_OK (uint8_t TypeData);
void        AppMem_Cb_Read_OK (uint8_t TypeData, uint32_t Addr, uint8_t TypeReq);
void        AppMem_Cb_Wrtie_ERROR (uint8_t TypeData);

void        AppMem_Cb_Erase_Chip_OK(void);
void        AppMem_Cb_Erase_Chip_Fail (void);

uint32_t    AppMem_Cb_Get_Addr (uint8_t TypeData, uint8_t RW);

void        AppMem_Process_Req_AT (uint8_t *pData, uint16_t Length);
uint8_t     AppMem_Get_Req_AT (uint8_t TypeData, uint16_t NumRec);
void        AppMem_Record_Invalid_Handle (uint8_t TypeData);

uint8_t     AppMem_Inc_Index_Send_1 (sRecordMemoryManager *sRec, uint8_t Num);
uint8_t     AppMem_Inc_Index_Send_2 (sRecordMemoryManager *sRec, uint8_t Num);
uint8_t     AppMem_Inc_Index_Save (sRecordMemoryManager *sRec);

void        AppMem_Forward_Data_To_Sim (uint8_t TypeData, uint8_t *pData, uint16_t Length);

void        AppMem_PowerOff_ExFlash (void);
void        AppMem_PowerOn_ExFlash (void);

uint8_t     AppMem_Check_New_Record (void);
void        AppMem_Check_Record_Format (uint8_t TypeData, uint8_t *pData, uint16_t Length);

void        AppMem_Init_Index_Record (void);


#endif

