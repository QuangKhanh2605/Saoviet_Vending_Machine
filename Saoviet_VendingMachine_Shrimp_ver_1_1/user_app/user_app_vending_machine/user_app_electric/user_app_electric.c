#include "user_app_electric.h"
#include "user_modbus_rtu.h"
#include "user_external_flash.h"
#include "user_inc_vending_machine.h"
/*============== Function static =============*/
static uint8_t fevent_electric_entry(uint8_t event);
static uint8_t fevent_electric_change_status_power(uint8_t event);
static uint8_t fevent_electric_check_connect(uint8_t event);
static uint8_t fevent_check_power_supply(uint8_t event);
/*============== Struct ===================*/
sEvent_struct               sEventAppElectric[] = 
{
  {_EVENT_ELECTRIC_ENTRY,               1, 5, TIME_ON_DCU,      fevent_electric_entry},
  
  {_EVENT_ELECTRIC_CHANGE_STATUS_POWER, 0, 5, 3000,             fevent_electric_change_status_power},
  
  {_EVENT_ELECTRIC_CHECK_CONNECT,       0, 5, 1000,             fevent_electric_check_connect},
  
  {_EVENT_CHECK_POWER_SUPPLY,           0, 0, 0,                fevent_check_power_supply},
};

Struct_Electric_Current         sElectric=
{
  .ID           = DEFAULT_ID_SLAVE,
  .Voltage      = 0, 
  .Current      = 0, 
  .ScaleVolCur  = DEFAULT_ELECTRIC_VOL_CUR_SCALE,
  .ScalePowEne  = DEFAULT_ELECTRIC_POW_ENE_SCALE,
  .PowerPresent = POWER_ERROR,
  .PowerBefore  = POWER_ERROR,
};

/*============= Function Handle =============*/
static uint8_t fevent_electric_entry(uint8_t event)
{
    fevent_enable(sEventAppElectric, _EVENT_ELECTRIC_CHANGE_STATUS_POWER);
    fevent_enable(sEventAppElectric, _EVENT_CHECK_POWER_SUPPLY);
    return 1;
}

static uint8_t fevent_electric_change_status_power(uint8_t event)
{
/*------------------Kiem tra thay doi trang thai nguon---------------*/
    if(sElectric.PowerPresent != sElectric.PowerBefore)
    {
        Status_Power_Respond_PcBox();

        sElectric.PowerBefore = sElectric.PowerPresent;
        Save_StatusElectric();
        
        if(sElectric.PowerPresent == POWER_OFF)
        {
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"app_electric: POWER OFF", sizeof("app_electric: POWER OFF")-1);
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
        }
        else if(sElectric.PowerPresent == POWER_ON)
        {
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"app_electric: POWER ON", sizeof("app_electric: POWER ON")-1);
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
        }
        else
        {
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"app_electric: POWER DISCONNECT", sizeof("app_electric: POWER DISCONNECT")-1);
            UTIL_Printf(DBLEVEL_L, (uint8_t*)"\r\n", sizeof("\r\n")-1);
        }
        Handle_State_Power();
    }
    
    fevent_enable(sEventAppElectric, event);
    return 1;
}

static uint8_t fevent_electric_check_connect(uint8_t event)
{
    return 1;
}

static uint8_t fevent_check_power_supply(uint8_t event)
{
    
    if(HAL_GPIO_ReadPin(DETECT_ELECTRIC_PORT, DETECT_ELECTRIC_PIN) == GPIO_PIN_RESET)
    {
        sElectric.PowerPresent = POWER_ON;
    }
    else
    {
        sElectric.PowerPresent = POWER_OFF;
    }
  
//    sElectric.PowerPresent = POWER_ON;
    
    fevent_enable(sEventAppElectric, event);
    return 1;
}

/*==================== Function Handle =====================*/
void Handle_State_Power(void)
{
/*-------------------Xu ly truong hop mat va co dien--------------*/
    if(sElectric.PowerPresent == POWER_OFF)
    {
        if(sStatusRelay.Lamp == ON_RELAY)
            ControlRelay(RELAY_LAMP, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_UNCTRL);
        
//        if(sStatusRelay.FridgeCool == ON_RELAY)
//            fevent_active(sEventAppTemperature, _EVENT_TEMP_OFF_FRIGE_FROZEN);
//        
//        if(sStatusRelay.FridgeHeat == ON_RELAY)
//            ControlRelay(RELAY_FRIDGE_HEAT, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_UNCTRL);
//        
        if(sStatusRelay.Warm == ON_RELAY)
            fevent_active(sEventAppRelay, _EVENT_RELAY_WARM_OFF);
        
        fevent_disable(sEventAppRelay,_EVENT_RELAY_WARM_ON);
    }
    else 
    {
        if(sStatusRelay.Lamp == OFF_RELAY)
        {
            if(sStatusRelay.Lamp_Ctrl == ON_RELAY)
                ControlRelay(RELAY_LAMP, ON_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_UNCTRL);
        }
    }
}

