


#ifndef USER_MODEM_INIT_H
#define USER_MODEM_INIT_H

#include "user_util.h"




/*================ Func ================== */
void 		Init_Memory_Infor(void);

void 		Init_DCU_ID(void);
void 		Save_DCU_ID(void);

void        Init_Timer_Send (void);
void        Save_Freq_Send_Data (void);

uint16_t    mInit_u16 (uint32_t Addr, uint16_t MAX);
void        mSave_u16 (uint32_t Addr, uint16_t Val);


#endif