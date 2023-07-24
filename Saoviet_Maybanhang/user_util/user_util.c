#include "user_util.h"


void UTIL_MEM_set( void *dst, uint8_t value, uint16_t size )
{
    uint8_t* dst8= (uint8_t *) dst;

    while( size-- )
    {
        *dst8++ = value;
    }
}



/*=========== Function Crc =============*/

/* 
    @brief Calculator Crc 2 byte use Crc Modbus RTU
*/
uint8_t Calculator_Crc_U16(uint16_t *crc, uint8_t* buf, uint16_t len)
{
  //uint16_t crc = 0xFFFF;
  
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





