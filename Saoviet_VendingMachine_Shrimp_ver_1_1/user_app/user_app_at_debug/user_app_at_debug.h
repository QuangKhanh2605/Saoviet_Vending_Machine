#ifndef USER_APP_AT_DEBUG_H__
#define USER_APP_AT_DEBUG_H__

#define USING_APP_AT_DEBUG

#include "event_driven.h"
#include "user_util.h"
#include "user_uart.h"


/*================== Define ====================*/

/*================= Struct Var ================*/
typedef enum
{
    _EVENT_AT_DEBUG_ENTRY = 0,
    _EVENT_AT_DEBUG_RECEIVE_HANDLE,
    _EVENT_AT_DEBUG_COMPLETE_RECEIVE,

    _EVENT_AT_DEBUG_END,
}eKindEventAtDebug;

extern sEvent_struct  sEventAppAtDebug[];

/*================== Function Handle ===============*/
uint8_t     AppAtDebug_Task(void);

#endif


