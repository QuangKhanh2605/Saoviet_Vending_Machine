/*
    8/2021
    Thu vien xu ly Uart
*/
#include "user_define.h"

#include "user_at_serial.h"
#include "user_string.h"
#include "onchipflash.h"

/*======================== Structs var======================*/

const struct_CheckList_AT CheckList_AT_CONFIG[] =
{
//			id ENUM				  kind process			     str Received
		{_SET_DEV_SERIAL, 		_fSET_DEV_SERIAL,		    {(uint8_t*)"at+seri=",8}},  //ex:
		{_QUERY_DEV_SERIAL,		_fQUERY_DEV_SERIAL,		    {(uint8_t*)"at+seri?",8}},
        
        {_QUERY_SIM_ID,		    _fQUERY_SIM_ID,		        {(uint8_t*)"at+simseri?",11}},
        
		{_SET_SERVER_MAIN,	 	_fSET_SERVER_MAIN_INFO,	    {(uint8_t*)"at+mqttmain=",12}},     //Ex: at+mqttmain=124.158.5.135,1883,admin,admin
		{_QUERY_SERVER_MAIN, 	_fQUERY_SERVER_MAIN_INFO,	{(uint8_t*)"at+mqttmain?",12}},
        
        {_SET_SERVER_BACKUP,	_fSET_SERVER_BACKUP_INFO,	{(uint8_t*)"at+mqttbackup=",14}},   //Ex: at+mqttbackup=124.158.5.135,1883,admin,admin
		{_QUERY_SERVER_BACKUP, 	_fQUERY_SERVER_BACKUP_INFO,	{(uint8_t*)"at+mqttbackup?",14}},
        
        {_REQ_SWITCH_SERVER, 	_fREQ_SWITCH_SERVER,	    {(uint8_t*)"at+swserver",11}},

        {_SET_DUTY_READ_DATA,	_fSET_DUTY_READ_DATA,	    {(uint8_t*)"at+duty=",8}},          //ex: at+duty=03x30 at+duty=03x30
        {_QUERY_DUTY_READ_DATA,	_fQUERY_DUTY_READ_DA,	    {(uint8_t*)"at+duty?",8}},
        
        {_SET_FREQ_ONLINE,	    _fSET_FREQ_ONLINE,	        {(uint8_t*)"at+freqmin=",11}},      // at+freqmin=5
        {_QUERY_FREQ_ONLINE,	_fQUERY_FREQ_ONLINE,	    {(uint8_t*)"at+freqmin?",11}},
        
        {_SET_DEFAULT_PULSE, 	_fSET_DATAPULSE_DEFAUL,	    {(uint8_t*)"at+resetpulse?",14}},
        {_SET_SAVE_BOX, 	    _fSET_SAVE_BOX,	            {(uint8_t*)"at+savebox?",11}},      //ex:at+savebox
        {_SET_NUM_WAKEUP, 	    _fSET_NUM_WAKEUP_DEV,	    {(uint8_t*)"at+numwake=",11}},     //ex: at+numwake=05
        {_READ_OLD_DATA, 	    _fREAD_OLD_DATA,	        {(uint8_t*)"at+readlog=",11}},     //ex: at+numwake=05
        
        {_SET_FLOW_THRESH, 		_fSET_FLOW_THRESH,		    {(uint8_t*)"at+flowthresh=",14}},  // at+flowthresh=1234,12345
		{_QUERY_FLOW_THRESH, 	_fQUERY_FLOW_THRESH,	    {(uint8_t*)"at+flowthresh?",14}},
        
        {_SET_QUAN_THRESH, 	    _fSET_QUAN_THRESH,		    {(uint8_t*)"at+quanthresh=",14}},   // at+quanthresh=1234,12345
		{_QUERY_QUAN_THRESH, 	_fQUERY_QUAN_THRESH,	    {(uint8_t*)"at+quanthresh?",14}},
        
        {_SET_BAT_THRESH, 		_fSET_BAT_THRESH,		    {(uint8_t*)"at+batthresh=",13}},    // at+batthresh=12
		{_QUERY_BAT_THRESH, 	_fQUERY_BAT_THRESH,	        {(uint8_t*)"at+batthresh?",13}},
        
        {_SET_PULSE_VALUE, 		_fSET_PULSE_VALUE,		    {(uint8_t*)"at+pulse=",9}},         // at+pulse=20
        {_QUERY_PULSE_VALUE, 	_fQUERY_PULSE_VALUE,        {(uint8_t*)"at+pulse?",9}},
        
        {_SET_RTC_TIME, 		_fSET_RTC_TIME,			    {(uint8_t*)"at+rtc_time=",12}}, 
        {_SET_RTC_DATE, 		_fSET_RTC_DATE,			    {(uint8_t*)"at+rtc_date=",12}},
        {_SET_RTC, 		        _fSET_RTC,			        {(uint8_t*)"at+rtc=",7}},
        {_QUERY_RTC, 			_fQUERY_RTC,			    {(uint8_t*)"at+rtc?",7}},
        
        {_SET_REQUEST_STIME,    _fSET_RF_RQ_STIME,			{(uint8_t*)"at+stime?",9}},

        {_RESET_MODEM, 	        _fRESET_MODEM,              {(uint8_t*)"at+mreset?",10}},
        {_ERASE_MEMORY, 	    _fERASE_MEMORY,             {(uint8_t*)"at+erasemem?",12}},
        
        {_QUERY_FIRM_VER,       _fQUERY_FIRM_VER,	        {(uint8_t*)"at+fwversion?",13}},    
        {_QUERY_UP_FIRM_HTTP,   _fGET_URLHTTP_FIRM,	        {(uint8_t*)"at+userurl=",11}},      
        {_QUERY_UP_FIRM_FTP,    _fGET_URLFTP_FIRM,	        {(uint8_t*)"at+userftp=",11}},
        
        {_QUERY_INDEX_LOG,      _fQUERY_INDEX_LOG,	        {(uint8_t*)"at+logindex?",12}},     //Vi tri index log
        {_TEST_LOG,             _fTEST_LOG,	                {(uint8_t*)"at+testlog?",11}},      //Ghi vao 1 ban tin log
        {_QUERY_READ_ALL_LOG,   _fQUERY_ALL_LOG,	        {(uint8_t*)"at+readlog?",11}},      //Doc tat ca cac log
        {_QUERY_READ_LAST_LOG,  _fGET_LAST_LOG,	    	    {(uint8_t*)"at+rdlastlog?",13}},     //Doc n long gan nhat
        {_QUERY_LAST_FOR_LOG,   _fGET_LAST_FOR_LOG,	        {(uint8_t*)"at+rdlastforlog=",16}},  // at+rdlastforlog=(200)
        
        {_QUERY_LAST_FOR_OPERA, _fGET_LAST_FOR_OPERA,	    {(uint8_t*)"at+rdlastforopera=",18}}, // at+rdlastforopera=(200)
        {_QUERY_LAST_FOR_EVENT, _fGET_LAST_FOR_EVENT,	    {(uint8_t*)"at+rdlastforevent=",18}}, 
        
        {_SET_CALIB_LEVEL,      _fSET_CALIB_LEVEL,	        {(uint8_t*)"at+levelcalib=",14}},   //at+levelcalib=-14
        {_QUERY_CALIB_LEVEL,    _fQUERY_CALIB_LEVEL,	    {(uint8_t*)"at+levelcalib?",14}}, 
        
        {_SET_LEVEL_THRESH, 	_fSET_LEVEL_THRESH,		    {(uint8_t*)"at+levelthresh=",15}},  // at+levelthresh=1234,12345
		{_QUERY_LEVEL_THRESH, 	_fQUERY_LEVEL_THRESH,	    {(uint8_t*)"at+levelthresh?",15}},
        
        {_QUERY_GPS_LOC,        _fQUERY_GPS_LOC,			{(uint8_t*)"at+gps?",7}},
        {_QUERY_LAST_FOR_GPS,   _fGET_LAST_FOR_GPS,	        {(uint8_t*)"at+rdlastforgps=",16}}, 
        
        {_SET_LIST_ID_SLAVE,	_fSET_LIST_ID_SLAVE,	    {(uint8_t*)"at+slave=",9}},
        {_QUERY_LIST_ID_SLAVE,	_fQUERY_LIST_ID_SLAVE,	    {(uint8_t*)"at+slave?",9}}, 
        
        {_SET_EX_BAT_THRESH,	_fSET_EX_BAT_THRESH,	    {(uint8_t*)"at+exbatthresh=",15}},
        {_QUERY_EX_BAT_THRESH,	_fQUERY_EX_BAT_THRESH,	    {(uint8_t*)"at+exbatthresh?",15}},
        
        {_SET_POWER_ON_TEMH,	_fSET_POWER_ON_TEMH,	    {(uint8_t*)"at+ontemh?",10}},
        {_SET_POWER_OFF_TEMH,	_fSET_POWER_OFF_TEMH,	    {(uint8_t*)"at+offtemh?",11}},
        
        {_RESET_DCU,            _fRESET_DCU,                {(uint8_t*)"at+resetdcu",11}},      //Ex: "at+resetdcu"
        
        {_RESET_PCBOX,          _fRESET_PCBOX,              {(uint8_t*)"at+resetpcbox",13}},    //Ex: "at+resetpcbox"
        {_ON_PCBOX,             _fON_PCBOX,                 {(uint8_t*)"at+onpcbox=",11}},       //Ex: "at+onpcbox"
        {_OFF_PCBOX,            _fOFF_PCBOX,                {(uint8_t*)"at+offpcbox=",12}},      //Ex: "at+offpcbox"
        
        {_APP_FIX_MOTOR,        _fAPP_FIX_MOTOR,            {(uint8_t*)"at+fixmotor=",12}},     //Ex: "at+fixmotor=1"
        {_APP_PUSH_MOTOR,       _fAPP_PUSH_MOTOR,           {(uint8_t*)"at+pushmotor=",13}},     //Ex: "at+purchase=1 1"
        
        {_GET_TIME_TSVH,		_fGET_TIME_TSVH,		    {(uint8_t*)"at+timetsvh?",12}},     //Ex: "at+timetsvh?"
        {_SET_TIME_TSVH,        _fSET_TIME_TSVH,            {(uint8_t*)"at+timetsvh=",12}},     //Ex: "at+timetsvh=1" (min)
        
        {_GET_SETUP_TEMP,       _fGET_SETUP_TEMP,           {(uint8_t*)"at+setuptemp?",13}},    //Ex: "at+setuptemp?"        
        {_SET_SETUP_TEMP,       _fSET_SETUP_TEMP,           {(uint8_t*)"at+setuptemp=",13}},    //Ex: "at+setuptemp=50"(Scale 0x00)
        
        {_GET_THRESH_TEMP,      _fGET_THRESH_TEMP,          {(uint8_t*)"at+threshtemp?",14}},   //Ex: "at+threshtemp?"
        {_SET_THRESH_TEMP,      _fSET_THRESH_TEMP,          {(uint8_t*)"at+threshtemp=",14}},   //Ex: "at+threshtemp=25"(Scale 0x00)
        
        {_GET_ID_SLAVE,         _fGET_ID_SLAVE,             {(uint8_t*)"at+slaveid?",11}},      //Ex: "at+slaveid?"
        {_SET_ID_SLAVE,         _fSET_ID_SLAVE,             {(uint8_t*)"at+slaveid=",11}},      //Ex: "at+slaveid=1"
        
        {_CTRL_RELAY_SCREEN,    _fCTRL_RELAY_SCREEN,        {(uint8_t*)"at+rlscreen=",12}},     //Ex: "at+rlscreen=1" (0:Off, 1:On)
        {_CTRL_RELAY_LAMP,      _fCTRL_RELAY_LAMP,          {(uint8_t*)"at+rllamp=",10}},       //Ex: "at+rllamp=1" (0:Off, 1:On)
        {_CTRL_RELAY_WARM,      _fCTRL_RELAY_WARM,          {(uint8_t*)"at+rlwarm=",10}},       //Ex: "at+rlwarm=1" (0:Off, 1:On)
        
        {_GET_TIME_WARM_RUN,    _fGET_TIME_WARM_RUN,        {(uint8_t*)"at+timewarmrun?",15}},  //Ex: "at+timewarmrun?"
        {_SET_TIME_WARM_RUN,    _fSET_TIME_WARM_RUN,        {(uint8_t*)"at+timewarmrun=",15}},  //Ex: "at+timewarmrun=5" (Min)
        
        {_GET_TIME_WARM_WAIT,   _fGET_TIME_WARM_WAIT,       {(uint8_t*)"at+timewarmwait?",16}}, //Ex: "at+timewarmwait?"
        {_SET_TIME_WARM_WAIT,   _fSET_TIME_WARM_WAIT,       {(uint8_t*)"at+timewarmwait=",16}}, //Ex: "at+timewarmwait=15" (Min)
        
        {_GET_USING_CRC,        _fGET_USING_CRC,            {(uint8_t*)"at+usingcrc?",12}},     //Ex: "at+usingcrc?"
        {_SET_USING_CRC,        _fSET_USING_CRC,            {(uint8_t*)"at+usingcrc=",12}},     //Ex: "at+usingcrc=1" (Using: 1; UnUsing = 0)
        
        {_GET_DEBUG_TEMP,       _fGET_DEBUG_TEMP,           {(uint8_t*)"at+temperature?",15}},  //Ex: "at+temperature?"
        {_CALIB_TEMPERATURE,    _fCALIB_TEMPERATURE,        {(uint8_t*)"at+calibtemp=",13}},
        
        {_GET_DEBUG_ELECTRIC,   _fGET_DEBUG_ELECTRIC,       {(uint8_t*)"at+electric?",12}},     //Ex: "at+electric?"
        
        {_GET_CYCLE_ON_OFF_PC,  _fGET_CYCLE_ON_OFF_PC,      {(uint8_t*)"at+cyclepcbox?",14}},   //Ex: "at+cyclepcbox?"
        {_SET_CYCLE_ON_OFF_PC,  _fSET_CYCLE_ON_OFF_PC,      {(uint8_t*)"at+cyclepcbox=",14}},   //Ex: "at+cyclepcbox=1,22:22,7:10"
        
        {_GET_STATE_PCBOX,      _fGET_STATE_PCBOX,          {(uint8_t*)"at+statepcbox",13}},
        
        {_TEST_ELEVATOR,        _fTEST_ELEVATOR,            {(uint8_t*)"at+elevator",11}},
        {_GET_CURRENT_WEIGHT,   _fGET_CURRENT_WEIGHT,       {(uint8_t*)"at+weight?",10}},
        
        {_TEST_LOCK_MAGNETIS,   _fTEST_LOCK_MAGNETIS,       {(uint8_t*)"at+lock=",8}},
        {_CHECK_TOP_ELEVATOR,   _fCHECK_TOP_ELEVATOR,       {(uint8_t*)"at+topelevator?",15}},
        {_CHECK_BOT_ELEVATOR,   _fCHECK_BOT_ELEVATOR,       {(uint8_t*)"at+botelevator?",15}},
        {_CHECK_LOCK_MAGNETIS,  _fCHECK_LOCK_MAGNETIS,      {(uint8_t*)"at+lockmagnetis?",16}},
        
        {_CTRL_ELEVATOR_FLOOR,  _fELEVATOR_FLOOR,           {(uint8_t*)"at+floor=",9}},
        
        {_END_AT_CMD,	        NULL,	                    {(uint8_t*)"at+end",6}},
};

