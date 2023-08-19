
#include "user_app_relay.h"
#include "user_comm_vending_machine.h"
#include "user_app_pc_box.h"
#include "user_external_flash.h"
/*============== Function static ===============*/
static uint8_t fevent_relay_entry(uint8_t event);
static uint8_t fevent_on_off_relay_elevator(uint8_t event);
static uint8_t fevent_on_off_relay_screen(uint8_t event);
static uint8_t fevent_on_off_relay_fridge_cool(uint8_t event);
static uint8_t fevent_on_off_relay_alarm(uint8_t event);
static uint8_t fevent_on_off_relay_fridge_heat(uint8_t event);
static uint8_t fevent_on_off_relay_lamp(uint8_t event);
static uint8_t fevent_on_off_relay_warm(uint8_t event);
/*=================== struct ==================*/
sEvent_struct               sEventAppRelay[] = 
{
  {_EVENT_RELAY_ENTRY,              1, 5, 2000,     fevent_relay_entry},
  {_EVENT_ON_OFF_RELAY_ELEVATOR,    0, 0, 5,        fevent_on_off_relay_elevator},
  {_EVENT_ON_OFF_RELAY_SCREEN,      0, 0, 5,        fevent_on_off_relay_screen},
  {_EVENT_ON_OFF_RELAY_FRIDGE_COOL, 0, 0, 5,        fevent_on_off_relay_fridge_cool},
  {_EVENT_ON_OFF_RELAY_ALARM,       0, 0, 5,        fevent_on_off_relay_alarm},
  {_EVENT_ON_OFF_RELAY_FRIDGE_HEAT, 0, 0, 5,        fevent_on_off_relay_fridge_heat},
  {_EVENT_ON_OFF_RELAY_LAMP,        0, 0, 5,        fevent_on_off_relay_lamp},
  {_EVENT_ON_OFF_RELAY_WARM,        0, 0, 5,        fevent_on_off_relay_warm},
};

Struct_StatusRelay          sStatusRelay={OFF_RELAY};                

/*
    Relay PC            ->      Elevator
    Relay Screen        ->      Screen
    Relay Fridge        ->      Fridge Frozen
    Relay Alarm         ->      Fridge Alarm
    Relay 5             ->      Fridge Heat
    Motor Elevator      ->      Lamp
    Layer 7             ->      Warm
*/

static GPIO_TypeDef*    RELAY_PORT[NUMBER_RELAY] = {ON_OFF_PC_GPIO_Port, 
                                                    ON_OFF_Screen_GPIO_Port, 
                                                    ON_OFF_Fridge_GPIO_Port, 
                                                    ON_OFF_Alarm_GPIO_Port,
                                                    ON_OFF_Relay_5_GPIO_Port, 
                                                    Motor_GPIO_Port,
                                                    Layer_7_GPIO_Port};

static uint16_t         RELAY_PIN[NUMBER_RELAY] = {ON_OFF_PC_Pin, 
                                                   ON_OFF_Screen_Pin, 
                                                   ON_OFF_Fridge_Pin, 
                                                   ON_OFF_Alarm_Pin,
                                                   ON_OFF_Relay_5_Pin, 
                                                   Motor_Pin,
                                                   Layer_7_Pin};
/*================= Function Handle ==============*/
static uint8_t fevent_relay_entry(uint8_t event)
{
    Respond_PcBox((uint8_t*)"ON",2);
    return 1;
}

static uint8_t fevent_on_off_relay_elevator(uint8_t event)
{
    if(sStatusRelay.Elevator == ON_RELAY)
    {
        On_Relay(RELAY_ELEVATOR);
        AppRelay_Debug(ON_RELAY, RELAY_ELEVATOR);
    }
    else if(sStatusRelay.Elevator == OFF_RELAY)
    {
        Off_Relay(RELAY_ELEVATOR);
        AppRelay_Debug(OFF_RELAY, RELAY_ELEVATOR);
    }
    return 1;
}

static uint8_t fevent_on_off_relay_screen(uint8_t event)
{
    if(sStatusRelay.Screen == ON_RELAY)
    {
        On_Relay(RELAY_SCREEN);
        AppRelay_Debug(ON_RELAY, RELAY_SCREEN);
        
        Relay_Respond_Pc_Box_Control(OBIS_ON_OFF_RELAY_SCREEN, ON_RELAY);
    }
    else if(sStatusRelay.Screen == OFF_RELAY)
    {
        Off_Relay(RELAY_SCREEN);
        AppRelay_Debug(OFF_RELAY, RELAY_SCREEN);
        
        Relay_Respond_Pc_Box_Control(OBIS_ON_OFF_RELAY_SCREEN, OFF_RELAY);
    }
    
    return 1;
}

