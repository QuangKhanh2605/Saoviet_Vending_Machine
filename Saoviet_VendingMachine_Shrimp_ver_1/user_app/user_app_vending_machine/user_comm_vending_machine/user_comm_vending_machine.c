

#include "user_comm_vending_machine.h"
#include "user_inc_vending_machine.h"
#include "onchipflash.h"
#include "user_external_flash.h"
void Init_AppVendingMachine(void)
{
    Init_AppComm_Vending_Machine();
    #ifdef USING_APP_PC_BOX
        Init_AppPcBox();
    #endif
        
    #ifdef USING_APP_DELIVERY 
        Init_AppDelivery();
    #endif
        
    #ifdef USING_APP_DOOR_SENSOR
        Init_AppDoorSensor();
    #endif
        
    #ifdef USING_APP_ELECTRIC
        Init_AppElectric();
    #endif
        
    #ifdef USING_APP_RELAY
        Init_AppRelay();
    #endif
        
    #ifdef USING_APP_TEMPERATURE
        Init_AppTemperature();
    #endif
        
    #ifdef USING_APP_VIB_SENSOR
        Init_AppVibSensor();
    #endif
        
    #ifdef USING_APP_RS485
        Init_AppRs485();
    #endif
}

uint8_t AppVendingMachine_Task(void)
{
    #ifdef USING_APP_PC_BOX
        AppPcBox_Task();
    #endif
        
    #ifdef USING_APP_DELIVERY 
        AppDelivery_Task();
    #endif
        
    #ifdef USING_APP_DOOR_SENSOR
        AppDoorSensor_Task();
    #endif
        
    #ifdef USING_APP_ELECTRIC
        AppElectric_Task();
    #endif
        
    #ifdef USING_APP_RELAY
        AppRelay_Task();
    #endif
        
    #ifdef USING_APP_TEMPERATURE
        AppTemperature_Task();
    #endif
        
    #ifdef USING_APP_VIB_SENSOR
        AppVibSensor_Task();
    #endif
        
    #ifdef USING_APP_RS485
        AppRs485_Task();
    #endif
        
    return 1;
}

void Init_AppComm_Vending_Machine(void)
{
    if(*(__IO uint8_t*)(0x080E0000) != 0xAA)
    {
        uint8_t aData[8] ={0};
        uint16_t length = 0;
        for(uint8_t i = 0; i<8; i++)
            aData[length++] = 0xAA;
        OnchipFlashPageErase(0x080E0000);
        HAL_Delay(1);
        OnchipFlashWriteData(0x080E0000, aData, length);
        
        Reset_WDG_DCU();
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_TEMP_SET_THRESH);
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_MAIN_ID);
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_STATUS_RELAY);
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_STATUS_ELECTRIC);
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_IDSLAVE_ELECTRIC);
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_FREQ_TSVH);
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_TIME_ON_OFF_PCBOX);
        Reset_WDG_DCU();
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_TIME_RELAY_WARM);
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_PCBOX_USING_CRC);
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_VALUE_TEMP_CALIB);
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_ID_SLAVE_485);
        eFlash_S25FL_Erase_Sector(EX_FLASH_ADDR_STATE_PCBOX);
        Reset_WDG_DCU();
    }
}

/*=========== Function Crc =============*/

/* 
    @brief Calculator Crc 2 byte use Crc Modbus RTU
*/
uint8_t Calculator_Crc_U16(uint16_t *crc, uint8_t* buf, uint16_t len)
{
  *crc = 0x0000;
  
  for (int pos = 0; pos < len; pos++) 
  {
    *crc ^= (uint16_t)buf[pos];          // XOR byte into least sig. byte of crc
  
    for (int i = 8; i != 0; i--) {    // Loop over each bit
      if ((*crc & 0x0001) != 0) {      // If the LSB is set
        *crc >>= 1;                    // Shift right and XOR 0xA001
        *crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        *crc >>= 1;                    // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return 1;  
}

/*
    @brief Calculator Crc 1 byte use XOR
*/
uint8_t Calculator_Crc_U8(uint8_t *crc, uint8_t* buf, uint16_t len)
{
    for (uint8_t i = 0; i < len; i++)
        *crc ^= buf[i];
    return 1;
}
/*====================================================*/

void Respond_PcBox (uint8_t *pData, uint16_t Length)
{
    HAL_UART_Transmit(&uart_pcbox, pData, Length, 1000);
}

/*
    Chuyen Scale sang Uint
*/
uint32_t Scale_To_Int(uint8_t Scale)
{
    /*
    Scale
    00->1 
    FF->0.1 
    FE->0.01 
    FD->0.001  
    FC->0.0001  
    FB->0.00001 
    */
  
    if(Scale == 0x00) return 1;
    else if(Scale == 0xFF) return 10;
    else if(Scale == 0xFE) return 100;
    else if(Scale == 0xFD) return 1000;
    else if(Scale == 0xFC) return 10000;
    else if(Scale == 0xFB) return 100000;
    
    return 1;
}

/*
    @brief  Tinh tan so theo Scale
    @retval Tan so nap vao bien tan
*/
int16_t Calculator_Scale_Int(int16_t Value, uint8_t Scale)
{
      /*
    Scale
    00->1 
    FF->0.1 
    FE->0.01 
    FD->0.001  
    FC->0.0001  
    FB->0.00001 
    */
  int Stamp_Scale = Value * (int)Scale_To_Int(DEFAULT_TEMP_SCALE);
  return Stamp_Scale/(int)Scale_To_Int(Scale);
}



