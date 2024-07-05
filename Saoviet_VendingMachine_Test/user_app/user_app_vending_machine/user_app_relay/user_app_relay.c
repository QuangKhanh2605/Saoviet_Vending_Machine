#include "user_app_relay.h"
#include "user_external_flash.h"
#include "user_inc_vending_machine.h"
/*============== Function static ===============*/
static uint8_t fevent_relay_entry(uint8_t event);

static uint8_t fevent_relay_warm_refresh(uint8_t event);
static uint8_t fevent_relay_warm_on(uint8_t event);
static uint8_t fevent_relay_warm_off(uint8_t event);

static uint8_t fevent_control_led_status(uint8_t event);
static uint8_t fevent_control_led_pcbox(uint8_t event);
static uint8_t fevent_control_led_slave(uint8_t event);
/*=================== struct ==================*/
sEvent_struct               sEventAppRelay[] = 
{
  {_EVENT_RELAY_ENTRY,              1, 0, 5,                    fevent_relay_entry},
  
  {_EVENT_RELAY_WARM_REFRESH,       0, 5, TIME_RL_WARM_REFRESH, fevent_relay_warm_refresh},
  {_EVENT_RELAY_WARM_ON,            0, 5, 5,                    fevent_relay_warm_on},
  {_EVENT_RELAY_WARM_OFF,           0, 0, 5,                    fevent_relay_warm_off},
  
  {_EVENT_CONTROL_LED_STATUS,       0, 5, TIME_LED_STATUS,      fevent_control_led_status},
  {_EVENT_CONTROL_LED_PCBOX,        0, 5, TIME_LED_PCBOX,       fevent_control_led_pcbox},
  {_EVENT_CONTROL_LED_SLAVE,        0, 5, TIME_LED_SLAVE,       fevent_control_led_slave},
};

Struct_StatusRelay          sStatusRelay={OFF_RELAY};                

/*
    Layer 7             ->      Elevator
    Relay PC            ->      Warm
    Relay Screen        ->      Screen
    Relay Fridge        ->      Fridge Frozen
    Relay Alarm         ->      Fridge Alarm
    Relay 5             ->      Fridge Heat
    Motor Elevator      ->      Lamp
*/

static GPIO_TypeDef*    RELAY_PORT[NUMBER_RELAY] = {RELAY_SCREEN_PORT, 
                                                    RELAY_FRIDGE_COOL_PORT, 
                                                    RELAY_ALARM_PORT, 
                                                    RELAY_FRIDGE_HEAT_PORT,
                                                    RELAY_LAMP_PORT, 
                                                    RELAY_WARM_PORT,
                                                    RELAY_PC_PORT,
                                                    RELAY_V1_PORT,
                                                    RELAY_V2_PORT};

static uint16_t         RELAY_PIN[NUMBER_RELAY] = {RELAY_SCREEN_PIN, 
                                                   RELAY_FRIDGE_COOL_PIN, 
                                                   RELAY_ALARM_PIN, 
                                                   RELAY_FRIDGE_HEAT_PIN,
                                                   RELAY_LAMP_PIN, 
                                                   RELAY_WARM_PIN,
                                                   RELAY_PC_PIN,
                                                   RELAY_V1_PIN,
                                                   RELAY_V2_PIN,};

static GPIO_TypeDef*  LED_PORT[3] = {Led_1_GPIO_Port, Led_2_GPIO_Port, Led_3_GPIO_Port};
static const uint16_t LED_PIN[3] = {Led_1_Pin, Led_2_Pin, Led_3_Pin};

uint8_t LedRecvPcBox = 0;
Struct_TimeCycleWarm    sTimeCycleWarm=
{
    .Run  = TIME_RL_WARM,
    .Wait = TIME_RL_WARM_REFRESH,
};
/*================= Function Handle ==============*/
static uint8_t fevent_relay_entry(uint8_t event)
{
    fevent_active(sEventAppRelay, _EVENT_CONTROL_LED_STATUS);
    fevent_active(sEventAppRelay, _EVENT_CONTROL_LED_PCBOX);
    fevent_active(sEventAppRelay, _EVENT_CONTROL_LED_SLAVE);
    return 1;
}