uint8_t 		PortConfig = 0;
uint8_t 		aDATA_CONFIG[128];

/*======================== Function ======================*/

void _fSET_DEV_SERIAL(sData *strRecei, uint16_t Pos)
{
    uint16_t i=0;
    uint8_t Fix = 0;

    if (PortConfig == 2)
    	Fix = 1;

    if (strRecei->Length_u16 <= (Pos + 20 + Fix))   					//1 byte crc phia sau neu dung server
    {
        DCU_Respond (PortConfig, (uint8_t *)"\r\nOK", 4, 1);

        //Must memset buff old ID. because avoid error char*
        UTIL_MEM_set(&aDCU_ID, 0, sizeof(aDCU_ID));
        sModem.sDCU_id.Length_u16 = 0;
        
        for (i = Pos; i < (strRecei->Length_u16 - Fix); i++)
        {
            if (sModem.sDCU_id.Length_u16 >= sizeof(aDCU_ID))
                break;
            
        	sModem.sDCU_id.Data_a8[sModem.sDCU_id.Length_u16++] = *(strRecei->Data_a8 + i);
        }
        //Save
        Save_DCU_ID();

        Reset_Chip_Immediately();
    } else
    	DCU_Respond(PortConfig, (uint8_t *)"\r\nFAIL LENGTH!", 14, 0);
}


void _fQUERY_DEV_SERIAL(sData *strRecei, uint16_t Pos)
{
    DCU_Respond(PortConfig, sModem.sDCU_id.Data_a8, sModem.sDCU_id.Length_u16, 0);
}

void _fQUERY_SIM_ID(sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_SIM
    DCU_Respond(PortConfig, sSimInfor.sSIMid.Data_a8, sSimInfor.sSIMid.Length_u16, 0);
#endif
}



// Ex: at+mqtt=124.158.5.135,1883,admin,admin
void _fSET_SERVER_MAIN_INFO (sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_SIM
	sData	sIPGet = {NULL, 0};
	sData	sPortGet = {NULL, 0};
	sData	sUserGet = {NULL, 0};
	sData	sPassGet = {NULL, 0};

    if (strRecei->Length_u16 > 64)
    {
        DCU_Respond(PortConfig,(uint8_t *)"ERROR", 5, 0);
        return;
    }
    //Chay qua khoang trang (neu co)
    for (Pos = 0; Pos < strRecei->Length_u16; Pos++)
        if (*(strRecei->Data_a8 + Pos) != ' ')   //khac khoang trang thi dung lai
            break;

    *(strRecei->Data_a8 + 63)  = ','; 						// Mark finish
    //Get IP
    sIPGet.Data_a8 = strRecei->Data_a8 + Pos;
    //Tinh length
	while (*(strRecei->Data_a8 + Pos++) != ',')
	{
		sIPGet.Length_u16++;
		if (Pos > strRecei->Length_u16)
			break;
	}
	//Get Port
	sPortGet.Data_a8 = strRecei->Data_a8 + Pos;
	if (Pos < 64)
	{
		while (*(strRecei->Data_a8 + Pos++) != ',')
		{
			sPortGet.Length_u16++;
			if (Pos > strRecei->Length_u16)
				break;
		}
	}
	//Get user
	sUserGet.Data_a8 = strRecei->Data_a8 + Pos;
	if (Pos < 64)
	{
		while (*(strRecei->Data_a8 + Pos++) != ',')
		{
			sUserGet.Length_u16++;
			if (Pos > strRecei->Length_u16)
				break;
		}
	}
	//Get Pass
	sPassGet.Data_a8 = strRecei->Data_a8 + Pos;
    if (Pos < 64)
    {
        while (Pos < strRecei->Length_u16)
        {
        	sPassGet.Length_u16++;
            Pos++;
        }
    }
    //Check xem co nhan Cau hinh k
    if ((sIPGet.Length_u16 > 0) && (sIPGet.Length_u16 < MAX_IP_LENGTH)
    		&& (sPortGet.Length_u16 > 0) && (sPortGet.Length_u16 < MAX_PORT_LENGTH))
    {
    	if (sUserGet.Length_u16 > MAX_USER_PASS_LENGTH)
    		sUserGet.Length_u16 = MAX_USER_PASS_LENGTH;
        
    	if (sPassGet.Length_u16 > MAX_USER_PASS_LENGTH)
    		sPassGet.Length_u16 = MAX_USER_PASS_LENGTH;
        
        Sim_Set_Server_Infor_Main (sIPGet, sPortGet, sUserGet, sPassGet);
    	//Set IP
		//Luu vao flash
		Save_Server_Infor();
		DCU_Respond(PortConfig,(uint8_t *)"OK", 2, 0);
		return;
    }

    DCU_Respond(PortConfig,(uint8_t *)"ERROR", 5, 0);
#endif
}

void _fQUERY_SERVER_MAIN_INFO (sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_SIM
	uint16_t Length = 0;
	uint8_t  aTEMP[64] = {0};

	Length = Sim_Get_Server_Infor_Main (&aTEMP[0]);
	//Print
	DCU_Respond(PortConfig, aTEMP, Length, 0);
#endif
}



// Ex: at+mqttbackup=124.158.5.135,1883,admin,admin
void _fSET_SERVER_BACKUP_INFO (sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_SIM
	sData	sIPGet = {NULL, 0};
	sData	sPortGet = {NULL, 0};
	sData	sUserGet = {NULL, 0};
	sData	sPassGet = {NULL, 0};

    if (strRecei->Length_u16 > 64)
    {
        DCU_Respond(PortConfig,(uint8_t *)"ERROR", 5, 0);
        return;
    }
    //Chay qua khoang trang (neu co)
    for (Pos = 0; Pos < strRecei->Length_u16; Pos++)
        if (*(strRecei->Data_a8 + Pos) != ' ')   //khac khoang trang thi dung lai
            break;

    *(strRecei->Data_a8 + 63)  = ','; 						// Mark finish
    //Get IP
    sIPGet.Data_a8 = strRecei->Data_a8 + Pos;
    //Tinh length
	while (*(strRecei->Data_a8 + Pos++) != ',')
	{
		sIPGet.Length_u16++;
		if (Pos > strRecei->Length_u16)
			break;
	}
	//Get Port
	sPortGet.Data_a8 = strRecei->Data_a8 + Pos;
	if (Pos < 64)
	{
		while (*(strRecei->Data_a8 + Pos++) != ',')
		{
			sPortGet.Length_u16++;
			if (Pos > strRecei->Length_u16)
				break;
		}
	}
	//Get user
	sUserGet.Data_a8 = strRecei->Data_a8 + Pos;
	if (Pos < 64)
	{
		while (*(strRecei->Data_a8 + Pos++) != ',')
		{
			sUserGet.Length_u16++;
			if (Pos > strRecei->Length_u16)
				break;
		}
	}
	//Get Pass
	sPassGet.Data_a8 = strRecei->Data_a8 + Pos;
    if (Pos < 64)
    {
        while (Pos < strRecei->Length_u16)
        {
        	sPassGet.Length_u16++;
            Pos++;
        }
    }
    //Check xem co nhan Cau hinh k
    if ((sIPGet.Length_u16 > 0) && (sIPGet.Length_u16 < MAX_IP_LENGTH)
    		&& (sPortGet.Length_u16 > 0) && (sPortGet.Length_u16 < MAX_PORT_LENGTH))
    {
    	if (sUserGet.Length_u16 > MAX_USER_PASS_LENGTH)
    		sUserGet.Length_u16 = MAX_USER_PASS_LENGTH;
        
    	if (sPassGet.Length_u16 > MAX_USER_PASS_LENGTH)
    		sPassGet.Length_u16 = MAX_USER_PASS_LENGTH;

        Sim_Set_Server_Infor_Backup (sIPGet, sPortGet, sUserGet, sPassGet);
		//Luu vao flash
		Save_Server_BackUp_Infor();
		DCU_Respond(PortConfig,(uint8_t *)"OK", 2, 0);
		return;
    }

    DCU_Respond(PortConfig,(uint8_t *)"ERROR", 5, 0);
#endif
}

void _fQUERY_SERVER_BACKUP_INFO (sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_SIM
	uint16_t Length = 0;
	uint8_t  aTEMP[64] = {0};

	Length = Sim_Get_Server_Infor_Backup (&aTEMP[0]);
	//Print
	DCU_Respond(PortConfig, aTEMP, Length, 0);
#endif
}

void _fREQ_SWITCH_SERVER (sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_SIM
    switch(sSimVar.ModeConnectNow_u8)  
    {
        case MODE_CONNECT_DATA_MAIN:
            sSimVar.ModeConnectNow_u8 = MODE_CONNECT_DATA_BACKUP;
            break;
        case MODE_CONNECT_DATA_BACKUP:
            sSimVar.ModeConnectNow_u8 = MODE_CONNECT_DATA_MAIN;
            break;
        default:
            DCU_Respond(PortConfig,(uint8_t *)"ERROR", 5, 0);
            return;
    }
    
    DCU_Respond(PortConfig,(uint8_t *)"OK", 2, 0);
    //Khoi dong lai module sim
    sSimCommon.PowerStatus_u8 = _POWER_START;
    AppSim_Start_Module_Sim();
#endif
}

