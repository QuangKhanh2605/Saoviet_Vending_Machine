/*
 * siml511.h
 *
 *  Created on: Dec 13, 2023
 *      Author: chien
 */

#ifndef USER_L511_H_
#define USER_L511_H_

#include "user_util.h"
#include "main.h"
#include "user_sim_common.h"


//#define USING_TRANSPARENT_MODE     

#define CID_SERVER          0x32
#define HTTP_HEADER_MAX     3
     
#define MQTT_PROTOCOL     
//#define HTTP_PROTOCOL    
//#define HTTPS_PROTOCOL     
     
     
/*================== Var struct =====================*/
typedef enum
{
    //Cmd Pre Init
	_SIM_AT_AT,
	_SIM_AT_ECHO,
	_SIM_AT_MAN_LOG,
    //Cmd Init
    _SIM_AT_BAUD_RATE,
    _SIM_AT_GET_ID,
	_SIM_AT_CHECK_RSSI,
	_SIM_AT_GET_IMEI,
	_SIM_AT_CHECK_SIM,
    _SIM_AT_SET_APN_1,
    _SIM_AT_SET_APN_2,
    
    _SIM_AT_TEST_0,
    _SIM_AT_GET_IP_APN,
    _SIM_AT_ACTIVE_APN,
//    //SMS
//    _SIM_SMS_FORMAT_TEXT,
//    _SIM_SMS_NEW_MESS_IND,
//    _SIM_SMS_READ_UNREAD,
//    _SIM_SMS_READ_ALL,
//    _SIM_SMS_DEL_ALL,
//    //Calling
//    _SIM_CALLING_LINE,
//    //Cmd Set 2G 3G 4G
//	_SIM_NET_RA_AUTO,
//	_SIM_NET_RA_2G,
//    _SIM_NET_RA_3G,
//    _SIM_NET_RA_4G,
//    
//    _SIM_NET_CHECK_RA,   
//    _SIM_NET_CHECK_BAND,
//    _SIM_NET_CHECK_CSPI,
    //Cmd Check Network
	_SIM_NET_CEREG,
	_SIM_NET_CHECK_ATTACH,
    //Cmd Config
	_SIM_NET_SYN_TIME_ZONE,
	_SIM_NET_GET_RTC,
    //Cmd Data mode and command mode
    _SIM_TCP_OUT_DATAMODE,
    _SIM_TCP_TRANS,
    _SIM_TCP_IN_DATAMODE,
    
    _SIM_TCP_TRANS_SETUP,
    //Cmd TCP
    _SIM_TCP_GET_IP_SIM,
    _SIM_TCP_NETOPEN,
    _SIM_TCP_NETCLOSE,
    
    _SIM_TCP_CLOSE_1,
    _SIM_TCP_CLOSE_2,
	_SIM_TCP_CONNECT_1,
    _SIM_TCP_CONNECT_2,
    
    _SIM_TCP_SEND_CMD_1,
    _SIM_TCP_SEND_CMD_2,
    _SIM_TCP_SEND_CMD_3,
        
    _SIM_MQTT_CONN_1,
    _SIM_MQTT_CONN_2,
    _SIM_MQTT_SUB_1,
    _SIM_MQTT_SUB_2,
    _SIM_MQTT_PUB_1,
    _SIM_MQTT_PUB_2,
    _SIM_MQTT_PUB_FB_1,
    _SIM_MQTT_PUB_FB_2,
    
    _SIM_TCP_PING_TEST1,
    _SIM_TCP_PING_TEST2,
    //File
    _SIM_SYS_DEL_FILE_1,
    _SIM_SYS_DEL_FILE_2,
    _SIM_SYS_LIST_FILE,
    //FTP
    _SIM_FTP_SERVER_1,
    _SIM_FTP_SERVER_2,
    _SIM_FTP_PORT,
    
    _SIM_FTP_USERNAME,
    _SIM_FTP_PASSWORD,
    _SIM_FTP_TYPE,
    _SIM_FTP_LIST_FILE,
    _SIM_FTP_GET_FILE_1,
    _SIM_FTP_GET_FILE1,
    _SIM_FTP_GET_FILE2,
    _SIM_FTP_GET_FILE3,
    _SIM_FTP_READ_1,
    _SIM_FTP_READ_2,

    //Http
    _SIM_HTTP_OPEN,
    _SIM_HTTP_CLOSE,
    _SIM_HTTP_CLOSE_2,
    _SIM_HTTP_RECV_LOCAL,
    _SIM_HTTP_RECV_TE,
    _SIM_HTTP_SET_HEADER_1,
    _SIM_HTTP_SET_HEADER_2,
    _SIM_HTTP_SET_URL_1,
    _SIM_HTTP_SET_URL_2,
    _SIM_HTTP_REQ_POST_1,
    _SIM_HTTP_REQ_POST_3,
    _SIM_HTTP_REQUEST_GET,
    
    _SIM_HTTP_POST_DATA_1, 
    _SIM_HTTP_POST_DATA_2,
    _SIM_HTTP_POST_DATA_3,
    
    _SIM_HTTP_POST_DATA_EX1,
    _SIM_HTTP_POST_DATA_EX2,
    
    _SIM_HTTP_POST_DAT_END,
    _SIM_HTTP_POST_SEND,
    _SIM_HTTP_POST_SEND_2,
    _SIM_HTTP_LENGTH,
    _SIM_HTTP_READ_RES,
    
    _SIM_HTTP_READ_1,
    _SIM_HTTP_READ_2,
        
	_SIM_END,                // don't use
}AT_SIM_TypeDef;

