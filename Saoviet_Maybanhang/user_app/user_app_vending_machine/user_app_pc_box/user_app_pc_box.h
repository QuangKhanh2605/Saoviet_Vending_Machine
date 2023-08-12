

#ifndef USER_APP_PC_BOX_H_
#define USER_APP_PC_BOX_H_

#define USING_APP_PC_BOX

#include "event_driven.h"
#include "user_util.h"
#include "user_uart.h"

typedef enum
{
    _EVENT_PC_BOX_ENTRY = 0,
    _EVENT_PC_BOX_RECEIVE_HANDLE,
    _EVENT_PC_BOX_COMPLETE_RECEIVE,
    _EVENT_PC_BOX_LOG_TSVH,
    _EVENT_PC_BOX_PING,
    _EVENT_PC_BOX_SET_DCU_ID,
    _EVENT_PC_BOX_GET_DCU_ID,
    _EVENT_WDG_STM32F4,
    _EVENT_RESET_DCU,

    _EVENT_PC_BOX_END,
}eKindEventPcBox;

extern sEvent_struct  sEventAppPcBox[];
/*================ Function ===================*/

uint8_t     AppPcBox_Task(void);
uint8_t     Log_TSVH(uint8_t *aData);
void        AppPcBox_Debug(void);
void        Init_DCU_ID(void);
#endif