// at+duty=06x60
void _fSET_DUTY_READ_DATA(sData *strRecei, uint16_t Pos)
{
    uint16_t TempFre = 0;
    uint8_t TempNum = 0;

    if (strRecei->Length_u16 < (Pos + 5))
    {
        DCU_Respond(PortConfig, (uint8_t *)"\r\nFAIL LENGTH", 13, 0);
        return;
    }
    TempNum = *(strRecei->Data_a8 + Pos++) - 0x30;
    TempNum = TempNum * 10 + *(strRecei->Data_a8 + Pos++) - 0x30;
    Pos++;
    TempFre = *(strRecei->Data_a8 + Pos++) - 0x30;
    TempFre = TempFre * 10 + *(strRecei->Data_a8 + Pos++) - 0x30;

    if (((TempNum > 0) && (TempNum <= MAX_NUMBER_WAKE_UP)) && (TempFre <=60))
    {
        sFreqInfor.NumWakeup_u8 = TempNum;
        sFreqInfor.FreqWakeup_u32 = TempFre;
            
        Save_Freq_Send_Data();
        //Set lai timer
        AppComm_Set_Next_TxTimer();
        DCU_Respond (PortConfig, (uint8_t *)"\r\nOK", 4, 1); 
    } else
    	DCU_Respond(PortConfig, (uint8_t *)"\r\nERROR", 7, 0);
}




void _fQUERY_DUTY_READ_DA(sData *strRecei, uint16_t Pos)
{
    uint8_t     length = 0;
    uint8_t     aTemp[12] = {0};

    aTemp[length++] = 'f';
    aTemp[length++] = 'r';
    aTemp[length++] = 'e';
    aTemp[length++] = 'q';
    aTemp[length++] = '=';
    aTemp[length++] = sFreqInfor.NumWakeup_u8 / 10 + 0x30;
    aTemp[length++] = sFreqInfor.NumWakeup_u8 % 10 + 0x30;
    aTemp[length++] = 'x';
    aTemp[length++] = (sFreqInfor.FreqWakeup_u32  / 10) % 10 + 0x30;
    aTemp[length++] = sFreqInfor.FreqWakeup_u32  % 10 + 0x30;

    DCU_Respond(PortConfig, &aTemp[0], length, 0);
}


void _fSET_FREQ_ONLINE(sData *strRecei, uint16_t Pos)
{
    uint16_t    Posfix = Pos;
    uint32_t    TempValue = 0;
	uint8_t		Temphex = 0;
	//
	while (Posfix < strRecei->Length_u16)
	{
        if ((*(strRecei->Data_a8 + Posfix) >= 0x30) && (*(strRecei->Data_a8 + Posfix) <= 0x39))   //Gap so thi dung lai 
            break;
        
        Posfix++;
	}

    while (Posfix < strRecei->Length_u16)
	{
		Temphex = *(strRecei->Data_a8 + Posfix);
		if ((Temphex < '0') || (Temphex > '9') || (Posfix >= strRecei->Length_u16))
			break;
        
		TempValue = TempValue * 10 + Temphex - 0x30;
		Posfix++;
	}  
    
    sFreqInfor.FreqSendOnline_u32 = TempValue;
    //Luu lai
    Save_Freq_Send_Data ();
    //Set lai timer
    AppComm_Set_Next_TxTimer();
    DCU_Respond(PortConfig, (uint8_t *)"\r\nOK!", 5, 0);
}


void _fQUERY_FREQ_ONLINE(sData *strRecei, uint16_t Pos)
{
    uint8_t aTemp[20] = "at+freqmin:";   //13 ki tu dau tien
    sData StrResp = {&aTemp[0], 11}; 

    Convert_Uint64_To_StringDec (&StrResp, (uint64_t) (sFreqInfor.FreqSendOnline_u32), 0);

	DCU_Respond(PortConfig, StrResp.Data_a8, StrResp.Length_u16, 0);
}


void _fSET_DATAPULSE_DEFAUL(sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_WM
    sPulse.Number_u32 = 0;
    //cho phep gui ban tin data len ngay lap tuc
    DCU_Respond(PortConfig, (uint8_t *)"\r\nRESET SO NUOC OK!", 19, 0);
#endif
}


void _fSET_NUM_WAKEUP_DEV(sData *strRecei, uint16_t Pos)    //105
{
    uint8_t TempU8 = 0;

    TempU8 = *(strRecei->Data_a8 + Pos++) - 0x30;
    TempU8 = TempU8 * 10 + *(strRecei->Data_a8 + Pos++) - 0x30;
    
    if (TempU8 <= MAX_NUMBER_WAKE_UP)
    {
    	sFreqInfor.NumWakeup_u8 = TempU8;
    	Save_Freq_Send_Data();
        
        DCU_Respond(PortConfig, (uint8_t *)"\r\nOK", 4, 1);
    } else
    	DCU_Respond(PortConfig, (uint8_t *)"\r\nFAIL", 6, 0);
}


void _fREAD_OLD_DATA(sData *strRecei, uint16_t Pos)
{

}



void _fSET_SAVE_BOX(sData *strRecei, uint16_t Pos)
{
    //if PortConfig == serial active event Save box immediate -> server -> Send Resp OK -> active
    if (PortConfig == _AT_REQUEST_SERIAL)
    {
        DCU_Respond(PortConfig, (uint8_t *)"\r\nOK", 4, 1);
        fevent_active(sEventAppComm, _EVENT_SAVE_BOX);
    } else
    {
    #ifdef USING_APP_SIM
        sMQTT.aMARK_MESS_PENDING[SEND_SAVE_BOX_OK] = TRUE;
    #endif
    }
}



//=1234,4567
void _fSET_FLOW_THRESH (sData *str_Receiv, uint16_t Pos)
{
    uint16_t    Posfix = Pos;
    uint32_t    TemLowThresh = 0;
    uint32_t    TemHighThresh = 0;
    uint8_t		Temphex = 0;
    //
    while(Posfix < str_Receiv->Length_u16)
    {
        if((*(str_Receiv->Data_a8 + Posfix) >= 0x30) && (*(str_Receiv->Data_a8 + Posfix) <= 0x39))   //Gap so thi dung lai 
            break;
        Posfix++;
    }
      
    //lï¿½y Thresh low
    while(*(str_Receiv->Data_a8 + Posfix) != ',')
    {
        Temphex = *(str_Receiv->Data_a8 + Posfix);
        if((Temphex < '0') || (Temphex > '9') || (Posfix >= str_Receiv->Length_u16))
            break;
        TemLowThresh = TemLowThresh * 10 + Temphex - 0x30;
        Posfix++;
    }  
    //
    while(Posfix < str_Receiv->Length_u16)
    {
        if((*(str_Receiv->Data_a8 + Posfix) >= 0x30) && (*(str_Receiv->Data_a8 + Posfix) <= 0x39))   //Gap so thi dung lai 
            break;
        Posfix++;
    }
    //lï¿½y Thresh high
    while(*(str_Receiv->Data_a8 + Posfix) != ',')
    {
        Temphex = *(str_Receiv->Data_a8 + Posfix);
        if((Temphex < '0') || (Temphex > '9') || (Posfix >= str_Receiv->Length_u16))
            break;
        TemHighThresh = TemHighThresh * 10 + Temphex - 0x30;
        Posfix++;
    }  
    //check xem co get duoc data khong
    if((TemLowThresh <= 0xFFFF) && (TemHighThresh <= 0xFFFF))
    {
    #ifdef USING_APP_WM
        sMeterThreshold.FlowLow  = TemLowThresh;
        sMeterThreshold.FlowHigh = TemHighThresh;
        
        AppWm_Save_Thresh_Measure();
    #endif
      
    #ifdef USING_APP_TEMP
        sMeterThreshold.FlowLow  = TemLowThresh;
        sMeterThreshold.FlowHigh = TemHighThresh;
        
        AppTemp_Save_Thresh_Measure();
    #endif
            
        DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);
    }else
        DCU_Respond(PortConfig,(uint8_t *)"OVER VALUE U16", 14, 0);
}

void _fQUERY_FLOW_THRESH (sData *str_Receiv, uint16_t Pos)
{
#ifdef USING_APP_WM
    uint8_t aTemp[32] = "at+flowthresh:";   //14 kï¿½ tu dau tien
    sData StrResp = {&aTemp[0], 14}; 
    
    Convert_Uint64_To_StringDec (&StrResp, (uint64_t) sMeterThreshold.FlowLow, 0);
    *(StrResp.Data_a8 + StrResp.Length_u16++) = ',';
    Convert_Uint64_To_StringDec (&StrResp, (uint64_t) sMeterThreshold.FlowHigh, 0);

    DCU_Respond(PortConfig, StrResp.Data_a8, StrResp.Length_u16, 0);
#endif
}


void _fSET_QUAN_THRESH (sData *str_Receiv, uint16_t Pos)
{
#ifdef USING_APP_WM
	uint16_t    Posfix = Pos;
    uint32_t    TemLowThresh = 0;
	uint32_t    TemHighThresh = 0;
	uint8_t		Temphex = 0;
	//
	while(Posfix < str_Receiv->Length_u16)
	{
        if((*(str_Receiv->Data_a8 + Posfix) >= 0x30) && (*(str_Receiv->Data_a8 + Posfix) <= 0x39))   //Gap so thi dung lai 
            break;
        Posfix++;
	}
	  
    //lï¿½y Thresh low
    while(*(str_Receiv->Data_a8 + Posfix) != ',')
	{
		Temphex = *(str_Receiv->Data_a8 + Posfix);
		if((Temphex < '0') || (Temphex > '9') || (Posfix >= str_Receiv->Length_u16))
			break;
		TemLowThresh = TemLowThresh * 10 + Temphex - 0x30;
		Posfix++;
	}  
	//
	while(Posfix < str_Receiv->Length_u16)
	{
        if((*(str_Receiv->Data_a8 + Posfix) >= 0x30) && (*(str_Receiv->Data_a8 + Posfix) <= 0x39))   //Gap so thi dung lai 
            break;
        Posfix++;
	}
	//lï¿½y Thresh high
	while(*(str_Receiv->Data_a8 + Posfix) != ',')
	{
		Temphex = *(str_Receiv->Data_a8 + Posfix);
		if((Temphex < '0') || (Temphex > '9') || (Posfix >= str_Receiv->Length_u16))
			break;
		TemHighThresh = TemHighThresh * 10 + Temphex - 0x30;
		Posfix++;
	}  
	//check xem co get duoc data khong
    if((TemLowThresh <= 0xFFFF) && (TemHighThresh <= 0xFFFF))
    {
        sMeterThreshold.PeakLow  = TemLowThresh;
        sMeterThreshold.PeakHigh = TemHighThresh;

        AppWm_Save_Thresh_Measure();

        DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);
    }else
        DCU_Respond(PortConfig,(uint8_t *)"OVER VALUE U16", 14, 0);
#endif
}


void _fQUERY_QUAN_THRESH (sData *str_Receiv, uint16_t Pos)
{
#ifdef USING_APP_WM
	uint8_t aTemp[32] = "at+quanthresh:";   //15 ki tu dau tien
    sData StrResp = {&aTemp[0], 14}; 
    
    Convert_Uint64_To_StringDec (&StrResp, (uint64_t) sMeterThreshold.PeakLow, 0);
	*(StrResp.Data_a8 + StrResp.Length_u16++) = ',';
    Convert_Uint64_To_StringDec (&StrResp, (uint64_t) sMeterThreshold.PeakHigh, 0);

	DCU_Respond(PortConfig, StrResp.Data_a8, StrResp.Length_u16, 0);
#endif
}