static uint8_t fevent_relay_warm_refresh(uint8_t event)
{
#ifdef USING_REFRESH_WARM
    if(sElectric.PowerPresent != POWER_OFF) 
    OnRelay_Warm(sTimeCycleWarm.Run);
#endif
    return 1;
}

static uint8_t fevent_relay_warm_on(uint8_t event)
{
/*---------------ON relay warm--------------*/
    fevent_disable(sEventAppRelay,_EVENT_RELAY_WARM_REFRESH);
    fevent_enable(sEventAppRelay,_EVENT_RELAY_WARM_OFF);
    ControlRelay(RELAY_WARM, ON_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_UNCTRL);
    return 1;
}

static uint8_t fevent_relay_warm_off(uint8_t event)
{
/*--------------OFF relay warm--------------*/
    ControlRelay(RELAY_WARM, OFF_RELAY, _RL_RESPOND, _RL_DEBUG, _RL_UNCTRL);
    sStatusApp.RL_Warm = _APP_FREE;
    fevent_enable(sEventAppRelay, _EVENT_RELAY_WARM_REFRESH);
    return 1;
}

static uint8_t fevent_control_led_status(uint8_t event)
{
/*-----------------Dieu khien led Status----------------*/
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
/*--------------------Dieu khien led PcBox----------------*/
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
/*------------------Dieu khien led Slave---------------------*/
    if(sStateSlave485.Electric == CONNECT_SLAVE)
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

/*
    @brief  Bat relay warm
    @param  time: Thoi gian bat
*/
void OnRelay_Warm(uint32_t time_min)
{
    sStatusApp.RL_Warm = _APP_BUSY;
    sEventAppRelay[_EVENT_RELAY_WARM_OFF].e_period = time_min * TIME_ONE_MINUTES;
    fevent_active(sEventAppRelay,_EVENT_RELAY_WARM_ON);
}

/*------------------------Function Respond Pc Box-------------------------*/
/*
    @brief  Phan hoi On/Off relay ve PcBox
    @param  State: Co gui du lieu ve PcBox hay khong
    @param  KindRelay: Loai relay
    @param  Status: Trang thai dang bat hay tat
*/
void Relay_Respond_Pc_Box(uint8_t State, uint8_t KindRelay, uint8_t Status)
{
  if(State == _RL_RESPOND)
  {
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_RESPOND_HANDLE_RELAY;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x02;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = KindRelay ;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = Status;
    
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
  }
}
/*---------------------Save and Init State Relay---------------------*/
/*
    @brief  Luu trang thai relay vao flash
*/
void Save_StatusRelay(void)
{
    uint8_t aWrite[16]={0};
    uint8_t length = 0;
    aWrite[length++] = BYTE_TEMP_FIRST_EXFLASH;
    aWrite[length++] = NUMBER_RELAY;
    aWrite[length++] = sStatusRelay.Screen;
    aWrite[length++] = sStatusRelay.FridgeCool;
    aWrite[length++] = sStatusRelay.Alarm;
    aWrite[length++] = sStatusRelay.FridgeHeat;
    aWrite[length++] = sStatusRelay.Lamp;
    aWrite[length++] = sStatusRelay.Warm;
    aWrite[length++] = sStatusRelay.Pc;
    
    aWrite[length++] = sStatusRelay.Screen_Ctrl;
    aWrite[length++] = sStatusRelay.FridgeCool_Ctrl;
    aWrite[length++] = sStatusRelay.Alarm_Ctrl;
    aWrite[length++] = sStatusRelay.FridgeHeat_Ctrl;
    aWrite[length++] = sStatusRelay.Lamp_Ctrl;
    aWrite[length++] = sStatusRelay.Warm_Ctrl;
    aWrite[length++] = sStatusRelay.Pc_Ctrl;
    
    eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_STATUS_RELAY);
    HAL_Delay(1);
    eFlash_S25FL_BufferWrite(aWrite, EX_FLASH_ADDR_STATUS_RELAY, length);
}

