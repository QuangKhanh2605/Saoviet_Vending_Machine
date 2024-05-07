#include "user_convert_variable.h"

/*
    Chuyen Scale sang Uint
*/
uint32_t Calculator_Scale(uint8_t Scale)
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
uint32_t Calculator_Value_Scale(uint32_t Value, uint8_t Scale, uint8_t Scale_Default)
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
  uint64_t Stamp_Scale = Value * Calculator_Scale(Scale_Default);
  return Stamp_Scale/Calculator_Scale(Scale);
}

/*=============== Convert Int To String ==============*/

/*
    @brief  Convert Int to String
    @return Length var
*/
uint8_t Convert_Int_To_String(uint8_t cData[], int var)
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
        stamp = var;
    
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

/*
    @brief  Convert Int to String(Scale)
    @return Length Data
*/
uint8_t Convert_Int_To_String_Scale(uint8_t cData[], int var, uint8_t Scale)
{
    uint8_t length = 0;
    uint32_t division = 0;
    uint32_t stamp = 0;
    uint8_t size_cData = 0;
    if(var < 0)
        stamp = 0 - var;
    else
        stamp = var;
    
    length = Convert_Int_To_String(cData, var);
    if(stamp != 0)
    {
        division = Calculator_Scale(Scale);
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
                    cData[i] = cData[i-1];
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

//---------------------- Convert Int to String Point ---------------------
/*
    @brief  Convert Int to String at Point
    @param  *Pos location in array cData
    @return Length Data
*/
uint8_t Convert_Point_Int_To_String(uint8_t cData[], uint16_t *Pos, int var)
{
    uint16_t length = 0;
    uint32_t stamp = 0;
    uint64_t division = 10;
    length = *Pos;
    if(var < 0)
    {
        stamp = 0 - var;
        cData[length++] = '-';
    }
    else
        stamp = var;
    
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
    
    *Pos = length;
    return length;
}
/*
    @brief  Convert Int to String at Point (Scale)
    @param  *Pos location in array cData
    @return Length Data
*/
uint8_t Convert_Point_Int_To_String_Scale(uint8_t cData[], uint16_t *Pos, int var, uint8_t Scale)
{
    uint16_t length = 0;            //Vi tri du lieu 
    uint16_t location_pos = 0;      //Vi tri dau tien cua Pos
    uint32_t division = 0;          //Kiem tra do dai du lieu cua variable tu scale
    uint32_t stamp = 0;             //Stamp cua variable
    uint8_t size_cData = 0;
    
    location_pos = *Pos;
    if(var < 0)
        stamp = 0 - var;
    else
        stamp = var;
    
    Convert_Point_Int_To_String(cData, Pos, var);
    length = *Pos;
    if(stamp != 0)
    {
        division = Calculator_Scale(Scale);
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
                if(cData[i-1] == '-' ||i == location_pos)
                {
                    cData[i] = '0';
                    break;
                }
                else
                    cData[i] = cData[i-1];
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
    *Pos = length;
    return length;
}

//---------------------Convert Variable Packet Integer ----------------------
uint32_t Convert_String_To_Integer_Point(uint8_t aData[], uint16_t Length, uint16_t *Pos)
{
    uint16_t i = *Pos;
    uint32_t stamp = 0;
    uint8_t length = 0;
    uint32_t division = 1;
    while(i < Length)
    {
        if(aData[i] >= '0' && aData[i] <= '9')
          break;
        else
          i++;
    }

    while(i < Length)
    {
        if(aData[i] >= '0' && aData[i] <= '9')
        {
          length++;
          i++;
        }
        else
        {
          i--;
          break;
        }
    }
    
    if(i == Length) i--; 
    *Pos = i + 1;

    while(length != 0)
    {
        stamp += (aData[i] - 0x30) * division;
        length--;
        division *=10;
        i--;
    }
        
    return stamp;
}

/*
    @brief  Dong goi bien Integer vao mang
*/
void Convert_Var_Packet_Integer (uint8_t *pTarget, uint16_t *LenTarget, uint32_t Data)
{
    uint16_t Pos = *LenTarget;
    
    uint64_t division = 10;
    
    while( Data/division > 0 )
    {
        division *= 10;
    }
    division = division/10;
    while(division > 0)
    {
        pTarget[Pos++] = Data/division + 0x30;
        Data = Data - (Data/division) * division;
        division = division/10;
    }
    *LenTarget = Pos ;
}

//------------------------Insert String------------------------
/*
    @brief  Insert String to String
*/
void Insert_String_To_String(uint8_t *destination, uint16_t *PosDes, uint8_t source[], uint16_t PosSour, uint32_t length)
{
    for(uint16_t i = 0; i< length; i++)
        destination[(*PosDes)++] = source[PosSour + i];
}


