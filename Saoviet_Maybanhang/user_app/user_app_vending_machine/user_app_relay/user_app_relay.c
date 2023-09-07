
#include "user_app_relay.h"
#include "user_comm_vending_machine.h"
#include "user_app_pc_box.h"
#include "user_external_flash.h"
/*============== Function static ===============*/
static uint8_t fevent_relay_entry(uint8_t event);

static uint8_t fevent_control_led_status(uint8_t event);
static uint8_t fevent_control_led_pcbox(uint8_t event);
static uint8_t fevent_control_led_slave(uint8_t event);
/*=================== struct ==================*/
sEvent_struct               sEventAppRelay[] = 
{
  {_EVENT_RELAY_ENTRY,              1, 0, 5,                fevent_relay_entry},
  
  {_EVENT_CONTROL_LED_STATUS,       0, 5, TIME_LED_STATUS,  fevent_control_led_status},
  {_EVENT_CONTROL_LED_PCBOX,        0, 5, TIME_LED_PCBOX,   fevent_control_led_pcbox},
  {_EVENT_CONTROL_LED_SLAVE,        0, 5, TIME_LED_SLAVE,   fevent_control_led_slave},
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

static GPIO_TypeDef*  LED_PORT[3] = {Led_1_GPIO_Port, Led_2_GPIO_Port, Led_3_GPIO_Port};
static const uint16_t LED_PIN[3] = {Led_1_Pin, Led_2_Pin, Led_3_Pin};

uint8_t LedRecvPcBox = 0;
uint8_t ConnectSlave = 0;
/*================= Function Handle ==============*/
static uint8_t fevent_relay_entry(uint8_t event)
{
    fevent_active(sEventAppRelay, _EVENT_CONTROL_LED_STATUS);
    fevent_active(sEventAppRelay, _EVENT_CONTROL_LED_PCBOX);
    fevent_active(sEventAppRelay, _EVENT_CONTROL_LED_SLAVE);
    return 1;
}

static uint8_t fevent_control_led_status(uint8_t event)
{
    if(sEventAppRelay[_EVENT_CONTROL_LED_STATUS].e_period == TIME_LED_STATUS)
    {
        LED_On(_LED_STATUS);
        sEventAppRelay[_EVENT_CONTROL_LED_STATUS].e_period = 40;
    }
    else
    {
        LED_Off(_LED_STATUS);
        sEventAppRelay[_EVENT_CONTROL_LED_STATUS].e_period = TIME_LED_STATUS;
    }
    
    fevent_enable(sEventAppRelay, event);
    return 1;
}

static uint8_t fevent_control_led_pcbox(uint8_t event)
{
    if(LedRecvPcBox > 0)
    {
        if(sEventAppRelay[_EVENT_CONTROL_LED_PCBOX].e_period == TIME_LED_PCBOX)
        {
            LED_On(_LED_PCBOX);
            sEventAppRelay[_EVENT_CONTROL_LED_PCBOX].e_period = 40;
        }
        else
        {
            LED_Off(_LED_PCBOX);
            sEventAppRelay[_EVENT_CONTROL_LED_PCBOX].e_period = TIME_LED_PCBOX;
            LedRecvPcBox--;
        }
    }
    
    fevent_enable(sEventAppRelay, event);
    return 1;
}

static uint8_t fevent_control_led_slave(uint8_t event)
{
    if(ConnectSlave == CONNECT)
    {
        LED_Toggle(_LED_SLAVE);
    }
    else
    {
        LED_Off(_LED_SLAVE);
    }
    
    fevent_enable(sEventAppRelay, event);
    return 1;
}

/*========== Function Handle ============*/
void ControlRelay(uint8_t Relay, uint8_t State, uint8_t StateRespond, uint8_t RelayDebug)
{
  if(State == OFF_RELAY || State == ON_RELAY)
  {
    switch(Relay)
    {
          case RELAY_ELEVATOR: 
            OnOff_Relay(RELAY_ELEVATOR, State);
            sStatusRelay.Elevator = State;
            break;
            
          case RELAY_SCREEN: 
            OnOff_Relay(RELAY_SCREEN, State);
            sStatusRelay.Screen = State;
            Relay_Respond_Pc_Box(StateRespond, OBIS_ON_OFF_RELAY_SCREEN, State);
            break;
            
          case RELAY_FRIDGE_COOL: 
            OnOff_Relay(RELAY_FRIDGE_COOL, State);
            sStatusRelay.FridgeCool = State;
            break;
            
          case RELAY_ALARM: 
            OnOff_Relay(RELAY_ALARM, State);
            sStatusRelay.Alarm = State;
            break;
            
          case RELAY_FRIDGE_HEAT: 
            OnOff_Relay(RELAY_FRIDGE_HEAT, State);
            sStatusRelay.FridgeHeat = State;
            break;
            
          case RELAY_LAMP: 
            OnOff_Relay(RELAY_LAMP, State);
            sStatusRelay.Lamp = State;
            Relay_Respond_Pc_Box(StateRespond, OBIS_ON_OFF_RELAY_LAMP, State);
            break;
            
          case RELAY_WARM: 
            OnOff_Relay(RELAY_WARM, State);
            sStatusRelay.Warm = State;
            Relay_Respond_Pc_Box(StateRespond, OBIS_ON_OFF_RELAY_WARM, State);
            break;
        
         default:
            break;
    }
    
    Relay_Debug(RelayDebug, Relay, State);
  }
}

void OnOff_Relay(Relay_TypeDef Relay, uint8_t State)
{
    if(State == ON_RELAY)
    {
        HAL_GPIO_WritePin(RELAY_PORT[Relay], RELAY_PIN[Relay], GPIO_PIN_ON_RELAY);
    }
    else
    {
        HAL_GPIO_WritePin(RELAY_PORT[Relay], RELAY_PIN[Relay], GPIO_PIN_OFF_RELAY);
    }
}


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
        //if(aRead[2] <= 0x01) sStatusRelay.Elevator  = aRead[2];
        if(aRead[3] <= 0x01) sStatusRelay.Screen    = aRead[3];
        //if(aRead[4] <= 0x01) sStatusRelay.FridgeCool= aRead[4];
        //if(aRead[5] <= 0x01) sStatusRelay.Alarm     = aRead[5];
        //if(aRead[6] <= 0x01) sStatusRelay.FridgeHeat= aRead[6];
        if(aRead[7] <= 0x01) sStatusRelay.Lamp      = aRead[7];
        //if(aRead[8] <= 0x01) sStatusRelay.Warm      = aRead[8];
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

void Relay_Respond_Pc_Box(uint8_t State, uint8_t Obis, uint8_t Data)
{
  if(State == _RL_RESPOND)
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
}

void LED_Toggle (Led_TypeDef Led)
{
    HAL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);
}


void LED_On (Led_TypeDef Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
}


void LED_Off (Led_TypeDef Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
}

void Relay_Debug(uint8_t State_Debug, uint8_t Relay, uint8_t Status)
{
    if(State_Debug == _RL_DEBUG)
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