void _fSET_BAT_THRESH (sData *str_Receiv, uint16_t Pos)
{
#ifdef USING_APP_WM
	uint16_t    Posfix = Pos;
    uint32_t    TemLowThresh = 0;
	uint8_t		Temphex = 0;
	//
	while(Posfix < str_Receiv->Length_u16)
	{
        if((*(str_Receiv->Data_a8 + Posfix) >= 0x30) && (*(str_Receiv->Data_a8 + Posfix) <= 0x39))   //Gap so thi dung lai 
            break;
        Posfix++;
	}
	  
    //lï¿½y Thresh low
    while(*(str_Receiv->Data_a8 + Posfix) != ',')
	{
		Temphex = *(str_Receiv->Data_a8 + Posfix);
		if((Temphex < '0') || (Temphex > '9') || (Posfix >= str_Receiv->Length_u16))
			break;
		TemLowThresh = TemLowThresh * 10 + Temphex - 0x30;
		Posfix++;
	}  
	
	//check xem co get duoc data khong

    if(TemLowThresh < 100)
    {
        sMeterThreshold.LowBatery  = TemLowThresh;
        
        AppWm_Save_Thresh_Measure();
        DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);
    }else
        DCU_Respond(PortConfig,(uint8_t *)"OVER VALUE", 10, 0);
#endif
}


void _fQUERY_BAT_THRESH (sData *str_Receiv, uint16_t Pos)
{
#ifdef USING_APP_WM
    uint8_t aTemp[32] = "at+batthresh:";   //13 ki tu dau tien
    sData StrResp = {&aTemp[0], 13}; 

    Convert_Uint64_To_StringDec (&StrResp, (uint64_t) sMeterThreshold.LowBatery, 0);

	DCU_Respond(PortConfig, StrResp.Data_a8, StrResp.Length_u16, 0);
#endif
}
 

void _fSET_PULSE_VALUE (sData *str_Receiv, uint16_t Pos)
{
	uint16_t    Posfix = Pos;
    uint32_t    TempValue = 0;
	uint8_t		Temphex = 0;
	//
	while (Posfix < str_Receiv->Length_u16)
	{
        if ((*(str_Receiv->Data_a8 + Posfix) >= 0x30) && (*(str_Receiv->Data_a8 + Posfix) <= 0x39))   //Gap so thi dung lai 
            break;
        Posfix++;
	}
    //
    while (*(str_Receiv->Data_a8 + Posfix) != ',')
	{
		Temphex = *(str_Receiv->Data_a8 + Posfix);
		if((Temphex < '0') || (Temphex > '9') || (Posfix >= str_Receiv->Length_u16))
			break;
		TempValue = TempValue * 10 + Temphex - 0x30;
		Posfix++;
	}  

#ifdef USING_APP_WM
    #ifdef LPTIM_ENCODER_MODE 
        sPulse.Number_u32 = TempValue * 4;
    #else   
        sPulse.Number_u32 = TempValue;
    #endif
        
    DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);
#endif
}

void _fQUERY_PULSE_VALUE (sData *str_Receiv, uint16_t Pos)
{
#ifdef USING_APP_WM
    uint8_t aTemp[32] = "at+pulse:";   //13 kï¿½ tu dau tien
    sData StrResp = {&aTemp[0], 9}; 

    #ifdef LPTIM_ENCODER_MODE
        Convert_Uint64_To_StringDec (&StrResp, (uint64_t) (sPulse.Number_u32 / 4), 0);
    #else   
        Convert_Uint64_To_StringDec (&StrResp, (uint64_t) (sPulse.Number_u32), 0);
    #endif
    
    DCU_Respond(PortConfig, StrResp.Data_a8, StrResp.Length_u16, 0);
#endif
}



void _fSET_RTC_TIME (sData *str_Receiv, uint16_t Pos)
{
    ST_TIME_FORMAT      sRTC_temp = {0};
	RTC_TimeTypeDef     sRTCTime = {0};
    uint8_t *pData = str_Receiv->Data_a8 + Pos;
    
	if ((*(pData + 2) != ':') || (*(pData + 5) != ':'))
	{
		//Send wrong format message
		DCU_Respond(PortConfig, (uint8_t *)"ERROR", 5, 0);
		return;
	} else
	{
		sRTC_temp.hour = (*(pData) - 0x30) * 10 + (*(pData + 1) - 0x30);
		sRTC_temp.min  = (*(pData + 3) - 0x30) * 10 + (*(pData + 4) - 0x30);
		sRTC_temp.sec  = (*(pData + 6) - 0x30) * 10 + (*(pData + 7) - 0x30);
		
		if (Check_update_Time(&sRTC_temp) != 1)
		{
			//Send wrong format message
			DCU_Respond(PortConfig, (uint8_t *)"ERROR", 5, 0);
			return;
		} else
		{
			sRTCTime.Seconds 	= sRTC_temp.sec;
			sRTCTime.Minutes 	= sRTC_temp.min;
			sRTCTime.Hours 		= sRTC_temp.hour;
            
			HAL_RTC_SetTime(&hrtc, &sRTCTime, RTC_FORMAT_BIN);
            
			DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);
		}
	}
}

void _fSET_RTC_DATE (sData *str_Receiv, uint16_t Pos)
{
	ST_TIME_FORMAT      sRTC_temp = {0};
    RTC_DateTypeDef     sRTCDate = {0};
    uint8_t *pData = str_Receiv->Data_a8 + Pos;
    
	if ((*(pData + 1) != '-') || (*(pData + 4) != '-') || (*(pData + 7) != '-'))
	{
		//Send wrong format message
		DCU_Respond(PortConfig, (uint8_t *)"ERROR", 5, 0);
		return;
	} else
	{
		sRTC_temp.day   = (*pData - 0x30) - 1;
		sRTC_temp.date  = (*(pData + 2) - 0x30) * 10 + (*(pData + 3) - 0x30);
		sRTC_temp.month = (*(pData + 5) - 0x30) * 10 + (*(pData + 6) - 0x30);
		sRTC_temp.year  = (*(pData + 8) - 0x30) * 10 + (*(pData + 9) - 0x30);
		
		if (Check_update_Time(&sRTC_temp) != 1)
		{
			//Send wrong format message
			DCU_Respond(PortConfig, (uint8_t *)"ERROR", 5, 0);
			return;
		} else
		{
			sRTCDate.WeekDay 	= sRTC_temp.day;
			sRTCDate.Date 		= sRTC_temp.date;
			sRTCDate.Month 		= sRTC_temp.month;
			sRTCDate.Year 		= sRTC_temp.year;
            
			HAL_RTC_SetDate(&hrtc, &sRTCDate, RTC_FORMAT_BIN);
			DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);
		}
	}
}



//Ex: at+rtc=2-20-06-21,12:10:20  
void _fSET_RTC (sData *str_Receiv, uint16_t Pos)        
{
    ST_TIME_FORMAT    sRTC_temp = {0};
	
    if((*(str_Receiv->Data_a8+1)!= '-')||(*(str_Receiv->Data_a8+4)!= '-')|| (*(str_Receiv->Data_a8+7)!= '-') ||  
        (*(str_Receiv->Data_a8+10)!= ',')||(*(str_Receiv->Data_a8+13)!= ':')||(*(str_Receiv->Data_a8+16)!= ':'))
	{
		//Send wrong format message
		DCU_Respond(PortConfig, (uint8_t *)"ERROR", 5, 0);
		return;
	}
	else
	{
		sRTC_temp.day   = (*(str_Receiv->Data_a8))-0x30 - 1;
		sRTC_temp.date  = ((*(str_Receiv->Data_a8 + 2))-0x30)*10 + (*(str_Receiv->Data_a8 + 3) - 0x30);
		sRTC_temp.month = ((*(str_Receiv->Data_a8 + 5))-0x30)*10 + (*(str_Receiv->Data_a8 + 6) - 0x30);
		sRTC_temp.year  = ((*(str_Receiv->Data_a8 + 8))-0x30)*10 + (*(str_Receiv->Data_a8 + 9) - 0x30);
  
        sRTC_temp.hour  = ((*(str_Receiv->Data_a8 + 11))-0x30)*10 + (*(str_Receiv->Data_a8 + 12) - 0x30);
		sRTC_temp.min   = ((*(str_Receiv->Data_a8 + 14))-0x30)*10 + (*(str_Receiv->Data_a8 + 15) - 0x30);
		sRTC_temp.sec   = ((*(str_Receiv->Data_a8 + 17))-0x30)*10 + (*(str_Receiv->Data_a8 + 18) - 0x30);
		
		if (Check_update_Time(&sRTC_temp) != 1)
		{
			//Send wrong format message
			DCU_Respond(PortConfig, (uint8_t *)"ERROR", 5, 0);
			return;
		} else
		{
            sRTCSet.day     = sRTC_temp.day;
            sRTCSet.date    = sRTC_temp.date;
            sRTCSet.month   = sRTC_temp.month;
            sRTCSet.year    = sRTC_temp.year;
            
            sRTCSet.hour    = sRTC_temp.hour;
            sRTCSet.min     = sRTC_temp.min;
            sRTCSet.sec     = sRTC_temp.sec;
            
            fevent_active(sEventAppComm, _EVENT_SET_RTC);
            DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);
		}
	}
}



void _fQUERY_RTC (sData *str_Receiv, uint16_t Pos)
{
    uint8_t length = 0;
    uint8_t aTEMP_RTC[40] = {0};
    
	aTEMP_RTC[length++] = 't'; aTEMP_RTC[length++] = 'i'; aTEMP_RTC[length++] = 'm'; aTEMP_RTC[length++] = 'e'; 
    aTEMP_RTC[length++] = '='; 
	aTEMP_RTC[length++] = sRTC.hour / 10 + 0x30;    aTEMP_RTC[length++] = sRTC.hour % 10 + 0x30;    aTEMP_RTC[length++] = ':';
	aTEMP_RTC[length++] = sRTC.min / 10 + 0x30;     aTEMP_RTC[length++] = sRTC.min % 10 + 0x30;     aTEMP_RTC[length++] = ':';
	aTEMP_RTC[length++] = sRTC.sec / 10 + 0x30;     aTEMP_RTC[length++] = sRTC.sec % 10 + 0x30;     aTEMP_RTC[length++] = ';';
    aTEMP_RTC[length++] = 0x0D; 
	aTEMP_RTC[length++] = 'd'; aTEMP_RTC[length++] = 'a'; aTEMP_RTC[length++] = 't'; aTEMP_RTC[length++] = 'e'; 
    aTEMP_RTC[length++] = '='; 
	aTEMP_RTC[length++] = sRTC.day + 0x31;          aTEMP_RTC[length++] = '-';
	aTEMP_RTC[length++] = sRTC.date / 10 + 0x30;    aTEMP_RTC[length++] = sRTC.date % 10 + 0x30;      aTEMP_RTC[length++] = '-';
	aTEMP_RTC[length++] = sRTC.month / 10 + 0x30;   aTEMP_RTC[length++] = sRTC.month % 10 + 0x30;     aTEMP_RTC[length++] = '-';
	aTEMP_RTC[length++] = sRTC.year / 10 + 0x30;    aTEMP_RTC[length++] = sRTC.year % 10 + 0x30;
	
	DCU_Respond(PortConfig, aTEMP_RTC, length, 0);
}


void _fSET_RF_RQ_STIME (sData *str_Receiv, uint16_t Pos)
{
#ifdef USING_APP_LORA
    sCtrlLora.CtrlStime_u8 = TRUE;
#endif
}

void _fRESET_MODEM (sData *str_Receiv, uint16_t Pos)
{
    DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);

    Reset_Chip_Immediately();
}

void _fERASE_MEMORY (sData *str_Receiv, uint16_t Pos)
{     
#ifdef USING_APP_MEM
    fevent_enable( sEventAppMem, _EVENT_MEM_ERASE);

    DCU_Respond(PortConfig, (uint8_t *)"PENDING", 7, 0);
#endif
}


void _fQUERY_FIRM_VER (sData *strRecei, uint16_t Pos)
{ 
    DCU_Respond(PortConfig, sFirmVersion.Data_a8, sFirmVersion.Length_u16, 0);
}

