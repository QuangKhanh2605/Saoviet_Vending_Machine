
#include "user_app_door_sensor.h"
#include "user_comm_vending_machine.h"

/*================= Function static ==============*/
static uint8_t fevent_door_sensor_1(uint8_t event);
static uint8_t fevent_door_sensor_2(uint8_t event);
static uint8_t fevent_door_respond_pc_box(uint8_t event);
static uint8_t fevent_door_entry(uint8_t event);
/*================= Struct ======================*/
sEvent_struct           sEventAppDoorSensor[]=
{
  {_EVENT_DOOR_ENTRY,               1, 5, TIME_ENTRY,                  fevent_door_entry},
  {_EVENT_DOOR_SENSOR_1,            0, 0, TIME_GET_DOOR_SENSOR,        fevent_door_sensor_1},
  {_EVENT_DOOR_SENSOR_2,            0, 0, 0,                           fevent_door_sensor_2},
  {_EVENT_DOOR_RESPOND_PC_BOX,      0, 0, 0,                           fevent_door_respond_pc_box},
};

StructStatusDoor        sStatusDoor = {0};
/*================== Function Handle =================*/
static uint8_t fevent_door_entry(uint8_t event)
{
    fevent_active(sEventAppDoorSensor, _EVENT_DOOR_SENSOR_1);
    return 1;
}

static uint8_t fevent_door_sensor_1(uint8_t event)
{ 
    if(HAL_GPIO_ReadPin(Door_Sensor_1_GPIO_Port, Door_Sensor_1_Pin) == INIT_STATUS_DOOR_SENSOR_INPUT)
    {
        sStatusDoor.Sensor1 = 1;
        fevent_active(sEventAppDoorSensor, _EVENT_DOOR_RESPOND_PC_BOX);
    }
    else
    {
        sStatusDoor.Sensor1 = 0;
    }

    fevent_active(sEventAppDoorSensor, _EVENT_DOOR_SENSOR_2);
    return 1;
}

static uint8_t fevent_door_sensor_2(uint8_t event)
{
      //    static uint8_t count_handle  = 0;
//    static uint8_t status_current= 0;
//    static uint8_t status_before = INIT_STATUS_IR_SENSOR_INPUT;
//    static uint8_t temp_status   = INIT_STATUS_IR_SENSOR_INPUT;
//    
//    status_current = HAL_GPIO_ReadPin(IR_Sensor_IN_GPIO_Port, IR_Sensor_IN_Pin);
//    if(status_current == status_before)
//    {
//        count_handle++;
//        if(count_handle >= NUMBER_SPLG_IR_SENSOR_INPUT)
//        {
//            count_handle = NUMBER_SPLG_IR_SENSOR_INPUT;
//            if(status_current != temp_status)
//            {
//                if(status_current == 0x01 ^ INIT_STATUS_IR_SENSOR_INPUT)
//                {
//                    Response_Ir_Sensor = 1;
//                }
//                else
//                {
//                    Response_Ir_Sensor = 0;
//                }
//            }
//            temp_status = status_current;
//        }
//    }
//    else
//    {
//        count_handle = 0;
//    }
//    status_before = HAL_GPIO_ReadPin(IR_Sensor_IN_GPIO_Port, IR_Sensor_IN_Pin);
//   
    if(HAL_GPIO_ReadPin(Door_Sensor_2_GPIO_Port, Door_Sensor_2_Pin) == INIT_STATUS_DOOR_SENSOR_INPUT)
    {
        sStatusDoor.Sensor2 = 1;
        fevent_active(sEventAppDoorSensor, _EVENT_DOOR_RESPOND_PC_BOX);
    }
    else
    {
        sStatusDoor.Sensor2 = 0;
    }
    
    fevent_active(sEventAppDoorSensor, _EVENT_DOOR_RESPOND_PC_BOX);
    return 1;
}

static uint8_t fevent_door_respond_pc_box(uint8_t event)
{
    if(sStatusDoor.Sensor1 == 1 || sStatusDoor.Sensor2 == 1)
    {
        uint8_t aData[5];
        uint8_t length = 0;
        length = Log_Data_Door(aData);
        Respond_PcBox(aData, length);
    }
    
    fevent_active(sEventAppDoorSensor, _EVENT_DOOR_SENSOR_1);
    sEventAppDoorSensor[_EVENT_DOOR_SENSOR_1].e_systick = HAL_GetTick();
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
    
    aData[length++] = TempCrc << 8;
    aData[length++] = TempCrc;
      
    return length;
}

/*============== Function Handle ==============*/
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


