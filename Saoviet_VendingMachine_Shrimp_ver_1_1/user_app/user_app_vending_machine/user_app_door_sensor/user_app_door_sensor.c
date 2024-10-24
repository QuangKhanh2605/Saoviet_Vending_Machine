#include "user_app_door_sensor.h"
#include "user_inc_vending_machine.h"
/*================= Function static ==============*/
static uint8_t fevent_door_entry(uint8_t event);
static uint8_t fevent_door_sensor(uint8_t event);
static uint8_t fevent_door_ctrl_respond(uint8_t event);
static uint8_t fevent_door_respond_pc_box(uint8_t event);

static uint8_t fevent_door_delivery(uint8_t event);
static uint8_t fevent_lock_magnetis(uint8_t event);
/*================= Struct ======================*/
sEvent_struct           sEventAppDoorSensor[]=
{
  {_EVENT_DOOR_ENTRY,               1, 5, 5000,               fevent_door_entry},
  {_EVENT_DOOR_SENSOR,              0, 0, 500,                fevent_door_sensor},
  {_EVENT_DOOR_CTRL_RESPOND,        0, 0, 0,                  fevent_door_ctrl_respond},
  {_EVENT_DOOR_RESPOND_PC_BOX,      0, 0, 60000,              fevent_door_respond_pc_box},
  
  {_EVENT_DOOR_DELIVERY,            1, 0, 200,                fevent_door_delivery},
  {_EVENT_LOCK_MAGNETIS,            1, 0, 100,                fevent_lock_magnetis},
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
    if(HAL_GPIO_ReadPin(DOOR_SENSOR_PORT, DOOR_SENSOR_PIN) == INIT_STATUS_DOOR_SENSOR_INPUT)
    {
        sStatusApp.Door = _APP_BUSY;
        sStatusDoor.SensorMachine = DOOR_OPEN;
    }
    else
    {
        sStatusApp.Door = _APP_FREE;
        sStatusDoor.SensorMachine = DOOR_CLOSE;
    }
/*---------------------------------------------------------------------*/
    fevent_active(sEventAppDoorSensor, _EVENT_DOOR_CTRL_RESPOND);
    fevent_enable(sEventAppDoorSensor, event);
    
    return 1;
}

static uint8_t fevent_door_ctrl_respond(uint8_t event)
{
/*----------------------Kiem tra trang thai Door de canh bao---------------------*/
    static uint8_t status_before = DOOR_CLOSE;
//    static uint8_t On_RL_Warm = 0;
    
    if(sStatusDoor.SensorMachine != status_before)
    {
        sStatusDoor.Handle_Respond = 1;
        fevent_active(sEventAppDoorSensor, _EVENT_DOOR_RESPOND_PC_BOX);
//        if(sStatusDoor.SensorMachine == DOOR_OPEN)
//        {
//            On_RL_Warm = 1;
//        }
        status_before = sStatusDoor.SensorMachine;
    }
    
//    if(On_RL_Warm == 1)
//    {
//        if(sStatusDoor.SensorMachine == DOOR_CLOSE)
//        {
//            On_RL_Warm = 0;
////            if(sElectric.PowerPresent != POWER_OFF)
////            OnRelay_Warm(sTimeCycleWarm.Run);
//        }
//    }

    return 1;
}

static uint8_t fevent_door_respond_pc_box(uint8_t event)
{
/*--------------Phan hoi canh bao len PcBox-------------*/
    if(sStatusDoor.Handle_Respond == 1)
    {
        Log_Data_Door_Respond_PcBox();
        AppDoorSensor_Debug();
    }
    
    if(sStatusDoor.SensorMachine == DOOR_CLOSE)
    {
        sStatusDoor.Handle_Respond = 0;
        return 1;
    }
    
    fevent_enable(sEventAppDoorSensor, event);
    return 1;
}

static uint8_t fevent_door_delivery(uint8_t event)
{
//    static uint8_t CountDoorClose = 0;
//    
//    if(HAL_GPIO_ReadPin(DOOR_DELIVERY_PORT, DOOR_DELIVERY_PIN) == INIT_STATIS_DOOR_DELIVERY_INPUT)
//    {
//        sStatusDoor.SensorDelivery = DOOR_OPEN;
//        CountDoorClose = 0;
//    }
//    else
//    {
//        if(CountDoorClose < 5)
//            CountDoorClose++;
//        else
//            sStatusDoor.SensorDelivery = DOOR_CLOSE;
//    }
//    
    fevent_enable(sEventAppDoorSensor, event);
    return 1;
}

static uint8_t fevent_lock_magnetis(uint8_t event)
{
    static uint8_t CountStateLock = 0;
    
    if(HAL_GPIO_ReadPin(FB_LOCK_PORT, FB_LOCK_PIN) == GPIO_PIN_SET)
    {
        sParamDelivery.LockMagnetis = LOCK_MAGNETIS_OPEN;
        CountStateLock = 0;
    }
    else
    {
        if(CountStateLock < 5)
            CountStateLock++;
        else
            sParamDelivery.LockMagnetis = LOCK_MAGNETIS_CLOSE;
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
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = 0x01;
    sRespPcBox.Data_a8[sRespPcBox.Length_u16++] = sStatusDoor.SensorMachine;
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
    
    if(sStatusDoor.SensorMachine == DOOR_OPEN)
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
/*==========================Handle Task=======================*/
void Init_AppDoorSensor(void)
{

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