/*
    @brief  Doc trang thai relay tu flash
*/
void Init_StatusRelay(void)
{
    uint8_t aRead[16] = {0};
    eFlash_S25FL_BufferRead(aRead, EX_FLASH_ADDR_STATUS_RELAY, 16);
    if( aRead[0] == BYTE_TEMP_FIRST_EXFLASH && aRead[1] == NUMBER_RELAY)
    {
        if(aRead[3] <= 0x01) sStatusRelay.Screen    = aRead[2];
        //if(aRead[4] <= 0x01) sStatusRelay.FridgeCool= aRead[3];
        //if(aRead[5] <= 0x01) sStatusRelay.Alarm     = aRead[4];
        //if(aRead[6] <= 0x01) sStatusRelay.FridgeHeat= aRead[5];
        if(aRead[7] <= 0x01) sStatusRelay.Lamp      = aRead[6];
        //if(aRead[8] <= 0x01) sStatusRelay.Warm      = aRead[7];
        //if(aRead[8] <= 0x01) sStatusRelay.Pc      = aRead[8];
        
        //if(aRead[10] <= 0x01) sStatusRelay.Screen_Ctrl    = aRead[9];
        //if(aRead[11] <= 0x01) sStatusRelay.FridgeCool_Ctrl= aRead[10];
        //if(aRead[12] <= 0x01) sStatusRelay.Alarm_Ctrl     = aRead[11];
        //if(aRead[13] <= 0x01) sStatusRelay.FridgeHeat_Ctrl= aRead[12];
        if(aRead[14] <= 0x01) sStatusRelay.Lamp_Ctrl      = aRead[13];
        //if(aRead[15] <= 0x01) sStatusRelay.Warm_Ctrl      = aRead[14];
        //if(aRead[15] <= 0x01) sStatusRelay.Pc_Ctrl      = aRead[15];
    }
    Setup_StatusRelay();
}

void Setup_StatusRelay(void)
{
  if(sStatusRelay.Screen == 1)      HAL_GPIO_WritePin(RELAY_PORT[0], RELAY_PIN[0], GPIO_PIN_ON_RELAY);
  else                              HAL_GPIO_WritePin(RELAY_PORT[0], RELAY_PIN[0], GPIO_PIN_OFF_RELAY);
  
  if(sStatusRelay.FridgeCool == 1)  HAL_GPIO_WritePin(RELAY_PORT[1], RELAY_PIN[1], GPIO_PIN_ON_RELAY);
  else                              HAL_GPIO_WritePin(RELAY_PORT[1], RELAY_PIN[1], GPIO_PIN_OFF_RELAY);
  
  if(sStatusRelay.Alarm == 1)       HAL_GPIO_WritePin(RELAY_PORT[2], RELAY_PIN[2], GPIO_PIN_ON_RELAY);
  else                              HAL_GPIO_WritePin(RELAY_PORT[2], RELAY_PIN[2], GPIO_PIN_OFF_RELAY);
  
  if(sStatusRelay.FridgeHeat == 1)  HAL_GPIO_WritePin(RELAY_PORT[3], RELAY_PIN[3], GPIO_PIN_ON_RELAY);
  else                              HAL_GPIO_WritePin(RELAY_PORT[3], RELAY_PIN[3], GPIO_PIN_OFF_RELAY);
  
  if(sStatusRelay.Lamp == 1)        HAL_GPIO_WritePin(RELAY_PORT[4], RELAY_PIN[4], GPIO_PIN_ON_RELAY);
  else                              HAL_GPIO_WritePin(RELAY_PORT[4], RELAY_PIN[4], GPIO_PIN_OFF_RELAY);
  
  if(sStatusRelay.Warm == 1)        HAL_GPIO_WritePin(RELAY_PORT[5], RELAY_PIN[5], GPIO_PIN_ON_RELAY);
  else                              HAL_GPIO_WritePin(RELAY_PORT[5], RELAY_PIN[5], GPIO_PIN_OFF_RELAY);
  
  if(sStatusRelay.Warm == 1)        HAL_GPIO_WritePin(RELAY_PORT[6], RELAY_PIN[6], GPIO_PIN_ON_RELAY);
  else                              HAL_GPIO_WritePin(RELAY_PORT[6], RELAY_PIN[6], GPIO_PIN_OFF_RELAY);
}