static uint8_t fevent_on_off_relay_fridge_cool(uint8_t event)
{
    if(sStatusRelay.FridgeCool == ON_RELAY)
    {
        On_Relay(RELAY_FRIDGE_COOL);
        //AppRelay_Debug(ON_RELAY, RELAY_FRIDGE_COOL);
    }
    else if(sStatusRelay.FridgeCool == OFF_RELAY)
    {
        Off_Relay(RELAY_FRIDGE_COOL);
        //AppRelay_Debug(OFF_RELAY, RELAY_FRIDGE_COOL);
    }

    return 1;
}

static uint8_t fevent_on_off_relay_alarm(uint8_t event)
{
    if(sStatusRelay.Alarm == ON_RELAY)
    {
        On_Relay(RELAY_ALARM);
        AppRelay_Debug(ON_RELAY, RELAY_ALARM);
    }
    else if(sStatusRelay.Alarm == OFF_RELAY)
    {
        Off_Relay(RELAY_ALARM);
        AppRelay_Debug(OFF_RELAY, RELAY_ALARM);
    }
        
    return 1;
}

static uint8_t fevent_on_off_relay_fridge_heat(uint8_t event)
{
    if(sStatusRelay.FridgeHeat == ON_RELAY)
    {
        On_Relay(RELAY_FRIDGE_HEAT);
        //AppRelay_Debug(ON_RELAY, RELAY_FRIDGE_HEAT);
    }
    else if(sStatusRelay.FridgeHeat == OFF_RELAY)
    {
        Off_Relay(RELAY_FRIDGE_HEAT);
        //AppRelay_Debug(OFF_RELAY, RELAY_FRIDGE_HEAT);
    }

    return 1;
}

static uint8_t fevent_on_off_relay_lamp(uint8_t event)
{
    if(sStatusRelay.Lamp == ON_RELAY)
    {
        On_Relay(RELAY_LAMP);
        AppRelay_Debug(ON_RELAY, RELAY_LAMP);
    }
    else if(sStatusRelay.Lamp == OFF_RELAY)
    {
        Off_Relay(RELAY_LAMP);
        AppRelay_Debug(OFF_RELAY, RELAY_LAMP);
    }
    
    return 1;
}

static uint8_t fevent_on_off_relay_warm(uint8_t event)
{
    if(sStatusRelay.Warm == ON_RELAY)
    {
        On_Relay(RELAY_WARM);
        AppRelay_Debug(ON_RELAY, RELAY_WARM);
    }
    else if(sStatusRelay.Warm == OFF_RELAY)
    {
        Off_Relay(RELAY_WARM);
        AppRelay_Debug(OFF_RELAY, RELAY_WARM);
    }
    
    return 1;
}

/*========== Function Handle ============*/
void Init_AppRelay(void)
{
    Read_Status_Relay_ExFlash();
    Init_StatusRelay();
}

void Write_Status_Relay_ExFlash(void)
{
    uint8_t aWrite[9]={0};
    aWrite[0] = DEFAULT_READ_EXFLASH;
    aWrite[1] = NUMBER_RELAY;
    aWrite[2] = sStatusRelay.Elevator;
    aWrite[3] = sStatusRelay.Screen;
    aWrite[4] = sStatusRelay.FridgeCool;
    aWrite[5] = sStatusRelay.Alarm;
    aWrite[6] = sStatusRelay.FridgeHeat;
    aWrite[7] = sStatusRelay.Lamp;
    aWrite[8] = sStatusRelay.Warm;
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_STATUS_RELAY);
    HAL_Delay(1);
    eFlash_S25FL_BufferWrite(aWrite, EX_FLASH_ADDR_STATUS_RELAY, 9);
}

void Read_Status_Relay_ExFlash(void)
{
    uint8_t aRead[9] = {0};
    eFlash_S25FL_BufferRead(aRead, EX_FLASH_ADDR_STATUS_RELAY, 9);
    if( aRead[0] == DEFAULT_READ_EXFLASH && aRead[1] == NUMBER_RELAY)
    {
        if(aRead[2] <= 0x01) sStatusRelay.Elevator  = aRead[2];
        if(aRead[3] <= 0x01) sStatusRelay.Screen    = aRead[3];
        if(aRead[4] <= 0x01) sStatusRelay.FridgeCool= aRead[4];
        if(aRead[5] <= 0x01) sStatusRelay.Alarm     = aRead[5];
        if(aRead[6] <= 0x01) sStatusRelay.FridgeHeat= aRead[6];
        if(aRead[7] <= 0x01) sStatusRelay.Lamp      = aRead[7];
        if(aRead[8] <= 0x01) sStatusRelay.Warm      = aRead[8];
    }
}

