
#include "user_app_relay.h"
#include "user_comm_vending_machine.h"

/*============== Function static ===============*/
static uint8_t fevent_on_off_relay_pc(uint8_t event);
static uint8_t fevent_on_off_relay_screen(uint8_t event);
static uint8_t fevent_on_off_relay_fridge(uint8_t event);
static uint8_t fevent_on_off_relay_alarm(uint8_t event);
static uint8_t fevent_on_off_relay_5(uint8_t event);
static uint8_t fevent_on_off_relay_lamp(uint8_t event);
static uint8_t fevent_on_off_relay_warm(uint8_t event);
static uint8_t fevent_relay_entry(uint8_t event);
/*=================== struct ==================*/
sEvent_struct               sEventAppRelay[] = 
{
  {_EVENT_RELAY_ENTRY,              1, 0, 5,    fevent_relay_entry},
  {_EVENT_ON_OFF_RELAY_PC,          0, 0, 5,    fevent_on_off_relay_pc},
  {_EVENT_ON_OFF_RELAY_SCREEN,      0, 0, 5,    fevent_on_off_relay_screen},
  {_EVENT_ON_OFF_RELAY_FRIDGE,      0, 0, 5,    fevent_on_off_relay_fridge},
  {_EVENT_ON_OFF_RELAY_ALARM,       0, 0, 5,    fevent_on_off_relay_alarm},
  {_EVENT_ON_OFF_RELAY_5,           0, 0, 5,    fevent_on_off_relay_5},
  {_EVENT_ON_OFF_RELAY_LAMP,        0, 0, 5,    fevent_on_off_relay_lamp},
  {_EVENT_ON_OFF_RELAY_WARM,        0, 0, 5,    fevent_on_off_relay_warm},
};


Struct_StatusRelay          sStatusRelay={OFF_RELAY};                

static GPIO_TypeDef*        RELAY_PORT[7] = {ON_OFF_PC_GPIO_Port, ON_OFF_Screen_GPIO_Port, 
                                             ON_OFF_Fridge_GPIO_Port, ON_OFF_Alarm_GPIO_Port,
                                             ON_OFF_Relay_5_GPIO_Port, Motor_GPIO_Port,
                                             Layer_7_GPIO_Port};

static uint16_t             RELAY_PIN[7] = {ON_OFF_PC_Pin, ON_OFF_Screen_Pin, 
                                            ON_OFF_Fridge_Pin, ON_OFF_Alarm_Pin,
                                            ON_OFF_Relay_5_Pin, Motor_Pin,
                                            Layer_7_Pin};
/*================= Function Handle ==============*/
static uint8_t fevent_relay_entry(uint8_t event)
{
    sStatusRelay.PC = ON_RELAY;
    fevent_active(sEventAppRelay, _EVENT_ON_OFF_RELAY_PC);
    return 1;
}

static uint8_t fevent_on_off_relay_pc(uint8_t event)
{
    if(sStatusRelay.PC == ON_RELAY)
    {
        On_Relay(RELAY_PC);
    }
    else if(sStatusRelay.PC == OFF_RELAY)
    {
        Off_Relay(RELAY_PC);
    }

    return 1;
}

static uint8_t fevent_on_off_relay_screen(uint8_t event)
{
    if(sStatusRelay.Screen == ON_RELAY)
    {
        On_Relay(RELAY_FRIDGE);
    }
    else if(sStatusRelay.Screen == OFF_RELAY)
    {
        Off_Relay(RELAY_FRIDGE);
    }

    return 1;
}

static uint8_t fevent_on_off_relay_fridge(uint8_t event)
{
    if(sStatusRelay.Fridge == ON_RELAY)
    {
        On_Relay(RELAY_FRIDGE);
    }
    else if(sStatusRelay.Fridge == OFF_RELAY)
    {
        Off_Relay(RELAY_FRIDGE);
    }

    return 1;
}

static uint8_t fevent_on_off_relay_alarm(uint8_t event)
{
    if(sStatusRelay.Alarm == ON_RELAY)
    {
        On_Relay(RELAY_ALARM);
    }
    else if(sStatusRelay.Alarm == OFF_RELAY)
    {
        Off_Relay(RELAY_ALARM);
    }
        
    return 1;
}

static uint8_t fevent_on_off_relay_5(uint8_t event)
{
    if(sStatusRelay.Relay_5 == ON_RELAY)
    {
        On_Relay(RELAY_5);
    }
    else if(sStatusRelay.Relay_5 == OFF_RELAY)
    {
        Off_Relay(RELAY_5);
    }

    return 1;
}

static uint8_t fevent_on_off_relay_lamp(uint8_t event)
{
    if(sStatusRelay.Lamp == ON_RELAY)
    {
        On_Relay(RELAY_LAMP);
    }
    else if(sStatusRelay.Lamp == OFF_RELAY)
    {
        Off_Relay(RELAY_LAMP);
    }
    
    return 1;
}

static uint8_t fevent_on_off_relay_warm(uint8_t event)
{
    if(sStatusRelay.Warm == ON_RELAY)
    {
        On_Relay(RELAY_WARM);
    }
    else if(sStatusRelay.Warm == OFF_RELAY)
    {
        Off_Relay(RELAY_WARM);
    }
    
    return 1;
}
/*========== Function Handle ============*/

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

