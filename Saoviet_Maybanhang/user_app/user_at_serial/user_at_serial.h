#ifndef USER_AT_SERIAL_H__
#define USER_AT_SERIAL_H__

#include "user_util.h"
#include "usart.h"

/*======================== Define ======================*/

    
/*======================== Structs var======================*/

typedef enum
{
    _RESET_DCU = 0,
    _GET_SERI_DCU,
    _SET_SERI_DCU,
    
    _GET_SETUP_TEMP,
    _SET_SETUP_TEMP,
    
    _GET_THRESH_TEMP,
    _SET_THRESH_TEMP,
    
    _END_AT_CMD,
}eNumTypeCommandAT;

typedef void (*_func_callback_f)(sData *str_Receiv, uint16_t Pos);

typedef struct {
	int 			    idStep;
	_func_callback_f	CallBack;
	sData	    sTempReceiver;				// temp Receive <- Sim900
} struct_CheckList_AT;

/*======================== External Var struct ======================*/
uint8_t     Check_AT_User(sData *StrUartRecei, uint8_t Type);

void        _fRESET_DCU(sData *strRecei, uint16_t Pos);
void        _fGET_SERI_DCU (sData *strRecei, uint16_t Pos);
void        _fSET_SERI_DCU (sData *strRecei, uint16_t Pos);

void        _fGET_SETUP_TEMP (sData *strRecei, uint16_t Pos);
void        _fSET_SETUP_TEMP (sData *strRecei, uint16_t Pos);

void        _fGET_THRESH_TEMP (sData *strRecei, uint16_t Pos);
void        _fSET_THRESH_TEMP (sData *strRecei, uint16_t Pos);

#endif  