void Init_StatusRelay(void)
{
  if(sStatusRelay.Elevator == 1)    HAL_GPIO_WritePin(RELAY_PORT[0], RELAY_PIN[0], GPIO_PIN_ON_RELAY);
  else                              HAL_GPIO_WritePin(RELAY_PORT[0], RELAY_PIN[0], GPIO_PIN_OFF_RELAY);
  
  if(sStatusRelay.Screen == 1)      HAL_GPIO_WritePin(RELAY_PORT[1], RELAY_PIN[1], GPIO_PIN_ON_RELAY);
  else                              HAL_GPIO_WritePin(RELAY_PORT[1], RELAY_PIN[1], GPIO_PIN_OFF_RELAY);
  
  if(sStatusRelay.FridgeCool == 1)  HAL_GPIO_WritePin(RELAY_PORT[2], RELAY_PIN[2], GPIO_PIN_ON_RELAY);
  else                              HAL_GPIO_WritePin(RELAY_PORT[2], RELAY_PIN[2], GPIO_PIN_OFF_RELAY);
  
  if(sStatusRelay.Alarm == 1)       HAL_GPIO_WritePin(RELAY_PORT[3], RELAY_PIN[3], GPIO_PIN_ON_RELAY);
  else                              HAL_GPIO_WritePin(RELAY_PORT[3], RELAY_PIN[3], GPIO_PIN_OFF_RELAY);
  
  if(sStatusRelay.FridgeHeat == 1)  HAL_GPIO_WritePin(RELAY_PORT[4], RELAY_PIN[4], GPIO_PIN_ON_RELAY);
  else                              HAL_GPIO_WritePin(RELAY_PORT[4], RELAY_PIN[4], GPIO_PIN_OFF_RELAY);
  
  if(sStatusRelay.Lamp == 1)        HAL_GPIO_WritePin(RELAY_PORT[5], RELAY_PIN[5], GPIO_PIN_ON_RELAY);
  else                              HAL_GPIO_WritePin(RELAY_PORT[5], RELAY_PIN[5], GPIO_PIN_OFF_RELAY);
  
  if(sStatusRelay.Warm == 1)        HAL_GPIO_WritePin(RELAY_PORT[6], RELAY_PIN[6], GPIO_PIN_ON_RELAY);
  else                              HAL_GPIO_WritePin(RELAY_PORT[6], RELAY_PIN[6], GPIO_PIN_OFF_RELAY);
}

void Relay_Respond_Pc_Box_Control(uint8_t Obis, uint8_t Data)
{
    uint8_t aData[5];
    uint8_t length = 0;
    uint16_t TempCrc = 0;
    
/*=============== Log ===============*/
    
    aData[length++] = Obis;
    aData[length++] = 0x01;
    aData[length++] = Data;
    
    Calculator_Crc_U16(&TempCrc, aData, length);
    
    aData[length++] = TempCrc;
    aData[length++] = TempCrc >> 8;
    
    Write_Queue_Repond_PcBox(aData, length);
}

void AppRelay_Debug(uint8_t Status, uint8_t Relay)
{
#ifdef USING_APP_RELAY_DEBUG
    if(Status == ON_RELAY)
    {
      UTIL_Printf(DBLEVEL_M, (uint8_t*)"user_app_relay: ON Relay: ", sizeof("user_app_relay: ON Relay: "));
    }
    else
    {
      UTIL_Printf(DBLEVEL_M, (uint8_t*)"user_app_relay: OFF Relay: ", sizeof("user_app_relay: OFF Relay: "));
    }
    
    switch(Relay)
    {
        case RELAY_ELEVATOR:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"ELEVATOR ", sizeof("ELEVATOR ")); 
           break;
           
        case RELAY_SCREEN:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"SCREEN ", sizeof("SCREEN ")); 
           break;
           
        case RELAY_FRIDGE_COOL:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"FRIDGE COOL ", sizeof("FRIDGE COOL ")); 
           break;
           
        case RELAY_ALARM:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"ALARM ", sizeof("ALARM ")); 
           break;
           
        case RELAY_FRIDGE_HEAT:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"FRIDGE HEAT ", sizeof("FRIDGE HEAT ")); 
           break;
           
        case RELAY_LAMP:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"LAMP ", sizeof("LAMP ")); 
           break;
           
        case RELAY_WARM:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"WARM ", sizeof("WARM ")); 
           break;
           
        default:
           break;
    }
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")); 
#endif
}

void On_Relay(Relay_TypeDef Relay)
{
    HAL_GPIO_WritePin(RELAY_PORT[Relay], RELAY_PIN[Relay], GPIO_PIN_ON_RELAY);
}

void Off_Relay(Relay_TypeDef Relay)
{
    HAL_GPIO_WritePin(RELAY_PORT[Relay], RELAY_PIN[Relay], GPIO_PIN_OFF_RELAY);
}

uint8_t AppRelay_Task(void)
{
    uint8_t i = 0;
    uint8_t Result = false;
    for( i = 0; i < _EVENT_RELAY_END; i++)
    {
        if(sEventAppRelay[i].e_status == 1)
        {
            Result = true;
            if((sEventAppRelay[i].e_systick == 0) ||
                ((HAL_GetTick() - sEventAppRelay[i].e_systick) >= sEventAppRelay[i].e_period))
            {
                sEventAppRelay[i].e_status = 0; //Disable event
                sEventAppRelay[i].e_systick= HAL_GetTick();
                sEventAppRelay[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}

