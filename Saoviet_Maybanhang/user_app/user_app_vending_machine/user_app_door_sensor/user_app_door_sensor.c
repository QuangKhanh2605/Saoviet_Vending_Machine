
#include "user_app_door_sensor.h"
#include "user_comm_vending_machine.h"
#include "user_app_pc_box.h"
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
    if(HAL_GPIO_ReadPin(Door_Sensor_1_GPIO_Port, Door_Sensor_1_Pin) == INIT_STATUS_DOOR_SENSOR_INPUT)
    {
        sStatusDoor.Sensor1 = DOOR_OPEN;
    }
    else
    {
        sStatusDoor.Sensor1 = DOOR_CLOSE;
    }
    
    if(HAL_GPIO_ReadPin(Door_Sensor_2_GPIO_Port, Door_Sensor_2_Pin) == INIT_STATUS_DOOR_SENSOR_INPUT)
    {
        sStatusDoor.Sensor2 = DOOR_OPEN;
    }
    else
    {
        sStatusDoor.Sensor2 = DOOR_CLOSE;
    }

    fevent_active(sEventAppDoorSensor, _EVENT_DOOR_CTRL_RESPOND);
    fevent_enable(sEventAppDoorSensor, event);
    
    return 1;
}

static uint8_t fevent_door_ctrl_respond(uint8_t event)
{
    static uint8_t status_before = DOOR1_CLOSE_DOOR2_CLOSE;
    static uint8_t status = DOOR1_CLOSE_DOOR2_CLOSE;
    if(sStatusDoor.Sensor1 == DOOR_OPEN && sStatusDoor.Sensor2 == DOOR_OPEN)
    {
        status = DOOR1_OPEN_DOOR2_OPEN;
        sStatusApp.Door = BUSY;
    }
    else if(sStatusDoor.Sensor1 == DOOR_CLOSE && sStatusDoor.Sensor2 == DOOR_OPEN)
    {
        status = DOOR1_CLOSE_DOOR2_OPEN;
        sStatusApp.Door = BUSY;
    }
    else if(sStatusDoor.Sensor1 == DOOR_OPEN && sStatusDoor.Sensor2 == DOOR_CLOSE)
    {
        status = DOOR1_OPEN_DOOR2_CLOSE;
        sStatusApp.Door = BUSY;
    }
    else
    {
        status = DOOR1_CLOSE_DOOR2_CLOSE;
        sStatusApp.Door = FREE;
    }
    
    if(status != status_before)
    {
        sStatusDoor.Handle_Respond = 1;
        fevent_active(sEventAppDoorSensor, _EVENT_DOOR_RESPOND_PC_BOX);
    }
    
    status_before = status;
       
    return 1;
}

static uint8_t fevent_door_respond_pc_box(uint8_t event)
{
    if(sStatusDoor.Sensor1 == DOOR_OPEN || sStatusDoor.Sensor2 == DOOR_OPEN || sStatusDoor.Handle_Respond == 1)
    {
        sStatusDoor.Handle_Respond = 0;
        uint8_t aData[5];
        uint8_t length = 0;
        length = Log_Data_Door(aData);
        
        Write_Queue_Repond_PcBox(aData, length);
        
        AppDoorSensor_Debug();
    }
    
    fevent_enable(sEventAppDoorSensor, event);
    return 1;
}

/*============= Function Handle ===========*/
uint8_t Log_Data_Door(uint8_t *aData)
{
    uint8_t length = 0;
    uint16_t TempCrc = 0;
    
/*=============== Log ===============*/
    
    aData[length++] = OBIS_WARNING_DOOR_SENSOR;
    aData[length++] = 0x02;
    aData[length++] = sStatusDoor.Sensor1;
    aData[length++] = sStatusDoor.Sensor2;
    
    Calculator_Crc_U16(&TempCrc, aData, length);
    
    aData[length++] = TempCrc;
    aData[length++] = TempCrc >> 8;
    
      
    return length;
}

/*============== Function Handle ==============*/
void AppDoorSensor_Debug(void)
{
#ifdef USING_APP_DOOR_SENSOR_DEBUG
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"app_door_sensor: Door1: ", sizeof("app_door_sensor: Door1: "));
    if(sStatusDoor.Sensor1 == 1)
    {
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"OPEN", sizeof("OPEN"));
    }
    else
    {
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"CLOSE", sizeof("CLOSE"));
    }
    UTIL_Printf(DBLEVEL_M, (uint8_t*)" Door2: ", sizeof(" Door2: "));
    
    if(sStatusDoor.Sensor2 == 1)
    {
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"OPEN", sizeof("OPEN"));
    }
    else
    {
        UTIL_Printf(DBLEVEL_M, (uint8_t*)"CLOSE", sizeof("CLOSE"));
    }
    UTIL_Printf(DBLEVEL_M, (uint8_t*)"\r\n", sizeof("\r\n"));
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

