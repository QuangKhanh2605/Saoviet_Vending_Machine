#include "user_app_door_sensor.h"
#include "user_inc_vending_machine.h"
/*================= Function static ==============*/
static uint8_t fevent_door_entry(uint8_t event);
static uint8_t fevent_door_sensor(uint8_t event);
static uint8_t fevent_door_ctrl_respond(uint8_t event);
static uint8_t fevent_door_respond_pc_box(uint8_t event);

/*================= Struct ======================*/
sEvent_struct           sEventAppDoorSensor[]=
{
  {_EVENT_DOOR_ENTRY,               1, 5, 0,                  fevent_door_entry},
  {_EVENT_DOOR_SENSOR,              0, 0, 2000,               fevent_door_sensor},
  {_EVENT_DOOR_CTRL_RESPOND,        0, 0, 0,                  fevent_door_ctrl_respond},
  {_EVENT_DOOR_RESPOND_PC_BOX,      0, 0, 60000,              fevent_door_respond_pc_box},
};

StructStatusDoor        sStatusDoor = {0};
/*================== Function Handle =================*/
static uint8_t fevent_door_entry(uint8_t event)
{
    fevent_active(sEventAppDoorSensor, _EVENT_DOOR_SENSOR);
    return 1;
}

static uint8_t fevent_door_sensor(uint8_t event)
{ 
/*---------------------Kiem tra trang thai Door-------------------*/
    if(HAL_GPIO_ReadPin(Door_Sensor_1_GPIO_Port, Door_Sensor_1_Pin) == INIT_STATUS_DOOR_SENSOR_INPUT_1)
        sStatusDoor.Sensor1 = DOOR_OPEN;
    else
        sStatusDoor.Sensor1 = DOOR_CLOSE;
    
    if(HAL_GPIO_ReadPin(Door_Sensor_2_GPIO_Port, Door_Sensor_2_Pin) == INIT_STATUS_DOOR_SENSOR_INPUT_2)
        sStatusDoor.Sensor2 = DOOR_OPEN;
    else
        sStatusDoor.Sensor2 = DOOR_CLOSE;
/*---------------------------------------------------------------------*/
    fevent_active(sEventAppDoorSensor, _EVENT_DOOR_CTRL_RESPOND);
    fevent_enable(sEventAppDoorSensor, event);
    
    return 1;
}

static uint8_t fevent_door_ctrl_respond(uint8_t event)
{
/*----------------------Kiem tra trang thai 2 Door de canh bao---------------------*/
    static uint8_t status_before = DOOR1_CLOSE_DOOR2_CLOSE;
    static uint8_t status = DOOR1_CLOSE_DOOR2_CLOSE;
    static uint8_t On_RL_Warm = 0;
    if(sStatusDoor.Sensor1 == DOOR_OPEN && sStatusDoor.Sensor2 == DOOR_OPEN)
    {
        status = DOOR1_OPEN_DOOR2_OPEN;
        sStatusApp.Door = _APP_BUSY;
        On_RL_Warm = 1;
    }
    else if(sStatusDoor.Sensor1 == DOOR_CLOSE && sStatusDoor.Sensor2 == DOOR_OPEN)
    {
        status = DOOR1_CLOSE_DOOR2_OPEN;
        sStatusApp.Door = _APP_BUSY;
    }
    else if(sStatusDoor.Sensor1 == DOOR_OPEN && sStatusDoor.Sensor2 == DOOR_CLOSE)
    {
        status = DOOR1_OPEN_DOOR2_CLOSE;
        sStatusApp.Door = _APP_BUSY;
    }
    else
    {
        status = DOOR1_CLOSE_DOOR2_CLOSE;
        sStatusApp.Door = _APP_FREE;
    }
    
    if(status != status_before)
    {
        sStatusDoor.Handle_Respond = 1;
        fevent_active(sEventAppDoorSensor, _EVENT_DOOR_RESPOND_PC_BOX);
    }
    
    status_before = status;
    
    if(On_RL_Warm == 1)
    {
        if(status == DOOR1_CLOSE_DOOR2_CLOSE)
        {
            On_RL_Warm = 0;
            if(sElectric.PowerPresent != POWER_OFF)
            OnRelay_Warm(sTimeCycleWarm.Run);
        }
    }

    return 1;
}

static uint8_t fevent_door_respond_pc_box(uint8_t event)
{
/*--------------Phan hoi canh bao len PcBox-------------*/
    if(sStatusDoor.Sensor1 == DOOR_OPEN || sStatusDoor.Sensor2 == DOOR_OPEN || sStatusDoor.Handle_Respond == 1)
    {
        sStatusDoor.Handle_Respond = 0;
        Log_Data_Door_Respond_PcBox();
        AppDoorSensor_Debug();
    }
    
    fevent_enable(sEventAppDoorSensor, event);
    return 1;
}

/*============= Function Handle ===========*/
/*
    @brief  Log trang thai canh bao mo cua
*/
void Log_Data_Door_Respond_PcBox(void)
{
/*=============== Log ===============*/
    
    sRespPcBox.Length_u16 = 0;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = OBIS_WARNING_DOOR_SENSOR;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x02;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sStatusDoor.Sensor1;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sStatusDoor.Sensor2;
    Packing_Respond_PcBox(sRespPcBox.Data_a8, sRespPcBox.Length_u16);
}

/*============== Function Handle ==============*/

/*
    @brief  Debug trang thai canh bao cua
*/
void AppDoorSensor_Debug(void)
{
#ifdef USING_APP_DOOR_SENSOR_DEBUG
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_door_sensor: Door1: ", sizeof("app_door_sensor: Door1: ")-1);
    
    if(sStatusDoor.Sensor1 == 1)
    {
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"OPEN", sizeof("OPEN")-1);
    }
    else
    {
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"CLOSE", sizeof("CLOSE")-1);
    }
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" Door2: ", sizeof(" Door2: ")-1);
    
    if(sStatusDoor.Sensor2 == 1)
    {
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"OPEN", sizeof("OPEN")-1);
    }
    else
    {
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"CLOSE", sizeof("CLOSE")-1);
    }
    
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n")-1);
#endif
}

uint8_t AppDoorSensor_Task(void)
{
    uint8_t i = 0;
    uint8_t Result = false;
    
    for(i = 0; i < _EVENT_DOOR_SENSOR_END; i++)
    {
        if(sEventAppDoorSensor[i].e_status == 1)
        {
            Result = true;
            
            if((sEventAppDoorSensor[i].e_systick == 0) || 
               ((HAL_GetTick() - sEventAppDoorSensor[i].e_systick) >= sEventAppDoorSensor[i].e_period))
            {
                sEventAppDoorSensor[i].e_status = 0; //Disable event
                sEventAppDoorSensor[i].e_systick= HAL_GetTick();
                sEventAppDoorSensor[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}

