#include "user_at_serial.h"
#include "user_string.h"
#include "user_comm_vending_machine.h"
#include "user_inc_vending_machine.h"
#include "user_external_flash.h"
/*================ Struct Var =============*/
const struct_CheckList_AT CheckList_AT_CONFIG[] =
{
    {_RESET_DCU,            _fRESET_DCU,                {(uint8_t*)"at+resetdcu",11}},      //Ex: "at+resetdcu"
    
    {_RESET_PCBOX,          _fRESET_PCBOX,              {(uint8_t*)"at+resetpcbox",13}},    //Ex: "at+resetpcbox"
    
    {_APP_FIX_MOTOR,        _fAPP_FIX_MOTOR,            {(uint8_t*)"at+fixmotor=",12}},     //Ex: "at+fixmotor=1"
    {_APP_PUSH_MOTOR,       _fAPP_PUSH_MOTOR,           {(uint8_t*)"at+pushmotor=",13}},    //Ex: "at+pushmotor=1 1"
  
    {_GET_SERI_DCU,		    _fGET_SERI_DCU,		        {(uint8_t*)"at+seri?",8}},          //Ex: "at+seri?"
    {_SET_SERI_DCU,         _fSET_SERI_DCU,             {(uint8_t*)"at+seri=",8}},          //Ex: "at+seri=SVVDMC23000001"
    
    {_GET_TIME_TSVH,		_fGET_TIME_TSVH,		    {(uint8_t*)"at+timetsvh?",12}},     //Ex: "at+timetsvh?"
    {_SET_TIME_TSVH,        _fSET_TIME_TSVH,            {(uint8_t*)"at+timetsvh=",12}},     //Ex: "at+timetsvh=1" (min)
    
    {_GET_TIME_PCBOX_RESET,	_fGET_TIME_PCBOX_RESET,		{(uint8_t*)"at+timepcboxreset?",18}},//Ex: "at+timepcboxreset?"
    {_SET_TIME_PCBOX_RESET, _fSET_TIME_PCBOX_RESET,     {(uint8_t*)"at+timepcboxreset=",18}},//Ex: "at+timepcboxreset=15" (min)
    
    {_GET_SETUP_TEMP,       _fGET_SETUP_TEMP,           {(uint8_t*)"at+setuptemp?",13}},    //Ex: "at+setuptemp?"        
    {_SET_SETUP_TEMP,       _fSET_SETUP_TEMP,           {(uint8_t*)"at+setuptemp=",13}},    //Ex: "at+setuptemp=50"(Scale 0xFF)
    
    {_GET_THRESH_TEMP,      _fGET_THRESH_TEMP,          {(uint8_t*)"at+threshtemp?",14}},   //Ex: "at+threshtemp?"
    {_SET_THRESH_TEMP,      _fSET_THRESH_TEMP,          {(uint8_t*)"at+threshtemp=",14}},   //Ex: "at+threshtemp=25"(Scale 0xFF)
    
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
    
    {_END_AT_CMD,	        NULL,	                    {(uint8_t*)"at+end",6}},
};

uint8_t 		aDATA_CONFIG[128];

/*=============== Function Handle Receive =============*/

/*
    @brief  Reset DCU
*/
void        _fRESET_DCU (sData *strRecei, uint16_t Pos)
{
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
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
        Fix_Motor(temp);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
    else
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"ERROR", 5);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
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
        Push_Motor(temp1, temp2);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
    else
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"ERROR", 5);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
}

/*
    @brief  Doc thoi gian gui thong so van hanh
*/
void        _fGET_TIME_TSVH (sData *strRecei, uint16_t Pos)
{
    char cData[5]={0};
    uint8_t length = 0;
 
    length = Convert_Int_To_String_Scale(cData, (int)sParamPcBox.TimeTSVH , 0x00);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"Time TSVH: ", sizeof("Time TSVH: ")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)" Min", sizeof(" Min")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
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
        Set_TimeTSVH(temp);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
    else
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"ERROR", 5);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
}

/*
    @brief  Doc thoi gian cho reset PcBox
*/
void        _fGET_TIME_PCBOX_RESET (sData *strRecei, uint16_t Pos)
{
    char cData[5]={0};
    uint8_t length = 0;
 
    length = Convert_Int_To_String_Scale(cData, (int)sParamPcBox.TimeResetPcBox , 0x00);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"Time TSVH: ", sizeof("Time TSVH: ")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)" Min", sizeof(" Min")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
}

/*
    @brief  Cai dat thoi gian cho reset PcBox
*/
void        _fSET_TIME_PCBOX_RESET (sData *strRecei, uint16_t Pos)
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
        Set_TimeResetPcBox(temp);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
    else
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"ERROR", 5);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
}