void _fGET_URLHTTP_FIRM (sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_SIM
    int PosFind;
    uint8_t Flag_Get_name = 0;
        
     //lay URL firm
    Reset_Buff(&sSimFwUpdate.StrURL_FirmIP);
    Reset_Buff(&sSimFwUpdate.StrURL_FirmPort);
    Reset_Buff(&sSimFwUpdate.StrURL_FirmPath); 
    Reset_Buff(&sSimFwUpdate.StrURL_FirmName);
    Reset_Buff(&sSimFwUpdate.StrURL_HTTP);
    
    sData Str1 = {(uint8_t*) "//", 2};
    
    PosFind = Find_String_V2(&Str1, strRecei);
    if (PosFind >=0)
    {
        PosFind += Str1.Length_u16;
        while (*(strRecei->Data_a8 + PosFind) != ':')   
        {
            *(sSimFwUpdate.StrURL_FirmIP.Data_a8 + sSimFwUpdate.StrURL_FirmIP.Length_u16++) = *(strRecei->Data_a8 + PosFind++);
            if ((PosFind >= strRecei->Length_u16) || (sSimFwUpdate.StrURL_FirmIP.Length_u16 >= LENGTH_FTP))
            {
                DCU_Respond(PortConfig,(uint8_t *)"ERROR", 5, 0);
                return;
            }
        }
        PosFind++;
        while (*(strRecei->Data_a8 + PosFind) != '/')
        {
            *(sSimFwUpdate.StrURL_FirmPort.Data_a8 + sSimFwUpdate.StrURL_FirmPort.Length_u16++) = *(strRecei->Data_a8 + PosFind++);
            if ((PosFind >= strRecei->Length_u16)|| (sSimFwUpdate.StrURL_FirmPort.Length_u16 >= LENGTH_FTP))
            {
                DCU_Respond(PortConfig,(uint8_t *)"ERROR", 5, 0);
                return;
            }
        }
        PosFind++;
        
        while (*(strRecei->Data_a8 + PosFind) != '/')
        {
            *(sSimFwUpdate.StrURL_FirmPath.Data_a8 + sSimFwUpdate.StrURL_FirmPath.Length_u16++) = *(strRecei->Data_a8 + PosFind++);
            if ((PosFind >= strRecei->Length_u16) || (sSimFwUpdate.StrURL_FirmPath.Length_u16 >= LENGTH_FTP))
            {
                //neu het thi coi nhu day la name
                Copy_String_2(&sSimFwUpdate.StrURL_FirmName , &sSimFwUpdate.StrURL_FirmPath);
                Reset_Buff(&sSimFwUpdate.StrURL_FirmPath);
                Flag_Get_name = 1;
                break;
            }
        }
        
        while (Flag_Get_name == 0)
        {
            while (*(strRecei->Data_a8 + PosFind) != '/')
            {
                *(sSimFwUpdate.StrURL_FirmName.Data_a8 + sSimFwUpdate.StrURL_FirmName.Length_u16++) = *(strRecei->Data_a8 + PosFind++);
                if ((PosFind >= strRecei->Length_u16) || (sSimFwUpdate.StrURL_FirmName.Length_u16 >= LENGTH_FTP))
                {
                    Flag_Get_name = 1;
                    //neu het thi coi nhu day la name
                    break;
                }
            }
            //neu con xuong day thi tiep tuc copy vao path va tiep tuc lau name
            if (Flag_Get_name == 0)
            {
                Copy_String_2(&sSimFwUpdate.StrURL_FirmPath , &sSimFwUpdate.StrURL_FirmName);
                Reset_Buff(&sSimFwUpdate.StrURL_FirmName);
                if ((PosFind >= strRecei->Length_u16) || (sSimFwUpdate.StrURL_FirmPath.Length_u16 >= LENGTH_FTP))
                {
                    DCU_Respond(PortConfig,(uint8_t *)"ERROR", 5, 0);
                    return;
                }
            }
            PosFind++;  
        }
        //check length
        Copy_String_2(&sSimFwUpdate.StrURL_HTTP , strRecei);
        
        DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);
        //chuyen sang update firm
        sSimVar.ModeConnectFuture_u8 = MODE_CONNECT_HTTP;
        
        sSimCommon.PowerStatus_u8 = _POWER_START;
        AppSim_Start_Module_Sim();
    }
    
#endif
}


void _fGET_URLFTP_FIRM (sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_SIM
    int         PosFind;
    uint8_t     Flag_Get_name = 0;
    
    //lay URL firm
    Reset_Buff(&sSimFwUpdate.StrURL_FirmIP);
    Reset_Buff(&sSimFwUpdate.StrURL_FirmPort);
    Reset_Buff(&sSimFwUpdate.StrURL_FirmPath); 
    Reset_Buff(&sSimFwUpdate.StrURL_FirmName);

    sData Str1 = {(uint8_t*) "//", 2};
    
    PosFind = Find_String_V2(&Str1, strRecei);
    if (PosFind >=0)
    {
        PosFind += Str1.Length_u16;
        while (*(strRecei->Data_a8 + PosFind) != ':')
        {
            *(sSimFwUpdate.StrURL_FirmIP.Data_a8 + sSimFwUpdate.StrURL_FirmIP.Length_u16++) = *(strRecei->Data_a8 + PosFind++);
            if (PosFind >= strRecei->Length_u16)
            {
                DCU_Respond(PortConfig,(uint8_t *)"ERROR", 5, 0);
                return;
            }
        }
        PosFind++;
        while (*(strRecei->Data_a8 + PosFind) != '/')
        {
            *(sSimFwUpdate.StrURL_FirmPort.Data_a8 + sSimFwUpdate.StrURL_FirmPort.Length_u16++) = *(strRecei->Data_a8 + PosFind++);
            if (PosFind >= strRecei->Length_u16)
            {
                DCU_Respond(PortConfig,(uint8_t *)"ERROR", 5, 0);
                return;
            }
        }
        PosFind++;
        
        while (*(strRecei->Data_a8 + PosFind) != '/')
        {
            *(sSimFwUpdate.StrURL_FirmPath.Data_a8 + sSimFwUpdate.StrURL_FirmPath.Length_u16++) = *(strRecei->Data_a8 + PosFind++);
            if (PosFind >= strRecei->Length_u16)
            {
                //neu het thi coi nhu day la name
                Copy_String_2(&sSimFwUpdate.StrURL_FirmName , &sSimFwUpdate.StrURL_FirmPath);
                Reset_Buff(&sSimFwUpdate.StrURL_FirmPath);
                Flag_Get_name = 1;
                break;
            }
        }
        
        while (Flag_Get_name == 0)
        {
            while (*(strRecei->Data_a8 + PosFind) != '/')
            {
                *(sSimFwUpdate.StrURL_FirmName.Data_a8 + sSimFwUpdate.StrURL_FirmName.Length_u16++) = *(strRecei->Data_a8 + PosFind++);
                if (PosFind >= strRecei->Length_u16)
                {
                    Flag_Get_name = 1;
                    //neu het thi coi nhu day la name
                    break;
                }
            }
            //neu con xuong day thi tiep tuc copy vao path va tiep tuc lau name
            if (Flag_Get_name == 0)
            {
                Copy_String_2(&sSimFwUpdate.StrURL_FirmPath , &sSimFwUpdate.StrURL_FirmName);
                Reset_Buff(&sSimFwUpdate.StrURL_FirmName);
                if (PosFind >= strRecei->Length_u16)
                {
                    DCU_Respond(PortConfig,(uint8_t *)"ERROR", 5, 0);
                    return;
                }
            }
            PosFind++;  
        }

        DCU_Respond(PortConfig, (uint8_t *)"FAIL", 4, 0);   // OK. Chua them FTP

//        DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);
//        //chuyen sang update firm
//        sSimVar.ModeConnectFuture_u8 = MODE_CONNECT_FTP;
//        sSimCommon.PowerStatus_u8 = _POWER_START;
//        AppSim_Start_Module_Sim();
    }
#endif
}


void _fQUERY_INDEX_LOG (sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_MEM
    uint8_t aTEMP[30] = {0};
    sData StrIndex = {&aTEMP[0], 0};
    
    Convert_Uint64_To_StringDec(&StrIndex, sRecLog.IndexSave_u16, 0);
    *(StrIndex.Data_a8 + StrIndex.Length_u16++) = '/';
    Convert_Uint64_To_StringDec(&StrIndex, sRecLog.MaxRecord_u16, 0);
    
    DCU_Respond(PortConfig, StrIndex.Data_a8, StrIndex.Length_u16, 0);
#endif
}

void _fTEST_LOG (sData *strRecei, uint16_t Pos)
{
    UTIL_Log_Str (DBLEVEL_M, "TEST LOG MODEM WM HOA BINH\r\n" );
}


void _fQUERY_ALL_LOG (sData *strRecei, uint16_t Pos)
{
//    sOldRec.StartIndex_u8 = sRecLog.IndexSave_u16; 
//    sOldRec.EndIndex_u8   = sRecLog.IndexSave_u16;
//    sOldRec.Type_u8       = _RQ_RECORD_LOG;
//        
//    sOldRec.Count_u8 = 0;
//    sOldRec.Kind_u8 = 1;
//    sOldRec.Port_u8 = PortConfig;
//    
//    fevent_enable(sEventAppComm, _EVENT_READ_OLD_RECORD);
//    DCU_Respond(PortConfig, (uint8_t *)"OK\r\n", 4, 0);
}

void _fGET_LAST_LOG (sData *strRecei, uint16_t Pos)
{
//    sOldRec.Type_u8  = _RQ_RECORD_LOG;
//    sOldRec.Kind_u8 = 0;
//    sOldRec.Port_u8 = PortConfig;
//    
//    fevent_enable(sEventAppComm, _EVENT_READ_OLD_RECORD);
//    DCU_Respond(PortConfig, (uint8_t *)"OK\r\n", 4, 0);
}


void _fGET_LAST_FOR_LOG (sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_MEM
    AppMem_Get_Req_AT(_RQ_RECORD_LOG, SERIAL_Get_Num(strRecei, Pos));
#endif
}


void _fGET_LAST_FOR_OPERA (sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_MEM
    AppMem_Get_Req_AT(_RQ_RECORD_TSVH, SERIAL_Get_Num(strRecei, Pos));
#endif
}



void _fGET_LAST_FOR_EVENT (sData *strRecei, uint16_t Pos)
{   
#ifdef USING_APP_MEM
    AppMem_Get_Req_AT(_RQ_RECORD_EVENT, SERIAL_Get_Num(strRecei, Pos));
#endif
}


void _fGET_LAST_FOR_GPS (sData *strRecei, uint16_t Pos)
{   
#ifdef USING_APP_MEM
    AppMem_Get_Req_AT(_RQ_RECORD_GPS, SERIAL_Get_Num(strRecei, Pos));
#endif
}




void _fSET_CALIB_LEVEL (sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_WM
    uint8_t Sign = 0;
    int16_t Value = 0;
    uint8_t Count = 0;
    uint8_t Result = TRUE;
    
    if (*(strRecei->Data_a8 + Pos++) == '-')
        Sign = 1;
    
    while (Pos < strRecei->Length_u16)
    {
        if ( (*(strRecei->Data_a8 + Pos) < 0x30) || (*(strRecei->Data_a8 + Pos) > 0x39) || (Count > 5) )
        {
            Result = FALSE;
            break;
        }
        //Tinh gia tri
        Value = Value * 10 + *(strRecei->Data_a8 + Pos) - 0x30;
        Count++;
        Pos++;
    }
    
    if (Result == TRUE)
    {
        if (Sign == 1)
            sWmVar.ZeroPointCalib_i16 = 0 - Value;
        else
            sWmVar.ZeroPointCalib_i16 = Value;
        
        AppWm_Save_Level_Calib ();
        
       DCU_Respond(PortConfig, (uint8_t *)"OK\r\n", 4, 0);
    } else
        DCU_Respond(PortConfig,(uint8_t *)"ERROR", 5, 0);        
#endif
    DCU_Respond(PortConfig,(uint8_t *)"NOT SUPPORT!", 12, 0);   
}


void _fQUERY_CALIB_LEVEL (sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_WM
    uint8_t aTEMP[8] = {0};
    sData StrIndex = {&aTEMP[0], 0};
    
    Convert_Int64_To_StringDec(&StrIndex, sWmVar.ZeroPointCalib_i16, 0);
    
    DCU_Respond(PortConfig, StrIndex.Data_a8, StrIndex.Length_u16, 0);
#else
    DCU_Respond(PortConfig,(uint8_t *)"NOT SUPPORT!", 12, 0);    
#endif
}


