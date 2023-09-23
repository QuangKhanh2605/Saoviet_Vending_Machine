#ifndef USER_AT_SERIAL_H__
#define USER_AT_SERIAL_H__

#include "user_util.h"
#include "usart.h"

/*======================== Define ======================*/

    
/*======================== Structs var======================*/

typedef enum
{
    _RESET_DCU = 0,
    _RESET_PCBOX,
    
    _APP_FIX_MOTOR,
    _APP_PUSH_MOTOR,
      
    _GET_SERI_DCU,
    _SET_SERI_DCU,
    
    _GET_TIME_TSVH,
    _SET_TIME_TSVH,
    
    _GET_TIME_PCBOX_RESET,
    _SET_TIME_PCBOX_RESET,
    
    _GET_SETUP_TEMP,
    _SET_SETUP_TEMP,
    
    _GET_THRESH_TEMP,
    _SET_THRESH_TEMP,
    
    _GET_ID_SLAVE,
    _SET_ID_SLAVE,
    
    _CTRL_RELAY_SCREEN,
    _CTRL_RELAY_LAMP,
    _CTRL_RELAY_WARM,
    
    _GET_TIME_WARM_RUN,
    _SET_TIME_WARM_RUN,
    
    _GET_TIME_WARM_WAIT,
    _SET_TIME_WARM_WAIT,
    
    _GET_USING_CRC,
    _SET_USING_CRC,
    
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

void        _fRESET_DCU (sData *strRecei, uint16_t Pos);
void        _fRESET_PCBOX (sData *strRecei, uint16_t Pos);

void        _fAPP_FIX_MOTOR (sData *strRecei, uint16_t Pos);
void        _fAPP_PUSH_MOTOR (sData *strRecei, uint16_t Pos);

void        _fGET_TIME_TSVH (sData *strRecei, uint16_t Pos);
void        _fSET_TIME_TSVH (sData *strRecei, uint16_t Pos);

void        _fGET_SERI_DCU (sData *strRecei, uint16_t Pos);
void        _fSET_SERI_DCU (sData *strRecei, uint16_t Pos);

void        _fGET_TIME_PCBOX_RESET (sData *strRecei, uint16_t Pos);
void        _fSET_TIME_PCBOX_RESET (sData *strRecei, uint16_t Pos);

void        _fGET_SETUP_TEMP (sData *strRecei, uint16_t Pos);
void        _fSET_SETUP_TEMP (sData *strRecei, uint16_t Pos);

void        _fGET_THRESH_TEMP (sData *strRecei, uint16_t Pos);
void        _fSET_THRESH_TEMP (sData *strRecei, uint16_t Pos);

void        _fGET_ID_SLAVE (sData *strRecei, uint16_t Pos);
void        _fSET_ID_SLAVE (sData *strRecei, uint16_t Pos);

void        _fCTRL_RELAY_SCREEN (sData *strRecei, uint16_t Pos);
void        _fCTRL_RELAY_LAMP (sData *strRecei, uint16_t Pos);
void        _fCTRL_RELAY_WARM (sData *strRecei, uint16_t Pos);

void        _fGET_TIME_WARM_RUN (sData *strRecei, uint16_t Pos);
void        _fSET_TIME_WARM_RUN (sData *strRecei, uint16_t Pos);

void        _fGET_TIME_WARM_WAIT (sData *strRecei, uint16_t Pos);
void        _fSET_TIME_WARM_WAIT (sData *strRecei, uint16_t Pos);

void        _fGET_USING_CRC (sData *strRecei, uint16_t Pos);
void        _fSET_USING_CRC (sData *strRecei, uint16_t Pos);
#endif  
