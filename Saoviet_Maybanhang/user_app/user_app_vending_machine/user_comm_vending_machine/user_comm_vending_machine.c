
#include "user_comm_vending_machine.h"

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
int16_t Calculator_Scale(int16_t Value, uint8_t Scale)
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

/*=============== Convert Int To String ==============*/

uint8_t Convert_Int_To_String(char cData[], int var)
{
    uint8_t length = 0;
    uint32_t stamp = 0;
    uint64_t division = 10;
    if(var < 0)
    {
        stamp = 0 - var;
        cData[length++] = '-';
    }
    else
    {
        stamp = var;
    }
    
    while( stamp/division > 0 )
    {
        division *= 10;
    }
    division = division/10;
    while(division > 0)
    {
        cData[length++] = stamp/division + 0x30;
        stamp = stamp - (stamp/division) * division;
        division = division/10;
    }
    
    return length;
}

uint8_t Convert_Int_To_String_Scale(char cData[], int var, uint8_t Scale)
{
    uint8_t length = 0;
    uint32_t division = 0;
    uint32_t stamp = 0;
    uint8_t size_cData = 0;
    if(var < 0)
    {
        stamp = 0 - var;
    }
    else
    {
        stamp = var;
    }
    
    length = Convert_Int_To_String(cData, var);
    if(stamp != 0)
    {
        division = Scale_To_Int(Scale);
        while(stamp < division)
        {
            stamp *=10;
            size_cData++;
            if(stamp == 0) break;
        }
        while(size_cData > 0)
        {
            for(uint8_t i = length; ; i--)
            {
                if(cData[i-1] == '-' ||i == 0)
                {
                    cData[i] = '0';
                    break;
                }
                else
                {
                    cData[i] = cData[i-1];
                }
            }
            length++;
            size_cData--;
        }
        division = division/10;
        size_cData = length;
        while(division > 0)
        {
            division = division/10;
            cData[size_cData] = cData[size_cData-1];
            size_cData--;
            if(division == 0) 
            {
              cData[size_cData]='.';
              length++;
            }
        }
    }
    return length;
}

uint8_t Reset_Chip (void)
{
    __disable_irq();
    //Reset
    NVIC_SystemReset(); 
}