/*
    @brief  Doc Seri DCU
*/
void        _fGET_SERI_DCU (sData *strRecei, uint16_t Pos)
{
    UTIL_Printf(DBLEVEL_L, sDCU_ID.Data_a8, sDCU_ID.Length_u16);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
}

/*
    @brief  Cai dat seri DCU
*/
void        _fSET_SERI_DCU (sData *strRecei, uint16_t Pos)
{
    uint8_t write[MAX_LENGTH_DCU_ID+2] = {0};
    
    write[0]= DEFAULT_READ_EXFLASH;
    write[1]= strRecei->Length_u16;
    
    sDCU_ID.Length_u16 = 0;
    UTIL_MEM_set(sDCU_ID.Data_a8, 0x00, MAX_LENGTH_DCU_ID);
    for(uint8_t i = Pos; i < strRecei->Length_u16; i++)
    {
        if(i < MAX_LENGTH_DCU_ID)
        {
            write[i+2] = strRecei->Data_a8[i];
            sDCU_ID.Data_a8[sDCU_ID.Length_u16++] = strRecei->Data_a8[i];
        }
        else break;
    }
    
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_MAIN_ID);
    eFlash_S25FL_BufferWrite(write, EX_FLASH_ADDR_MAIN_ID, strRecei->Length_u16+2);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
}

/*
    @brief  Doc nhiet do cai dat tu lanh
*/
void        _fGET_SETUP_TEMP (sData *strRecei, uint16_t Pos)
{
    char cData[5]={0};
    uint8_t length = 0;
 
    length = Convert_Int_To_String_Scale(cData, (int)sTemp_Crtl_Fridge.TempSetup , sTemp_Crtl_Fridge.Scale);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"SetupTemp: ", sizeof("SetupTemp: ")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"°C", sizeof("°C")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
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
        Set_Threshold_Temperature(temp, DEFAULT_TEMP_SCALE);
    
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
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
        Set_Threshold_Temperature(temp, DEFAULT_TEMP_SCALE);
    
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
    else
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"ERROR", 5);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }

}

/*
    @brief  Doc gia tri nguong nhieu do dieu chinh
*/
void        _fGET_THRESH_TEMP (sData *strRecei, uint16_t Pos)
{
    char cData[5]={0};
    uint8_t length = 0;
 
    length = Convert_Int_To_String_Scale(cData, (int)sTemp_Crtl_Fridge.Threshold , sTemp_Crtl_Fridge.Scale);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"ThreshTemp: ", sizeof("ThreshTemp: ")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"°C", sizeof("°C")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
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
            if( strRecei->Data_a8[i] < '0' || strRecei->Data_a8[i]>'9') break;
            else length++;
        }
        temp = Convert_String_To_Dec(strRecei->Data_a8 , length);
        sTemp_Crtl_Fridge.Threshold = temp;
        Set_Threshold_Temperature(sTemp_Crtl_Fridge.TempSetup, DEFAULT_TEMP_SCALE);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
    else
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"ERROR", 5);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
}

/*
    @brief  Doc ID Slave Electric
*/
void        _fGET_ID_SLAVE (sData *strRecei, uint16_t Pos)
{
    char cData[5]={0};
    uint8_t length = 0;
 
    length = Convert_Int_To_String(cData, (int)sElectric.ID);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"Slave ID: ", sizeof("Slave ID: ")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
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
            Write_IdSlave_Electric_ExFlash();
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
        }
        else
        {
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"ERROR", 5);
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
        }
    }
    else
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"ERROR", 5);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
}

/*
    @brief  Dieu khien relay man hinh
*/
void        _fCTRL_RELAY_SCREEN (sData *strRecei, uint16_t Pos)
{
    if(strRecei->Data_a8[Pos] == '0' || strRecei->Data_a8[Pos] == '1')
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
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
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
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
    if(strRecei->Data_a8[Pos] == '0' || strRecei->Data_a8[Pos] == '1')
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
    if(strRecei->Data_a8[Pos] - 0x30 == OFF_RELAY)
    {
        fevent_active(sEventAppRelay, _EVENT_RELAY_WARM_OFF);
    }
    else if(strRecei->Data_a8[Pos] - 0x30 == ON_RELAY)
    {
        if(sElectric.PowerPresent != POWER_OFF)
        {
            OnRelay_Warm(sTimeCycleWarm.Run);
            fevent_active(sEventAppRelay,_EVENT_RELAY_WARM_ON);
        }
    }
}


