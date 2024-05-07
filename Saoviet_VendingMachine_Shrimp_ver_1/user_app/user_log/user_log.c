
#include "user_log.h"
#include "user_define.h"
#include "queue_p.h"
#include "user_modem.h"


/*==================var struct======================*/


/*==================Function======================*/
/*
    Func: Luu vao flash noi
*/
uint8_t LOG_Save_Record (uint8_t *pData, uint16_t Length)
{
    uint8_t aTEMP[128] = {0};
    uint8_t i = 0, Count = 0, TempCrc = 0;
    uint8_t aTEMP_NUM[10] = {0};
    sData  strNum = {&aTEMP_NUM[0], 0};

    if (Length >= 40)
        Length = 40;

    //Dong goi theo 1.ST.Log
#ifdef USING_APP_MEM
    Convert_Uint64_To_StringDec(&strNum, sRecLog.IndexSave_u16, 0);
#endif
    for (i = 0; i < strNum.Length_u16; i++)
        aTEMP[Count++] = *(strNum.Data_a8 + i);

    aTEMP[Count++] = '.';
//    aTEMP[Count++] = sRTC.year/10 + 0x30;
//    aTEMP[Count++] = sRTC.year%10 + 0x30;
//    aTEMP[Count++] = '/';
    aTEMP[Count++] = sRTC.month/10 + 0x30;
    aTEMP[Count++] = sRTC.month%10 + 0x30;
    aTEMP[Count++] = '/';
    aTEMP[Count++] = sRTC.date/10 + 0x30;
    aTEMP[Count++] = sRTC.date%10 + 0x30;
    aTEMP[Count++] = ' ';
    aTEMP[Count++] = sRTC.hour/10 + 0x30;
    aTEMP[Count++] = sRTC.hour%10 + 0x30;
    aTEMP[Count++] = ':';
    aTEMP[Count++] = sRTC.min/10 + 0x30;
    aTEMP[Count++] = sRTC.min%10 + 0x30;
    aTEMP[Count++] = ':';
    aTEMP[Count++] = sRTC.sec/10 + 0x30;
    aTEMP[Count++] = sRTC.sec%10 + 0x30;

    aTEMP[Count++] = '.';   //9 byte: 64 - (3 + 15 + 1 + 1)- 4 = 40

    for (i = 0; i < Length; i++)
        aTEMP[Count++] = *(pData + i);
    
    Count++;
	for (i = 0; i < (Count - 1); i++)
		TempCrc ^= aTEMP[i];

    aTEMP[Count-1] = TempCrc;

#ifdef USING_APP_MEM
    AppMem_Write_Data(_MEM_DATA_LOG, &aTEMP[0], Count, sRecLog.SizeRecord_u16);
#endif
    
    return 1;
}