void _fSET_LEVEL_THRESH (sData *str_Receiv, uint16_t Pos)
{
#ifdef USING_APP_WM
    uint16_t    Posfix = Pos;
    uint32_t    TemLowThresh = 0;
    uint32_t    TemHighThresh = 0;
    uint8_t		Temphex = 0;
    //
    while(Posfix < str_Receiv->Length_u16)
    {
        if((*(str_Receiv->Data_a8 + Posfix) >= 0x30) && (*(str_Receiv->Data_a8 + Posfix) <= 0x39))   //Gap so thi dung lai 
            break;
        Posfix++;
    }
      
    //lay Thresh low
    while(*(str_Receiv->Data_a8 + Posfix) != ',')
    {
        Temphex = *(str_Receiv->Data_a8 + Posfix);
        if((Temphex < '0') || (Temphex > '9') || (Posfix >= str_Receiv->Length_u16))
            break;
        TemLowThresh = TemLowThresh * 10 + Temphex - 0x30;
        Posfix++;
    }  
    //
    while(Posfix < str_Receiv->Length_u16)
    {
        if((*(str_Receiv->Data_a8 + Posfix) >= 0x30) && (*(str_Receiv->Data_a8 + Posfix) <= 0x39))   //Gap so thi dung lai 
            break;
        Posfix++;
    }
    //lay Thresh high
    while(*(str_Receiv->Data_a8 + Posfix) != ',')
    {
        Temphex = *(str_Receiv->Data_a8 + Posfix);
        if((Temphex < '0') || (Temphex > '9') || (Posfix >= str_Receiv->Length_u16))
            break;
        TemHighThresh = TemHighThresh * 10 + Temphex - 0x30;
        Posfix++;
    }  
    //check xem co get duoc data khong
    if((TemLowThresh <= 0xFFFF) && (TemHighThresh <= 0xFFFF))
    {
        sMeterThreshold.LevelLow  = TemLowThresh;
        sMeterThreshold.LevelHigh = TemHighThresh;
        
        AppWm_Save_Thresh_Measure();
        
        DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);
    }else
        DCU_Respond(PortConfig,(uint8_t *)"OVER VALUE U16", 14, 0);
#endif
}


void _fQUERY_LEVEL_THRESH (sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_WM
    uint8_t aTemp[32] = "at+levelthresh:";   //15 ki tu dau tien
    sData StrResp = {&aTemp[0], 15}; 
    
    Convert_Int64_To_StringDec (&StrResp, (uint64_t) sMeterThreshold.LevelLow, 0);
	*(StrResp.Data_a8 + StrResp.Length_u16++) = ',';
    Convert_Int64_To_StringDec (&StrResp, (uint64_t) sMeterThreshold.LevelHigh, 0);

	DCU_Respond(PortConfig, StrResp.Data_a8, StrResp.Length_u16, 0);
#endif
}
 
void _fQUERY_GPS_LOC (sData *strRecei, uint16_t Pos)
{
#ifdef DEVICE_TYPE_GATEWAY
#ifdef USING_APP_SIM
    if (sSimCommon.sGPS.Status_u8 == true)
        DCU_Respond(PortConfig, sSimCommon.sGPS.aPOS_INFOR, sSimCommon.sGPS.LengData_u8 , 0);
    else
        DCU_Respond(PortConfig,(uint8_t *)"FAIL!", 5, 0);   
#endif
#endif
}


/*===================== Func Handler =========================*/
uint8_t Check_AT_User(sData *StrUartRecei, uint8_t Type)
{
	uint8_t var;
	int Pos_Str = -1;
	uint16_t i = 0;
	sData sDataConfig = {&aDATA_CONFIG[0], 0};

	//convert lai chu in hoa thanh chu thuong
	for (i = 0; i < StrUartRecei->Length_u16; i++)
	{
		if ((*(StrUartRecei->Data_a8 + i) == '=') || (*(StrUartRecei->Data_a8 + i) == '?'))
			break;

		if ((*(StrUartRecei->Data_a8 + i) >= 0x41) && (*(StrUartRecei->Data_a8 + i) <= 0x5A))
			*(StrUartRecei->Data_a8 + i) = *(StrUartRecei->Data_a8 + i) + 0x20;
	}

	for (var = _SET_DEV_SERIAL; var <= _END_AT_CMD; var++)
	{
		Pos_Str = Find_String_V2((sData*) &CheckList_AT_CONFIG[var].sTempReceiver, StrUartRecei);
		if ((Pos_Str >= 0) && (CheckList_AT_CONFIG[var].CallBack != NULL))
		{

            //Copy lenh vao buff. de repond kem theo lenh
            Reset_Buff(&sModem.strATCmd);

            for (i = 0; i <(StrUartRecei->Length_u16 - Pos_Str); i++)
            {
                if ((*(StrUartRecei->Data_a8 + Pos_Str + i) == '?') || (*(StrUartRecei->Data_a8+Pos_Str+i) == '=') \
                    || (sModem.strATCmd.Length_u16 >= LEGNTH_AT_SERIAL))
                    break;
                else 
                    *(sModem.strATCmd.Data_a8 + sModem.strATCmd.Length_u16++) = *(StrUartRecei->Data_a8+Pos_Str+i); 
            }
            //Copy data after at cmd
            Pos_Str += CheckList_AT_CONFIG[var].sTempReceiver.Length_u16;

            for (i = Pos_Str; i < StrUartRecei->Length_u16; i++)
            {
                if (*(StrUartRecei->Data_a8 + i) == 0x0D)
                {
                    if ( (i < (StrUartRecei->Length_u16 - 1)) && (*(StrUartRecei->Data_a8 + i + 1) == 0x0A) )
                    {
                        break;
                    }
                }
                //
                if ( sDataConfig.Length_u16 < sizeof(aDATA_CONFIG) )
                    *(sDataConfig.Data_a8 + sDataConfig.Length_u16++) = *(StrUartRecei->Data_a8 + i);
            }
            
            PortConfig = Type;
            
            //Test
            switch (Type)
            {
                case _AT_REQUEST_SERIAL:
                case _AT_REQUEST_SERVER: 
                    //check at request
                    CheckList_AT_CONFIG[var].CallBack(&sDataConfig, 0);
                    break;
                case _AT_REQUEST_LORA:       
                #ifdef USING_APP_LORA
                    //Copy data to lora at cmd handle
                    shLora.LengthConfig_u8 = 0;
                
                    for (i = 0; i < CheckList_AT_CONFIG[var].sTempReceiver.Length_u16; i++)
                    {
                        shLora.aDATA_CONFIG[shLora.LengthConfig_u8++] = *(CheckList_AT_CONFIG[var].sTempReceiver.Data_a8 + i); 
                    } 
                        
                    for (i = 0; i <sDataConfig.Length_u16; i++)
                    {
                        shLora.aDATA_CONFIG[shLora.LengthConfig_u8++] = *(sDataConfig.Data_a8 + i); 
                    }
                    
                    shLora.rAtcmd_u8 = TRUE;  
                
                    #ifndef DEVICE_TYPE_GATEWAY 
                        CheckList_AT_CONFIG[var].CallBack(&sDataConfig, 0);
                    #endif
                #endif
                    break;
                default:
                    break;
            }

			return 1;
		}
	}

	return 0;
}




uint16_t SERIAL_Get_Num (sData *strRecei, uint16_t Pos)
{
    uint16_t    Value = 0xFFFF;
    uint8_t     temp = 0,  count = 0;
    
    while (Pos < strRecei->Length_u16)
    {
        if (*(strRecei->Data_a8 + Pos++) == '(')
        {
            Value = 0;
            count = 0;
            temp = *(strRecei->Data_a8 + Pos++);
            while ((temp >= 0x30) && (temp <= 0x39))
            {
                Value = Value * 10 + (temp - 0x30);
                count++;
                temp = *(strRecei->Data_a8 + Pos++);
                if ((count >= 8) || (Pos > strRecei->Length_u16))
                    break;
            }
            
            break;
        }     
    }
    
    return Value;
}
      




void _fSET_LIST_ID_SLAVE (sData *str_Receiv, uint16_t Pos)
{
#ifdef USING_APP_TEMH
    uint8_t Temp = 0;
    uint8_t count = Pos;
    uint8_t i = 0;
    
    //check xem co chu khong. neu co chu thi bao error
    for (i = Pos; i < str_Receiv->Length_u16; i++)
    {
        if((*(str_Receiv->Data_a8 + i) < 0x30) || (*(str_Receiv->Data_a8 + i) > 0x39))
        {
			DCU_Respond(PortConfig, (uint8_t *)"ERROR", 5, 0);
			return;
        }
    }
    //bat dau lay data
    Temp = (*(str_Receiv->Data_a8 + count) - 0x30) * 10 + *(str_Receiv->Data_a8 + count + 1) - 0x30;
    count += 2;
    
    if (Temp > 0x70) 
    {
        DCU_Respond(PortConfig, (uint8_t *)"ERROR", 5, 0);
        return;
    }
    
    if ((str_Receiv->Length_u16 - count) == (2 * Temp))
    {
        if (Temp >= MAX_SLAVE)
        {
            Temp = MAX_SLAVE;
        }
        //Get data
        sTempHumi.NumSlave_u8 = Temp;
        
        for (i = 0; i < sTempHumi.NumSlave_u8; i++)
        {
            sTempHumi.aSlaveID[i] = (*(str_Receiv->Data_a8 + count) - 0x30) * 10 + *(str_Receiv->Data_a8 + count + 1) - 0x30;
            count += 2;
        }
        
        AppTemH_Save_Slave_ID();
        
        DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);
    } else
    {
        DCU_Respond(PortConfig, (uint8_t *)"ERROR", 5, 0);
    }
#endif
}



void _fQUERY_LIST_ID_SLAVE (sData *str_Receiv, uint16_t Pos)
{
#ifdef USING_APP_TEMH
    uint8_t length = 0;
    uint8_t i = 0;
    uint8_t aTEMP[64] = {0};
    
    aTEMP[length++] = sTempHumi.NumSlave_u8 / 10 + 0x30;
    aTEMP[length++] = sTempHumi.NumSlave_u8 % 10 + 0x30;
    aTEMP[length++] = ':';
    
    for(i = 0; i < sTempHumi.NumSlave_u8; i++)
    {
        aTEMP[length++] = sTempHumi.aSlaveID[i]/10 + 0x30;
        aTEMP[length++] = sTempHumi.aSlaveID[i]%10 + 0x30;
    }
    
    DCU_Respond(PortConfig, &aTEMP[0], length, 0);
#endif
}


void _fSET_EX_BAT_THRESH (sData *str_Receiv, uint16_t Pos)
{
#ifdef USING_APP_TEMH
    uint16_t Temp = 0;
    
    Temp = SERIAL_Get_Num(str_Receiv, Pos);
    
    if (Temp != 0xFFFF)
    {
        sMeterThreshold.ExBatVol = Temp;
        //
        AppTemH_Save_Thresh_Measure();
        DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);
    } else
    {
        DCU_Respond(PortConfig, (uint8_t *)"Fail Format!", 12, 0);
    }
#endif
}


void _fQUERY_EX_BAT_THRESH (sData *str_Receiv, uint16_t Pos)
{
//    uint8_t aTemp[32] = {0};   
//    sData StrResp = {&aTemp[0], 0}; 
//
//    Convert_Uint64_To_StringDec (&StrResp, (uint64_t) sMeterThreshold.ExBatVol, 0);
//
//	DCU_Respond(PortConfig, StrResp.Data_a8, StrResp.Length_u16, 0);
}


void _fSET_POWER_ON_TEMH (sData *str_Receiv, uint16_t Pos)
{
#ifdef USING_APP_TEMH
    sTempHumi.PowerStatus_u8 = true;
    V12_IN_ON;
 
    DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);
#endif
}

void _fSET_POWER_OFF_TEMH (sData *str_Receiv, uint16_t Pos)
{
#ifdef USING_APP_TEMH
    sTempHumi.PowerStatus_u8 = false;
    V12_IN_OFF;
    
    DCU_Respond(PortConfig, (uint8_t *)"OK", 2, 0);
#endif
}

/*
    @brief  Reset DCU
*/
void        _fRESET_DCU (sData *strRecei, uint16_t Pos)
{
    DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
//    Reset_Chip();
    
//    fevent_enable(sEventAppPcBox, _EVENT_RESET_DCU);
//    fevent_disable(sEventAppPcBox, _EVENT_PC_BOX_RECEIVE_HANDLE);
//    fevent_disable(sEventAppPcBox, _EVENT_PC_BOX_COMPLETE_RECEIVE);
    ResetDCU();
}