/*---------------------Save and Init Time Warm Door---------------------*/
uint8_t Save_TimeRelayWarm(uint8_t Run, uint8_t Wait)
{
    if(Run > 0 && Run <= 30 && Wait >= 5)
    {
        sTimeCycleWarm.Run = Run;
        sTimeCycleWarm.Wait = Wait;
        
        uint8_t write[3] = {0};
        uint8_t length = 0;
        write[length++]= BYTE_TEMP_FIRST_EXFLASH;
        write[length++]= sTimeCycleWarm.Run;
        write[length++]= sTimeCycleWarm.Wait;
        
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_TIME_RELAY_WARM);
        eFlash_S25FL_BufferWrite(write, EX_FLASH_ADDR_TIME_RELAY_WARM, length);
        
        sEventAppRelay[_EVENT_RELAY_WARM_REFRESH].e_period = sTimeCycleWarm.Wait * TIME_ONE_MINUTES;
        return 1;
    }
    return 0;
}

void Init_TimeRelayWarm(void)
{
    uint8_t aRead[4] = {0};
    eFlash_S25FL_BufferRead(aRead, EX_FLASH_ADDR_TIME_RELAY_WARM, 3);
    if(aRead[0] == BYTE_TEMP_FIRST_EXFLASH )
    {
        sTimeCycleWarm.Run  = aRead[1];
        sTimeCycleWarm.Wait = aRead[2];
    }
    sEventAppRelay[_EVENT_RELAY_WARM_REFRESH].e_period = sTimeCycleWarm.Wait * TIME_ONE_MINUTES;
}
/*---------------------Function Ctrl Relay---------------------*/
/*
    @brief  Dieu khien Relay 
    @param  Relay: Loai relay
    @param  State: Trang thai dong hoac mo
    @param  StateRespond: Co phan hoi ve PcBox hay k
    @param  RelayDebug: Co hien thi Debug hay k 
    @param  RelayCtrl: Co phai nguoi dung dieu khien relay k  
*/
void ControlRelay(uint8_t Relay, uint8_t State, uint8_t StateRespond, uint8_t RelayDebug, uint8_t RelayCtrl)
{
  if(State == OFF_RELAY || State == ON_RELAY)
  {
    switch(Relay)
    {
            
          case RELAY_SCREEN: 
            OnOff_Relay(RELAY_SCREEN, State);
            sStatusRelay.Screen = State;
            if(RelayCtrl == _RL_CTRL) sStatusRelay.Screen_Ctrl = State;
            Save_StatusRelay();
            break;
            
          case RELAY_FRIDGE_COOL: 
            OnOff_Relay(RELAY_FRIDGE_COOL, State);
            sStatusRelay.FridgeCool = State;
            if(RelayCtrl == _RL_CTRL) sStatusRelay.FridgeCool_Ctrl = State;
            break;
            
          case RELAY_ALARM: 
            OnOff_Relay(RELAY_ALARM, State);
            sStatusRelay.Alarm = State;
            if(RelayCtrl == _RL_CTRL) sStatusRelay.Alarm_Ctrl = State;
            break;
            
          case RELAY_FRIDGE_HEAT: 
            OnOff_Relay(RELAY_FRIDGE_HEAT, State);
            sStatusRelay.FridgeHeat = State;
            if(RelayCtrl == _RL_CTRL) sStatusRelay.FridgeHeat_Ctrl = State;
            break;
            
          case RELAY_LAMP: 
            OnOff_Relay(RELAY_LAMP, State);
            sStatusRelay.Lamp = State;
            if(RelayCtrl == _RL_CTRL) sStatusRelay.Lamp_Ctrl = State;
            Save_StatusRelay();
            break;
            
          case RELAY_WARM: 
            OnOff_Relay(RELAY_WARM, State);
            sStatusRelay.Warm = State;
            if(RelayCtrl == _RL_CTRL) sStatusRelay.Warm_Ctrl = State;
            break;
            
          case RELAY_PC: 
            OnOff_Relay(RELAY_PC, State);
            sStatusRelay.Pc = State;
            if(RelayCtrl == _RL_CTRL) sStatusRelay.Pc_Ctrl = State;
            break;
        
         default:
            break;
    }
    Relay_Respond_Pc_Box(StateRespond, Relay, State);
    Relay_Debug(RelayDebug, Relay, State);
  }
}