void        _fGET_TIME_WARM_RUN (sData *strRecei, uint16_t Pos)
{
    char cData[5]={0};
    uint8_t length = 0;
 
    length = Convert_Int_To_String_Scale(cData, (int)sTimeCycleWarm.Run , 0x00);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"Time Warm Run: ", sizeof("Time Warm Run: ")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)" Min", sizeof(" Min")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
}

void        _fSET_TIME_WARM_RUN (sData *strRecei, uint16_t Pos)
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
        Setup_TimeCycle_Relay_Warm(temp, sTimeCycleWarm.Wait);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
    else
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"ERROR", 5);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
}

void        _fGET_TIME_WARM_WAIT (sData *strRecei, uint16_t Pos)
{
    char cData[5]={0};
    uint8_t length = 0;
 
    length = Convert_Int_To_String_Scale(cData, (int)sTimeCycleWarm.Wait , 0x00);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"Time Warm Wait: ", sizeof("Time Warm Wait: ")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)" Min", sizeof(" Min")-1);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
}

void        _fSET_TIME_WARM_WAIT (sData *strRecei, uint16_t Pos)
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
        Setup_TimeCycle_Relay_Warm(sTimeCycleWarm.Run, temp);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
    else
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"ERROR", 5);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
}

void        _fGET_USING_CRC (sData *strRecei, uint16_t Pos)
{
    if(sParamPcBox.UsingCrc == _USING_CRC)
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"PcBox Using CRC", sizeof("PcBox Using CRC")-1);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n"));
    }
    else
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"PcBox NO CRC", sizeof("PcBox NO CRC")-1);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
    }
}

void        _fSET_USING_CRC (sData *strRecei, uint16_t Pos)
{
    if( strRecei->Data_a8[0] >= '0' && strRecei->Data_a8[0] <= '9')
    {
      if(strRecei->Data_a8[0] == '0')
      {
        sParamPcBox.UsingCrc = _UNUSING_CRC;
      }
      else if(strRecei->Data_a8[0] == '1')
      {
        sParamPcBox.UsingCrc = _USING_CRC;
      }
      Write_Flash_Using_Crc();
    
      UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
      UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
    else
    {
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"ERROR", 5);
        UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    }
}
/*================== Function Handle ====================*/
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

	for (var = 0; var <= _END_AT_CMD; var++)
	{
		Pos_Str = Find_String_V2((sData*) &CheckList_AT_CONFIG[var].sTempReceiver, StrUartRecei);
		if ((Pos_Str >= 0) && (CheckList_AT_CONFIG[var].CallBack != NULL))
		{

//            //Copy lenh vao buff. de repond kem theo lenh
//            Reset_Buff(&sModem.strATCmd);
//
//            for (i = 0; i <(StrUartRecei->Length_u16 - Pos_Str); i++)
//            {
//                if ((*(StrUartRecei->Data_a8 + Pos_Str + i) == '?') || (*(StrUartRecei->Data_a8+Pos_Str+i) == '=') \
//                    || (sModem.strATCmd.Length_u16 >= LEGNTH_AT_SERIAL))
//                    break;
//                else 
//                    *(sModem.strATCmd.Data_a8 + sModem.strATCmd.Length_u16++) = *(StrUartRecei->Data_a8+Pos_Str+i); 
//            }
          
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
            
//            PortConfig = Type;
            
            //Test
            switch (Type)
            {
                case _AT_REQUEST_SERIAL:
                case _AT_REQUEST_SERVER: 
                    //check at request
                    CheckList_AT_CONFIG[var].CallBack(&sDataConfig, 0);
                    break;
//                case _AT_REQUEST_LORA:       
//                #ifdef USING_APP_LORA
//                    //Copy data to lora at cmd handle
//                    shLora.LengthConfig_u8 = 0;
//                
//                    for (i = 0; i < CheckList_AT_CONFIG[var].sTempReceiver.Length_u16; i++)
//                    {
//                        shLora.aDATA_CONFIG[shLora.LengthConfig_u8++] = *(CheckList_AT_CONFIG[var].sTempReceiver.Data_a8 + i); 
//                    } 
//                        
//                    for (i = 0; i <sDataConfig.Length_u16; i++)
//                    {
//                        shLora.aDATA_CONFIG[shLora.LengthConfig_u8++] = *(sDataConfig.Data_a8 + i); 
//                    }
//                    
//                    shLora.rAtcmd_u8 = TRUE;  
//                
//                    #ifndef DEVICE_TYPE_GATEWAY 
//                        CheckList_AT_CONFIG[var].CallBack(&sDataConfig, 0);
//                    #endif
//                #endif
//                    break;
                default:
                    break;
            }

			return 1;
		}
	}

	return 0;
}