/*
    @brief  Reset PcBox
*/
void        _fRESET_PCBOX (sData *strRecei, uint16_t Pos)
{
    fevent_active(sEventAppPcBox, _EVENT_RESET_PC_BOX);
}

/*
    @brief  On PcBox
*/
void        _fON_PCBOX (sData *strRecei, uint16_t Pos)
{
    uint32_t temp = 0;
    
    if( strRecei->Data_a8[0] >= '0' && strRecei->Data_a8[0] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = 0; i < strRecei->Length_u16; i++)
        {
            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i]>'9') break;
            else length++;
        }
        temp = Convert_String_To_Dec(strRecei->Data_a8 , length);
        if(temp >=500)
        {
            if(sParamPcBox.StatePcBox == _STT_PCBOX_SLEEP)
            {
                ON_PcBox(temp);
            }
            else
            {
                switch(sParamPcBox.StatePcBox)
                {
                    case _STT_PCBOX_CONNECT:
                      UTIL_Printf(DBLEVEL_L, (uint8_t*)"PcBox CONNECT", sizeof("PcBox CONNECT")-1);
                      break;
                      
                    case _STT_PCBOX_DISCONNECT:
                      UTIL_Printf(DBLEVEL_L, (uint8_t*)"PcBox DISCONNECT", sizeof("PcBox DISCONNECT")-1);
                      break;
                      
                    case _STT_PCBOX_SLEEP:
                      break;
                      
                    default:
                      break;
                }
                UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
            }
        }
        else
            DCU_Respond(PortConfig, (uint8_t*)"FAIL\r\n", 6, 0);
    }
    else
        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
}

/*
    @brief  Off PcBox
*/
void        _fOFF_PCBOX (sData *strRecei, uint16_t Pos)
{
    uint32_t temp = 0;
    
    if( strRecei->Data_a8[0] >= '0' && strRecei->Data_a8[0] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = 0; i < strRecei->Length_u16; i++)
        {
            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i]>'9') break;
            else length++;
        }
        temp = Convert_String_To_Dec(strRecei->Data_a8 , length);
        if(temp >=500)
        {
            if(sParamPcBox.StatePcBox == _STT_PCBOX_CONNECT || sParamPcBox.StatePcBox == _STT_PCBOX_DISCONNECT)
            {
                OFF_PcBox(temp);
            }
            else
            {
                switch(sParamPcBox.StatePcBox)
                {
                    case _STT_PCBOX_CONNECT:
                      break;
                      
                    case _STT_PCBOX_DISCONNECT:
                      break;
                      
                    case _STT_PCBOX_SLEEP:
                      
                      UTIL_Printf(DBLEVEL_L, (uint8_t*)"PcBox SLEEP", sizeof("PcBox SLEEP")-1);
                      break;
                      
                    default:
                      break;
                }
                UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
            }
        }
        else
            DCU_Respond(PortConfig, (uint8_t*)"FAIL\r\n", 6, 0);
    }
    else
        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
}

/*
    @brief  Fix Motor
*/
void        _fAPP_FIX_MOTOR (sData *strRecei, uint16_t Pos)
{
    int16_t temp = 0;
    if( strRecei->Data_a8[0] >= '0' && strRecei->Data_a8[0] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = 0; i < strRecei->Length_u16; i++)
        {
            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i]>'9') break;
            else length++;
        }
        temp = Convert_String_To_Dec(strRecei->Data_a8 , length);
        if(temp > 0 && temp <= NUMBER_MOTOR)
        {
            if(Fix_Motor(temp) == 1)
                DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
            else
                DCU_Respond(PortConfig, (uint8_t*)"Handle: DELIVERY_BUSY\r\n", sizeof("Handle: DELIVERY_BUSY\r\n")-1, 0);
        }
        else
            DCU_Respond(PortConfig, (uint8_t*)"FAIL\r\n", 6, 0);
    }
    else
        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
}

/*
    @brief  Push motor
*/
void        _fAPP_PUSH_MOTOR (sData *strRecei, uint16_t Pos)
{
    int16_t temp1 = 0;
    int16_t temp2 = 0;
    if( strRecei->Data_a8[0] >= '0' && strRecei->Data_a8[0] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = 0; i < strRecei->Length_u16; i++)
        {
            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i]>'9') 
            {
              temp2 = strRecei->Data_a8[i+1] - 0x30;
              break;
            }
            else length++;
        }
        temp1 = Convert_String_To_Dec(strRecei->Data_a8 , length);
        
        if(temp1 <= NUMBER_MOTOR && temp2 <= NUMBER_PUSH_MOTOR)
        {
            if(sParamDelivery.StateHanlde == DELIVERY_FREE)
            {
                sParamDelivery.aDataPush[temp1-1] = temp2;
                Delivery_Entry();
                DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
            }
            else
              DCU_Respond(PortConfig, (uint8_t*)"Handle: DELIVERY_BUSY\r\n", sizeof("Handle: DELIVERY_BUSY\r\n")-1, 0);
        }
        else
            DCU_Respond(PortConfig, (uint8_t*)"FAIL\r\n", 6, 0);
    }
    else
        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
}

/*
    @brief  Doc thoi gian gui thong so van hanh
*/
void        _fGET_TIME_TSVH (sData *strRecei, uint16_t Pos)
{
    uint8_t aDataRespond[50]={0};
    uint16_t lengthRespond = 0;
    
    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"Time TSVH: ",0 , 11);
    Convert_Point_Int_To_String_Scale (aDataRespond, &lengthRespond, (int)(sParamPcBox.TimeTSVH), 0x00);
    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*) " Min\r\n",0 , 7);
    
    DCU_Respond(PortConfig, aDataRespond, lengthRespond, 0);
}

/*
    @brief  Cai dat thoi gian gui thong so van hanh 
*/
void        _fSET_TIME_TSVH (sData *strRecei, uint16_t Pos)
{
    uint8_t temp = 0;
    if( strRecei->Data_a8[0] >= '0' && strRecei->Data_a8[0] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = 0; i < strRecei->Length_u16; i++)
        {
            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i]>'9') break;
            else length++;
        }
        temp = Convert_String_To_Dec(strRecei->Data_a8 , length);
        Save_TimeTSVH(temp);
        DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
    }
    else
        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
}

/*
    @brief  Doc nhiet do cai dat tu lanh
*/
void        _fGET_SETUP_TEMP (sData *strRecei, uint16_t Pos)
{
    uint8_t aDataRespond[50]={0};
    uint16_t lengthRespond = 0;
    
    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"SetupTemp: ",0 , 11);
    Convert_Point_Int_To_String_Scale (aDataRespond, &lengthRespond, (int)(sTemp_Crtl_Fridge.TempSetup), 0xFF);
    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*) "°C\r\n",0 , 4);
    
    DCU_Respond(PortConfig, aDataRespond, lengthRespond, 0);
}
    
/*
    @brief Cai dat nhiet do tu lanh 
*/
void        _fSET_SETUP_TEMP (sData *strRecei, uint16_t Pos)
{
    int16_t temp = 0;
    if(strRecei->Data_a8[0] == '-')
    {
        uint16_t stamp = 0;
        uint8_t length = 0;
        strRecei->Data_a8[0] = '0';
        for(uint8_t i = 0; i < strRecei->Length_u16; i++)
        {
            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i]>'9') break;
            else length++;
        }
        stamp = Convert_String_To_Dec(strRecei->Data_a8 , length);
        temp  = 0 - stamp;
        Save_Threshold_Temperature(temp, DEFAULT_TEMP_SCALE);
    
        DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
    }
    else if( strRecei->Data_a8[0] >= '0' && strRecei->Data_a8[0] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = 0; i < strRecei->Length_u16; i++)
        {
            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i]>'9') break;
            else length++;
        }
        temp = Convert_String_To_Dec(strRecei->Data_a8 , length);
        Save_Threshold_Temperature(temp, DEFAULT_TEMP_SCALE);
    
        DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
    }
    else
        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
}

/*
    @brief  Doc gia tri nguong nhieu do dieu chinh
*/
void        _fGET_THRESH_TEMP (sData *strRecei, uint16_t Pos)
{ 
    uint8_t aDataRespond[50]={0};
    uint16_t lengthRespond = 0;
    
    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"ThreshTemp: ",0 , 12);
    Convert_Point_Int_To_String_Scale (aDataRespond, &lengthRespond, (int)(sTemp_Crtl_Fridge.Threshold), 0xFF);
    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*) "°C\r\n",0 , 4);
    
    DCU_Respond(PortConfig, aDataRespond, lengthRespond, 0);
}

/*
    @brief  Cai dat nguong dieu chinh nhiet do 
*/
void        _fSET_THRESH_TEMP (sData *strRecei, uint16_t Pos)
{
    int16_t temp = 0;
    if( strRecei->Data_a8[0] >= '0' && strRecei->Data_a8[0] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = 0; i < strRecei->Length_u16; i++)
        {
            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i] > '9') break;
            else length++;
        }
        temp = Convert_String_To_Dec(strRecei->Data_a8 , length);
        sTemp_Crtl_Fridge.Threshold = temp;
        Save_Threshold_Temperature(sTemp_Crtl_Fridge.TempSetup, DEFAULT_TEMP_SCALE);
        DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
    }
    else
        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
}

/*
    @brief  Doc ID Slave Electric
*/
void        _fGET_ID_SLAVE (sData *strRecei, uint16_t Pos)
{
    uint8_t aDataRespond[50]={0};
    uint16_t lengthRespond = 0;
    
    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"Slave ID: ",0 , 10);
    Convert_Point_Int_To_String_Scale (aDataRespond, &lengthRespond, (int)(sElectric.ID), 0x00);
    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*) "\r\n",0 , 2);
    
    DCU_Respond(PortConfig, aDataRespond, lengthRespond, 0);
}

/*
    @brief cai dat ID Slave Electric
*/
void        _fSET_ID_SLAVE (sData *strRecei, uint16_t Pos)
{
    int16_t temp = 0;
    if( strRecei->Data_a8[0] >= '0' && strRecei->Data_a8[0] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = 0; i < strRecei->Length_u16; i++)
        {
            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i]>'9') break;
            else length++;
        }
        temp = Convert_String_To_Dec(strRecei->Data_a8 , length);
        if(temp > 0 && temp < 256)
        {
            sElectric.ID = temp;
            Save_IdSlaveElectric();
            DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
        }
        else
            DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
    }
    else
        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
}

/*
    @brief  Dieu khien relay man hinh
*/
void        _fCTRL_RELAY_SCREEN (sData *strRecei, uint16_t Pos)
{
    if(strRecei->Data_a8[Pos] == '0' || strRecei->Data_a8[Pos] == '1')
        DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
      
    if(strRecei->Data_a8[Pos] - 0x30 == OFF_RELAY)
    {
        ControlRelay(RELAY_SCREEN, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_CTRL);
    }
    else if(strRecei->Data_a8[Pos] - 0x30 == ON_RELAY)
    {
        ControlRelay(RELAY_SCREEN, ON_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_CTRL);
    }
}

/*
    @brief  Dieu khien relay den
*/
void        _fCTRL_RELAY_LAMP (sData *strRecei, uint16_t Pos)
{
    if(strRecei->Data_a8[Pos] == '0' || strRecei->Data_a8[Pos] == '1')
        DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
    
    if(strRecei->Data_a8[Pos] - 0x30 == OFF_RELAY)
    {
        ControlRelay(RELAY_LAMP, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_CTRL);
    }
    else if(strRecei->Data_a8[Pos] - 0x30 == ON_RELAY)
    {
        if(sElectric.PowerPresent != POWER_OFF)
        {
            ControlRelay(RELAY_LAMP, ON_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_CTRL);
        }
        else
        {
            sStatusRelay.Lamp_Ctrl = ON_RELAY;
            ControlRelay(RELAY_LAMP, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_UNCTRL);
        }
    }
}