typedef enum
{
	_SIM_URC_RESET_SIM900 = 0,
	_SIM_URC_SIM_LOST,
	_SIM_URC_SIM_REMOVE,
	_SIM_URC_CLOSED,
	_SIM_URC_PDP_DEACT,
	_SIM_URC_CALL_READY,
    _SIM_URC_CALLING,
    _SIM_URC_SMS_CONTENT,
    _SIM_URC_SMS_INDICATION1,
    _SIM_URC_SMS_INDICATION2,
	_SIM_URC_ERROR,
    _SIM_URC_RECEIV_SERVER,
	_SIM_URC_END,
}URC_SIM_TypeDef;




/*===================== extern ====================*/
extern const sCommand_Sim_Struct aSimL506Step[];
extern const sCommand_Sim_Struct aSimUrc[];

extern uint8_t aSimStepBlockInit[9];
#ifdef MQTT_PROTOCOL
    extern uint8_t aSimStepBlockNework[6];
#else
    extern uint8_t aSimStepBlockNework[12];
#endif

#ifdef USING_TRANSPARENT_MODE
    extern uint8_t aSimStepBlockConnect[13];
    extern uint8_t aSIM_STEP_PUBLISH_FB[2];
    extern uint8_t aSIM_STEP_PUBLISH[2];
#else
    #ifdef MQTT_PROTOCOL
        extern uint8_t aSimStepBlockConnect[17];
    #endif
        
    #ifdef HTTP_PROTOCOL
        extern uint8_t aSimStepBlockConnect[6];
        extern uint8_t aSimStepBlockPost[9]; 
    #endif
        
    #ifdef HTTPS_PROTOCOL
        extern uint8_t aSimStepBlockConnect[2];
        extern uint8_t aSimStepBlockPost[5]; 
    #endif
        
    extern uint8_t aSIM_STEP_PUBLISH_FB[4];
    extern uint8_t aSIM_STEP_PUBLISH[4];
#endif
    
extern uint8_t aSIM_STEP_HTTP_HEAD[2];
extern uint8_t aSIM_STEP_INFOR_CSQ[3];

#ifdef MQTT_PROTOCOL
    extern uint8_t aSimStepBlockDisConnect[4];
#else
    extern uint8_t aSimStepBlockDisConnect[1];
#endif
    
extern uint8_t aSimStepBlockGetClock[3];

extern uint8_t aSimStepBlockHttpInit[17];
extern uint8_t aSimStepBlockHttpRead[2];
extern uint8_t aSimStepBlockSMS[2]; 

/*================== Function ==========================*/
uint8_t     L511_Check_Step_Long_Timeout (uint8_t sim_step);
sCommand_Sim_Struct * L511_Get_AT_Cmd (uint8_t step);
uint32_t    L511_Get_Delay_2_AT (uint8_t Step);
uint8_t     L511_Is_Step_Check_URC (uint8_t sim_step);

uint8_t     L511_Check_Step_Skip_Error (uint8_t step);

#endif /* SIM_MODULE_SIML511_SIML511_H_ */