/*=================== Function Handle =====================*/
/*
    @brief  Gui trang thai canh bao nguon len PcBox
*/
void  Status_Power_Respond_PcBox(void)
{
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_WARNING_POWER;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sElectric.PowerPresent;
    
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
}

/*
    @brief  Luu trang thai nguon vao Flash
*/
void Save_StatusElectric(void)
{
    uint8_t aWrite[2]={0};
    aWrite[0] = BYTE_TEMP_FIRST_EXFLASH;
    aWrite[1] = sElectric.PowerPresent;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_STATUS_ELECTRIC);
    HAL_Delay(1);
    eFlash_S25FL_BufferWrite(aWrite, EX_FLASH_ADDR_STATUS_ELECTRIC, 2);
}

/*
    @brief  Doc trang thai nguon tu Flash
*/
void Init_StatusElectric(void)
{
    uint8_t aRead[2] = {0};
    eFlash_S25FL_BufferRead(aRead, EX_FLASH_ADDR_STATUS_ELECTRIC, 2);
    if(aRead[0] == BYTE_TEMP_FIRST_EXFLASH)
    {
        if(aRead[1] <= POWER_ERROR) 
        {
          sElectric.PowerPresent  = aRead[1];
          sElectric.PowerBefore   = aRead[1];
        }
    }
}

/*
    @brief  Viet Id Slave vao Flash
*/
void Save_IdSlaveElectric(void)
{
    uint8_t aWrite[2]={0};
    aWrite[0] = BYTE_TEMP_FIRST_EXFLASH;
    aWrite[1] = sElectric.ID;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_IDSLAVE_ELECTRIC);
    HAL_Delay(1);
    eFlash_S25FL_BufferWrite(aWrite, EX_FLASH_ADDR_IDSLAVE_ELECTRIC, 2);
}

/*
    @brief  Doc ID slave tu Flash
*/
void Init_IdSlaveElectric(void)
{
    uint8_t aRead[2] = {0};
    eFlash_S25FL_BufferRead(aRead, EX_FLASH_ADDR_IDSLAVE_ELECTRIC, 2);
    if( aRead[0] == BYTE_TEMP_FIRST_EXFLASH )
    {
        sElectric.ID  = aRead[1];
    }
}



/*
    @brief Debug app Electric
*/
void AppElectric_Debug(void)
{
#ifdef USING_APP_ELECTRIC_DEBUG
    uint8_t aData[12]={0};
    uint8_t length = 0;
    length = Convert_Int_To_String_Scale(aData, (int)sElectric.Voltage, sElectric.ScaleVolCur);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_electric: V: ", sizeof("app_electric: V: ")-1);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" A: ", sizeof(" A: ")-1);
    length = Convert_Int_To_String_Scale(aData, (int)sElectric.Current , sElectric.ScaleVolCur);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" P: ", sizeof(" P: ")-1);
    length = Convert_Int_To_String_Scale(aData, (int)sElectric.Power , sElectric.ScalePowEne);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" E: ", sizeof(" E: ")-1);
    length = Convert_Int_To_String_Scale(aData, (int)sElectric.Energy , sElectric.ScalePowEne);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)aData, length);
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
    
    
#endif
}
/*=======================Handle Task=======================*/
void Init_AppElectric(void)
{
    Init_IdSlaveElectric();
    Init_StatusElectric();
}

uint8_t  AppElectric_Task(void)
{
  
    uint8_t i = 0;
    uint8_t Result = false;
    for(i = 0; i < _EVENT_ELECTRIC_END; i++)
    {
        if(sEventAppElectric[i].e_status == 1)
        {
            Result = true;
            
            if((sEventAppElectric[i].e_systick == 0) ||
               ((HAL_GetTick() - sEventAppElectric[i].e_systick) >= sEventAppElectric[i].e_period))
            {
                sEventAppElectric[i].e_status = 0;  //Disable event
                sEventAppElectric[i].e_systick= HAL_GetTick();
                sEventAppElectric[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}