/*
    @brief  Dieu khien relay warm 
*/
void        _fCTRL_RELAY_WARM (sData *strRecei, uint16_t Pos)
{
//    if(strRecei->Data_a8[Pos] == '0' || strRecei->Data_a8[Pos] == '1')
//        DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
//    
//    if(strRecei->Data_a8[Pos] - 0x30 == OFF_RELAY)
//    {
//        fevent_active(sEventAppRelay, _EVENT_RELAY_WARM_OFF);
//    }
//    else if(strRecei->Data_a8[Pos] - 0x30 == ON_RELAY)
//    {
//        if(sElectric.PowerPresent != POWER_OFF)
//        {
//            OnRelay_Warm(sTimeCycleWarm.Run);
//            fevent_active(sEventAppRelay,_EVENT_RELAY_WARM_ON);
//        }
//    }
}


void        _fGET_TIME_WARM_RUN (sData *strRecei, uint16_t Pos)
{
//    uint8_t aDataRespond[50]={0};
//    uint16_t lengthRespond = 0;
//    
//    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"Time Warm Run: ",0 , 15);
//    Convert_Point_Int_To_String_Scale (aDataRespond, &lengthRespond, (int)(sTimeCycleWarm.Run), 0x00);
//    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*) " Min\r\n",0 , 7);
//    
//    DCU_Respond(PortConfig, aDataRespond, lengthRespond, 0);
}

void        _fSET_TIME_WARM_RUN (sData *strRecei, uint16_t Pos)
{
//    uint8_t temp = 0;
//    if( strRecei->Data_a8[0] >= '0' && strRecei->Data_a8[0] <= '9')
//    {
//        uint8_t length = 0;
//        for(uint8_t i = 0; i < strRecei->Length_u16; i++)
//        {
//            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i]>'9') break;
//            else length++;
//        }
//        temp = Convert_String_To_Dec(strRecei->Data_a8 , length);
//        if(Save_TimeRelayWarm(temp, sTimeCycleWarm.Wait) == 1)
//            DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
//        else
//            DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
//    }
//    else
//        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
}

void        _fGET_TIME_WARM_WAIT (sData *strRecei, uint16_t Pos)
{
//    uint8_t aDataRespond[50]={0};
//    uint16_t lengthRespond = 0;
//    
//    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"Time Warm Wait: ",0 , 16);
//    Convert_Point_Int_To_String_Scale (aDataRespond, &lengthRespond, (int)(sTimeCycleWarm.Wait), 0x00);
//    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*) " Min\r\n",0 , 7);
//    
//    DCU_Respond(PortConfig, aDataRespond, lengthRespond, 0);
}

void        _fSET_TIME_WARM_WAIT (sData *strRecei, uint16_t Pos)
{
//    uint8_t temp = 0;
//    if( strRecei->Data_a8[0] >= '0' && strRecei->Data_a8[0] <= '9')
//    {
//        uint8_t length = 0;
//        for(uint8_t i = 0; i < strRecei->Length_u16; i++)
//        {
//            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i]>'9') break;
//            else length++;
//        }
//        temp = Convert_String_To_Dec(strRecei->Data_a8 , length);
//        if(Save_TimeRelayWarm(sTimeCycleWarm.Run, temp) == 1)
//            DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
//        else
//            DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
//    }
//    else
//        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
}

void        _fGET_USING_CRC (sData *strRecei, uint16_t Pos)
{
    if(sParamPcBox.UsingCrc == _USING_CRC)
        DCU_Respond(PortConfig, (uint8_t*)"PcBox Using CRC\r\n", sizeof("PcBox Using CRC\r\n")-1, 0);
    else
        DCU_Respond(PortConfig, (uint8_t*)"PcBox NO CRC\r\n", sizeof("PcBox NO CRC\r\n")-1, 0);
}

void        _fSET_USING_CRC (sData *strRecei, uint16_t Pos)
{
    if( strRecei->Data_a8[0] >= '0' && strRecei->Data_a8[0] <= '9')
    {
      if(Save_UsingCrc(strRecei->Data_a8[0] - 0x30) ==1)
        DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
      else
        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
    }
    else
        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
}

void        _fGET_DEBUG_TEMP (sData *strRecei, uint16_t Pos)
{
    AppTemperature_Debug();
}

void        _fCALIB_TEMPERATURE (sData *strRecei, uint16_t Pos)
{
    int16_t temp = 0;
    uint8_t checkTemp = 0;
    
    if(strRecei->Data_a8[0] == '-')
      checkTemp = 1;
      
    if( strRecei->Data_a8[checkTemp] >= '0' && strRecei->Data_a8[checkTemp] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = checkTemp; i < strRecei->Length_u16; i++)
        {
            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i]>'9') break;
            else length++;
        }
        temp = Convert_String_To_Dec(strRecei->Data_a8 + checkTemp , length);
        
        if(checkTemp == 1)
          temp = 0 - temp;
        
        if(temp > -500 && temp < 500)
            Save_TempCalib(temp - sTemperature.Measure);
        else
            Save_TempCalib(0);
        
        DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
    }
    else
        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
}

void        _fGET_DEBUG_ELECTRIC (sData *strRecei, uint16_t Pos)
{
    AppElectric_Debug();
}

void        _fGET_CYCLE_ON_OFF_PC (sData *strRecei, uint16_t Pos)
{
#ifdef USING_APP_PC_BOX
    uint8_t aDataRespond[100]={0};
    uint16_t lengthRespond = 0;
    
    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"CyclePcBox: ",0 , 12);
    
    if(sCycleOnOffPcBox.State == _OFF_CYCLE)
        Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"OFF, ",0 , 5);
    else
        Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"ON, ",0 , 4);

    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"TimeOFF: ",0 , 9);
    Convert_Point_Int_To_String_Scale (aDataRespond, &lengthRespond, (int)(sCycleOnOffPcBox.HoursOFF), 0x00);
    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)":",0 , 1);
    
    Convert_Point_Int_To_String_Scale (aDataRespond, &lengthRespond, (int)(sCycleOnOffPcBox.MinutesOFF), 0x00);
    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)", TimeON: ",0 , 10);
    
    Convert_Point_Int_To_String_Scale (aDataRespond, &lengthRespond, (int)(sCycleOnOffPcBox.HoursON), 0x00);
    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)":",0 , 1);
    
    Convert_Point_Int_To_String_Scale (aDataRespond, &lengthRespond, (int)(sCycleOnOffPcBox.MinutesON), 0x00);
    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)".\r\n",0 , 3);

    DCU_Respond(PortConfig, aDataRespond, lengthRespond, 0);
#endif
}
void        _fSET_CYCLE_ON_OFF_PC (sData *strRecei, uint16_t Pos)
{
#ifdef  USING_APP_PC_BOX
    uint16_t i = 0;
    uint8_t State;
    uint8_t HoursOFF;
    uint8_t MinutesOFF;
    uint8_t HoursON;
    uint8_t MinutesON;
    
    State = Convert_String_To_Integer_Point(strRecei->Data_a8, strRecei->Length_u16, &i);
    HoursOFF = Convert_String_To_Integer_Point(strRecei->Data_a8, strRecei->Length_u16, &i);
    MinutesOFF = Convert_String_To_Integer_Point(strRecei->Data_a8, strRecei->Length_u16, &i);
    
    HoursON = Convert_String_To_Integer_Point(strRecei->Data_a8, strRecei->Length_u16, &i);
    MinutesON = Convert_String_To_Integer_Point(strRecei->Data_a8, strRecei->Length_u16, &i);
    
    if(Save_CycleOnOffPcBox(State, HoursOFF, MinutesOFF, HoursON, MinutesON) == 1)
        DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
    else
        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);

#endif
}

void        _fGET_STATE_PCBOX (sData *strRecei, uint16_t Pos)
{
    uint8_t aDataRespond[100]={0};
    uint16_t lengthRespond = 0;
    
    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"StatePcBox: ",0 , 12);
    switch(sParamPcBox.StatePcBox)
    {
        case _STT_PCBOX_CONNECT:
          Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"PcBox CONNECT",0 , 13);
          break;
          
        case _STT_PCBOX_DISCONNECT:
          Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"PcBox DISCONNECT",0 , 16);
          break;
          
        case _STT_PCBOX_SLEEP:
          Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"PcBox SLEEP",0 , 11);
          break;
          
        default:
          break;
    }

    Insert_String_To_String(aDataRespond, &lengthRespond, (uint8_t*)"\r\n",0 , 2);
    DCU_Respond(PortConfig, aDataRespond, lengthRespond, 0);
}

void        _fTEST_ELEVATOR (sData *strRecei, uint16_t Pos)
{
  if(sParamDelivery.StateHanlde == DELIVERY_FREE)
  {
    Enable_Test_Elevator();
    DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
  }
  else
  {
    DCU_Respond(PortConfig, (uint8_t*)"Elevator Busy\r\n", 15, 0);
  }
}

void        _fGET_CURRENT_WEIGHT (sData *strRecei, uint16_t Pos)
{
    if(sParamDelivery.StateConnectWeight == _WEIGHT_485_CONNECT)
    {
        uint8_t aData[10]={0};
        uint8_t length = 0;
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"Weight: ", sizeof("Weight: ")-1);
        length = Convert_Int_To_String(aData, sParamDelivery.Weighing);
        UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
    }
    else
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"Weight_Disconnect!!!", sizeof("Weight_Disconnect!!!")-1);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
}

void        _fTEST_LOCK_MAGNETIS (sData *strRecei, uint16_t Pos)
{
    int16_t temp = 0;
    if( strRecei->Data_a8[0] >= '0' && strRecei->Data_a8[0] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = 0; i < strRecei->Length_u16; i++)
        {
            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i]>'9') break;
            else length++;
        }
        temp = Convert_String_To_Dec(strRecei->Data_a8 , length);
        if(temp == 1)
        {
            Control_LockMagnetis(GPIO_PIN_SET);
            DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
        }
        else if(temp == 0)
        {
            Control_LockMagnetis(GPIO_PIN_RESET);
            DCU_Respond(PortConfig, (uint8_t*)"OK\r\n", 4, 0);
        }
        else 
        {
            DCU_Respond(PortConfig, (uint8_t*)"FAIL\r\n", 6, 0);
        }
    }
    else
        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
}

void        _fCHECK_TOP_ELEVATOR (sData *strRecei, uint16_t Pos)
{
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"TOP_ELEVATOR: ", sizeof("TOP_ELEVATOR: ")-1);

    if(HAL_GPIO_ReadPin(FB_Top_Elevator_GPIO_Port, FB_Top_Elevator_Pin) == GPIO_PIN_SET)
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"ON", sizeof("ON")-1);
    else
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"OFF", sizeof("OFF")-1);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
}

void        _fCHECK_BOT_ELEVATOR (sData *strRecei, uint16_t Pos)
{
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"BOT_ELEVATOR: ", sizeof("BOT_ELEVATOR: ")-1);

    if(HAL_GPIO_ReadPin(FB_Bot_Elevator_GPIO_Port, FB_Bot_Elevator_Pin) == GPIO_PIN_SET)
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"ON", sizeof("ON")-1);
    else
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"OFF", sizeof("OFF")-1);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
}

void        _fCHECK_LOCK_MAGNETIS (sData *strRecei, uint16_t Pos)
{
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"LOCK_MAGNETIS: ", sizeof("LOCK_MAGNETIS: ")-1);

    if(sParamDelivery.LockMagnetis == LOCK_MAGNETIS_OPEN)
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"OPEN", sizeof("OPEN")-1);
    else
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"CLOSE", sizeof("CLOSE")-1);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
}

void        _fELEVATOR_FLOOR (sData *strRecei, uint16_t Pos)
{
    int16_t temp = 0;
    if( strRecei->Data_a8[0] >= '0' && strRecei->Data_a8[0] <= '9')
    {
        uint8_t length = 0;
        for(uint8_t i = 0; i < strRecei->Length_u16; i++)
        {
            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i]>'9') break;
            else length++;
        }
        temp = Convert_String_To_Dec(strRecei->Data_a8 , length);
        if(temp <= NUMBER_MAX_FLOOR)
        {
            sElevator.FloorHandle = temp;
            fevent_enable(sEventAppDelivery, _EVENT_ELEVATOR_FLOOR);
            sParamDelivery.StateHanlde = DELIVERY_PURCHASE;
        }
    }
    else
        DCU_Respond(PortConfig, (uint8_t*)"ERROR\r\n", 7, 0);
}

