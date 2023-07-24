

#ifndef USER_APP_PC_BOX_H_
#define USER_APP_PC_BOX_H_

#include "event_driver.h"
#include "user_util.h"
#include "user_uart.h"
#include "user_define.h"
#include "user_obis.h"

typedef enum
{
    _EVENT_PC_BOX_RECEIVE_HANDLE,
    _EVENT_PC_BOX_COMPLETE_RECEIVE,

    _EVENT_PC_BOX_END,
}eKindEventPcBox;

extern sEvent_struct  sEventAppPcBox[];

/*================ Function ===================*/

uint8_t     AppPcBox_Task(void);

#endif
