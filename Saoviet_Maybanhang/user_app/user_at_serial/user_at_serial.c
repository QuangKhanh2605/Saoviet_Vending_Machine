#include "user_at_serial.h"
#include "user_string.h"
#include "user_comm_vending_machine.h"
#include "user_inc_vending_machine.h"
#include "user_external_flash.h"
/*================ Struct Var =============*/

const struct_CheckList_AT CheckList_AT_CONFIG[] =
{
    {_RESET_DCU,        _fRESET_DCU,                {(uint8_t*)"at+resetdcu",11}},
  
    {_GET_SERI_DCU,		_fGET_SERI_DCU,		        {(uint8_t*)"at+seri?",8}},
    {_SET_SERI_DCU,     _fSET_SERI_DCU,             {(uint8_t*)"at+seri=",8}},
    
    {_GET_SETUP_TEMP,   _fGET_SETUP_TEMP,           {(uint8_t*)"at+setuptemp?",13}},
    {_SET_SETUP_TEMP,   _fSET_SETUP_TEMP,           {(uint8_t*)"at+setuptemp=",13}},
    
    {_GET_THRESH_TEMP,  _fGET_THRESH_TEMP,          {(uint8_t*)"at+threshtemp?",14}},
    {_SET_THRESH_TEMP,  _fSET_THRESH_TEMP,          {(uint8_t*)"at+threshtemp=",14}},
    
    {_GET_ID_SLAVE,     _fGET_ID_SLAVE,             {(uint8_t*)"at+slaveid?",11}},
    {_SET_ID_SLAVE,     _fSET_ID_SLAVE,             {(uint8_t*)"at+slaveid=",11}},
    
    {_END_AT_CMD,	    NULL,	                    {(uint8_t*)"at+end",6}},
};

uint8_t 		aDATA_CONFIG[128];

/*=============== Function Handle Receive =============*/
void        _fRESET_DCU (sData *strRecei, uint16_t Pos)
{
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"OK", 2);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", 2);
    Write_Status_Relay_ExFlash();
    Reset_Chip();
}

void        _fGET_SERI_DCU (sData *strRecei, uint16_t Pos)
{
    UTIL_Printf(DBLEVEL_L, sDCU_ID.Data_a8, sDCU_ID.Length_u16);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n"));
}

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

void        _fGET_SETUP_TEMP (sData *strRecei, uint16_t Pos)
{
    char cData[5]={0};
    uint8_t length = 0;
 
    length = Convert_Int_To_String_Scale(cData, (int)sTemp_Crtl_Fridge.TempSetup , sTemp_Crtl_Fridge.Scale);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"SetupTemp: ", sizeof("SetupTemp: "));
    UTIL_Printf(DBLEVEL_L, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n"));
}
    
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

void        _fGET_THRESH_TEMP (sData *strRecei, uint16_t Pos)
{
    char cData[5]={0};
    uint8_t length = 0;
 
    length = Convert_Int_To_String_Scale(cData, (int)sTemp_Crtl_Fridge.Threshold , sTemp_Crtl_Fridge.Scale);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"ThreshTemp: ", sizeof("ThreshTemp: "));
    UTIL_Printf(DBLEVEL_L, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n"));
}
    
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

void        _fGET_ID_SLAVE (sData *strRecei, uint16_t Pos)
{
    char cData[5]={0};
    uint8_t length = 0;
 
    length = Convert_Int_To_String(cData, (int)sElectric.ID);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"Slave ID: ", sizeof("Slave ID: "));
    UTIL_Printf(DBLEVEL_L, (uint8_t*)cData, length);
    UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n"));
}
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