/*
    @brief  ON/OFF Relay
    @param  Relay: Loai relay
    @param  State: Trang thai ON hoac OFF
*/
void OnOff_Relay(Relay_TypeDef Relay, uint8_t State)
{
    if(State == ON_RELAY)
        HAL_GPIO_WritePin(RELAY_PORT[Relay], RELAY_PIN[Relay], GPIO_PIN_ON_RELAY);
    else
        HAL_GPIO_WritePin(RELAY_PORT[Relay], RELAY_PIN[Relay], GPIO_PIN_OFF_RELAY);
}

/*---------------------Function Ctrl Led---------------------*/
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
/*-------------------------Function Handle-------------------------*/
/*
    @brief  Hien thi debug bat tat relay
    @param  State_Debug: Co thuc hien hien thi debug khong
    @param  KindRelay: Loai relay 
    @param  Status: Trang thai relay hien tai
*/
void Relay_Debug(uint8_t State_Debug, uint8_t KindRelay, uint8_t Status)
{
    if(State_Debug == _RL_DEBUG)
    {
#ifdef USING_APP_RELAY_DEBUG
    if(Status == ON_RELAY)
    {
      UTIL_Printf(DBLEVEL_M, (uint8_t*)"user_app_relay: ON Relay: ", sizeof("user_app_relay: ON Relay: ")-1);
    }
    else
    {
      UTIL_Printf(DBLEVEL_M, (uint8_t*)"user_app_relay: OFF Relay: ", sizeof("user_app_relay: OFF Relay: ")-1);
    }
    
    switch(KindRelay)
    {
        case RELAY_SCREEN:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"SCREEN ", sizeof("SCREEN ")-1); 
           break;
           
        case RELAY_FRIDGE_COOL:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"FRIDGE COOL ", sizeof("FRIDGE COOL ")-1); 
           break;
           
        case RELAY_ALARM:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"ALARM ", sizeof("ALARM ")-1); 
           break;
           
        case RELAY_FRIDGE_HEAT:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"FRIDGE HEAT ", sizeof("FRIDGE HEAT ")-1); 
           break;
           
        case RELAY_LAMP:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"LAMP ", sizeof("LAMP ")-1); 
           break;
           
        case RELAY_WARM:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"WARM ", sizeof("WARM ")-1); 
           break;
           
        case RELAY_PC:
           UTIL_Printf(DBLEVEL_M, (uint8_t*)"PCBOX ", sizeof("PCBOX ")-1); 
           break;
           
        default:
           break;
    }
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1); 
#endif
    }
}

void Init_AppRelay(void)
{
    Init_StatusRelay();
    Init_TimeRelayWarm();
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

