#include "user_app_relay.h"

/*============== Function static ===============*/
static uint8_t fevent_relay_pc(uint8_t event);
static uint8_t fevent_relay_screen(uint8_t event);
static uint8_t fevent_relay_fridge(uint8_t event);
static uint8_t fevent_relay_alarm(uint8_t event);
static uint8_t fevent_relay_5(uint8_t event);
/*=================== struct ==================*/
sEvent_struct               sEventAppRelay[] = 
{
  {_EVENT_RELAY_PC,          0, 0, 5,    fevent_relay_pc},
  {_EVENT_RELAY_SCREEN,      0, 0, 5,    fevent_relay_screen},
  {_EVENT_RELAY_FRIDGE,      0, 0, 5,    fevent_relay_fridge},
  {_EVENT_RELAY_ALARM,       0, 0, 5,    fevent_relay_alarm},
  {_EVENT_RELAY_5,           0, 0, 5,    fevent_relay_5},
};


Struct_Relay                sRelay_PC     ={0};
Struct_Relay                sRelay_Screen ={0};
Struct_Relay                sRelay_Fridge ={0};
Struct_Relay                sRelay_Alarm  ={0};
Struct_Relay                sRelay_Relay5 ={0};

static GPIO_TypeDef*        RELAY_PORT[5] = {ON_OFF_PC_GPIO_Port, ON_OFF_Screen_GPIO_Port, 
                                             ON_OFF_Fridge_GPIO_Port, ON_OFF_Alarm_GPIO_Port,
                                             ON_OFF_Relay_5_GPIO_Port};

static uint16_t             RELAY_PIN[5] = {ON_OFF_PC_Pin, ON_OFF_Screen_Pin, 
                                            ON_OFF_Fridge_Pin, ON_OFF_Alarm_Pin,
                                            ON_OFF_Relay_5_Pin};
/*================= Function Handle ==============*/
static uint8_t fevent_relay_pc(uint8_t event)
{
    if(sRelay_PC.Status_Recv == 1)
    {
        sRelay_PC.Status_Recv = 0;
        if(sRelay_PC.Status_Relay == 1)
        {
            On_Relay(RELAY_PC);
        }
        else if(sRelay_PC.Status_Relay == 0)
        {
            Off_Relay(RELAY_PC);
        }
    }
    fevent_enable(sEventAppRelay, event);
    return 1;
}

static uint8_t fevent_relay_screen(uint8_t event)
{
    if(sRelay_Fridge.Status_Recv == 1)
    {
        sRelay_Fridge.Status_Recv = 0;
        if(sRelay_Fridge.Status_Relay == 1)
        {
            On_Relay(RELAY_FRIDGE);
        }
        else if(sRelay_Fridge.Status_Relay == 0)
        {
            Off_Relay(RELAY_FRIDGE);
        }
    }
    fevent_enable(sEventAppRelay, event);
    return 1;
}

static uint8_t fevent_relay_fridge(uint8_t event)
{
    if(sRelay_Fridge.Status_Recv == 1)
    {
        sRelay_Fridge.Status_Recv = 0;
        if(sRelay_Fridge.Status_Relay == 1)
        {
            On_Relay(RELAY_FRIDGE);
        }
        else if(sRelay_Fridge.Status_Relay == 0)
        {
            Off_Relay(RELAY_FRIDGE);
        }
    }
    fevent_enable(sEventAppRelay, event);
    return 1;
}

static uint8_t fevent_relay_alarm(uint8_t event)
{
    if(sRelay_Alarm.Status_Recv == 1)
    {
        sRelay_Alarm.Status_Recv = 0;
        if(sRelay_Alarm.Status_Relay == 1)
        {
            On_Relay(RELAY_ALARM);
        }
        else if(sRelay_Alarm.Status_Relay == 0)
        {
            Off_Relay(RELAY_ALARM);
        }
    }
    fevent_enable(sEventAppRelay, event);
    return 1;
}

static uint8_t fevent_relay_5(uint8_t event)
{
    if(sRelay_Relay5.Status_Recv == 1)
    {
        sRelay_Relay5.Status_Recv = 0;
        if(sRelay_Relay5.Status_Relay == 1)
        {
            On_Relay(RELAY_5);
        }
        else if(sRelay_Relay5.Status_Relay == 0)
        {
            Off_Relay(RELAY_5);
        }
    }
    fevent_enable(sEventAppRelay, event);
    return 1;
}

void On_Relay(Relay_TypeDef Relay)
{
    HAL_GPIO_WritePin(RELAY_PORT[Relay], RELAY_PIN[Relay], GPIO_PIN_SET);
}

void Off_Relay(Relay_TypeDef Relay)
{
    HAL_GPIO_WritePin(RELAY_PORT[Relay], RELAY_PIN[Relay], GPIO_PIN_RESET);
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

