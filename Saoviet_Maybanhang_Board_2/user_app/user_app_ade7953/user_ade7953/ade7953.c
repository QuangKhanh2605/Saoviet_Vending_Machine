#include "stm32l0xx_hal.h"
#include "spi.h"
#include "ade7953.h"

sADE_Struct 				sADE;
sMeter_Struct 				sMeter_Real_Value;
sMeter_Constant_Struct 	    sMeter_Constant;
sMeter_Status_Struct 	    sMeter_Status;


const uint8_t ADEConfigInfo[40][4] = {
//ADE HAdd - ADE LAdd - DataLength - EPPROM Addr
     0X00,    0X04,    0X01,    0X00,	// 1			LCYCMODE
     0X00,    0X07,    0X01,    0X05,	// 2			PGA_V
     0X00,    0X08,    0X01,    0X0A,	// 3			PGA_IA
     0X00,    0X09,    0X01,    0X10,	// 4			PGA_IB
     0X08,    0X00,    0X01,    0X15,	// 5			EX_REF
     0X01,    0X00,    0X02,    0X1A,	// 6			ZXTOUT
     0X01,    0X02,    0X02,    0X20,	// 7			CONFIG
     0X01,    0X03,    0X02,    0X25,	// 8			CF1DEN
     0X01,    0X04,    0X02,    0X2A,	// 9			CF2DEN
     0X01,    0X07,    0X02,    0X30,	// 10		   CFMODE
     0X01,    0X08,    0X02,    0X35,	// 11		   PHCALA
     0X01,    0X09,    0X02,    0X3A,	// 12		   PHCALB
     0X01,    0X10,    0X02,    0X40,	// 13		   ALT_Output
     0X02,    0X01,    0X03,    0X45,	// 14		   ACCMODE
     0X02,    0X03,    0X03,    0X4A,	// 15		   AP_NOLOAD
     0X02,    0X04,    0X03,    0X50,	// 16		   VAR_NOLOAD
     0X02,    0X05,    0X03,    0X55,	// 17		   VA_NLOAD
     0X02,    0X24,    0X03,    0X5A,	// 18		   OVLVL
     0X02,    0X25,    0X03,    0X60,	// 19		   OILVL
     0X02,    0X2C,    0X03,    0X65,	// 20		   IRQENA
     0X02,    0X2F,    0X03,    0X6A,	// 21		   IRQENB
     0X02,    0X80,    0X03,    0X70,	// 22		   AIGAIN
     0X02,    0X81,    0X03,    0X75,	// 23		   AVGAIN
     0X02,    0X82,    0X03,    0X7A,	// 24		   AWGAIN
     0X02,    0X83,    0X03,    0X80,	// 25		   AVARGAIN
     0X02,    0X84,    0X03,    0X85,	// 26		   AVAGAIN
     0X02,    0X86,    0X03,    0X8A,	// 27		   AIRMSOS
     0X02,    0X88,    0X03,    0X90,	// 28		   VRMSOS		
     0X02,    0X89,    0X03,    0X95,	// 29		   AWATTOS		
     0X02,    0X8A,    0X03,    0X9A,	// 30		   AVAROS		
     0X02,    0X8B,    0X03,    0XA0,	// 31		   AVAOS		
     0X02,    0X8C,    0X03,    0XA5,	// 32		   BIGAIN		
     0X02,    0X8D,    0X03,    0XAA,	// 33		   BVGAIN		
     0X02,    0X8E,    0X03,    0XB0,	// 34		   BWGAIN		
     0X02,    0X8F,    0X03,    0XB5,	// 35		   BVARGAIN		
     0X02,    0X90,    0X03,    0XBA,	// 36		   BVAGAIN
     0X02,    0X92,    0X03,    0XC0,	// 37		   BIRMSOS	
     0X02,    0X95,    0X03,    0XC5,	// 38		   BWATTOS		
     0X02,    0X96,    0X03,    0XCA,	// 39		   BVAROS	
     0X02,    0X97,    0X03,    0XD0,	// 40		   BVAOS	
};
//     0X04,    0X00,    0X00,	// 3			PGA_IA
//     0X00,    0X00,    0X00,	// 2			PGA_V
//     0X3B,    0X01,    0X00,	// 8			CF1DEN	//defaul: 1600
uint8_t ADEConfigDefault[40][3] = {
//    HByte - MByte - LByte
     0X40,    0X00,    0X00,  // 1			LCYCMODE
     0X00,    0X00,    0X00,	// 2			PGA_V
     0X00,    0X00,    0X00,	// 3			PGA_IA
     0X00,    0X00,    0X00,	// 4			PGA_IB
     0X00,    0X00,    0X00,	// 5			EX_REF
     0XFF,    0XFF,    0X00,	// 6			ZXTOUT
     0XA0,    0X04,    0X00,	// 7			CONFIG
     0X49,    0XBF,    0X00,	// 8			CF1DEN	//defaul: 1600
     0X3B,    0X01,    0X00,	// 9			CF2DEN	//defaul: 1600
     0X00,    0X10,    0X00,	// 10		   CFMODE
     0X00,    0X70,    0X00,	// 11		   PHCALA
     0X00,    0X00,    0X00,	// 12		   PHCALB
     0X00,    0X00,    0X00,	// 13		   ALT_Output
     0X00,    0X00,    0X00,	// 14		   ACCMODE
     0X00,    0XE4,    0X19,	// 15		   AP_NOLOAD
     0X00,    0XE4,    0X19,	// 16		   VAR_NOLOAD
     0X00,    0X00,    0X00,	// 17		   VA_NLOAD
     0XFF,    0XFF,    0XFF,	// 18		   OVLVL
     0XFF,    0XFF,    0XFF,	// 19		   OILVL
     0X10,    0X07,    0XC7,	// 20		   IRQENA
     0X00,    0X00,    0X00,	// 21		   IRQENB
     0X40,    0X00,    0X00,	// 22		   AIGAIN
     0X40,    0X00,    0X00,	// 23		   AVGAIN
     0X49,    0XB9,    0XDB,	// 24		   AWGAIN 
     0X40,    0X00,    0X00,	// 25		   AVARGAIN
     0X40,    0X00,    0X00,	// 26		   AVAGAIN
     0X00,    0X00,    0X00,	// 27		   AIRMSOS
     0X00,    0X00,    0X00,	// 28		   VRMSOS		
     0X00,    0X00,    0X00,	// 29		   AWATTOS		
     0X00,    0X00,    0X00,	// 30		   AVAROS		
     0X00,    0X00,    0X00,	// 31		   AVAOS		
     0X40,    0X00,    0X00,	// 32		   BIGAIN		
     0X40,    0X00,    0X00,	// 33		   BVGAIN		
     0X40,    0X00,    0X00,	// 34		   BWGAIN		
     0X40,    0X00,    0X00,	// 35		   BVARGAIN		
     0X40,    0X00,    0X00,	// 36		   BVAGAIN
     0X00,    0X00,    0X00,	// 37		   BIRMSOS	
     0X00,    0X00,    0X00,	// 38		   BWATTOS		
     0X00,    0X00,    0X00,	// 39		   BVAROS	
     0X00,    0X00,    0X00,	// 40		   BVAOS	
};

const uint8_t ADEReadoutReg[22][3] = {
     0X01,    0X0A,    0x02,  //01        PFA
     0X01,    0X0B,    0x02,  //02        PFB
     0X01,    0X0C,    0x02,  //03        ANGLE_A
     0X01,    0X0D,    0x02,  //04        ANGLE_B
     0X01,    0X0E,    0x02,  //05        PERIOD
     0X03,    0X10,    0x04,  //06        AVA
     0X03,    0X11,    0x04,  //07        BVA
     0X03,    0X12,    0x04,  //08        AWATT
     0X03,    0X13,    0x04,  //09        BWATT
     0X03,    0X14,    0x04,  //10        AVAR
     0X03,    0X15,    0x04,  //11        BVAR
     0X03,    0X1A,    0x04,  //12        IRMSA
     0X03,    0X1B,    0x04,  //13        IRMSB
     0X03,    0X1C,    0x04,  //14        VRMS
     0X03,    0X1E,    0x04,  //15        AENERGYA
     0X03,    0X1F,    0x04,  //16        AENERGYB
     0X03,    0X20,    0x04,  //17        RENERGYA
     0X03,    0X21,    0x04,  //18        RENERGYB
     0X03,    0X22,    0x04,  //19        APENERGYA
     0X03,    0X23,    0x04,  //20        APENERGYB
     0X03,    0X2E,    0x04,  //21        RSTIRQSTATA
	  0X03,    0X01,    0x04,  //22        ACCMODE 
};

void ADE7953_Init(void) {
	//Reset ADE
  
	//Init recommeded settings
	ADE7953_RecommendedSetting();
	
	//Write config & calib value
	ADE7953_Write_Config();
}

void ADE7953_RecommendedSetting (void) {
  uint8_t   SetCom1[4] = {0x00,0xFE,0x00,0xAD}; // unlock - 2 byte add + 2 byte value
  uint8_t   SetCom2[5] = {0x01,0x20,0x00,0x00,0x30}; // config optimum
  uint8_t   ReceiveBuff[5];
  
  ADE7953_CS_OFF;
  HAL_SPI_TransmitReceive(&hspi1,SetCom1,ReceiveBuff,4,100);
  ADE7953_CS_ON;
  
  ADE7953_CS_OFF;
  HAL_SPI_TransmitReceive(&hspi1,SetCom2,ReceiveBuff,5,100);
  ADE7953_CS_ON;
}

int8_t ADE7953_Write_Reg (uint8_t Command_Index, bool SequenWrite) {
  uint8_t   SendBuff[8], ReceiveBuff[8], i = 0;
  
  //Write ADE
  SendBuff[0] = ADEConfigInfo[Command_Index][0];
  SendBuff[1] = ADEConfigInfo[Command_Index][1];
  SendBuff[2] = 0x00;
  for(i=0;i<ADEConfigInfo[Command_Index][2];i++)
    SendBuff[i+3] = ADEConfigDefault[Command_Index][i];
  
  ADE7953_CS_OFF;
  HAL_SPI_TransmitReceive(&hspi1,SendBuff,ReceiveBuff,ADEConfigInfo[Command_Index][2] + 3,100);
  ADE7953_CS_ON;
  
  if(SequenWrite) {
    ADE7953_CS_OFF;
	 HAL_SPI_TransmitReceive(&hspi1,SendBuff,ReceiveBuff,ADEConfigInfo[Command_Index][2] + 3,100);
	 ADE7953_CS_ON; 
  }
  
  //Read ADE - Check
  SendBuff[2] = 0x80;
  for(i=0;i<ADEConfigInfo[Command_Index][2];i++)
    SendBuff[i+3] = 0xFF;

  ADE7953_CS_OFF;
  HAL_SPI_TransmitReceive(&hspi1,SendBuff,ReceiveBuff,ADEConfigInfo[Command_Index][2] + 3,100);
  ADE7953_CS_ON; 
  
  if(Command_Index != 13)
    for(i=0;i<ADEConfigInfo[Command_Index][2];i++) {
      if(ReceiveBuff[i+3] != ADEConfigDefault[Command_Index][i])
        return -1;
    }
  else
    if(ReceiveBuff[5] != ADEConfigDefault[Command_Index][2])
      return -1;
  
  return 1;
}

int8_t ADE7953_Write_Config (void){
  uint8_t   i = 0;
  int8_t    writeReVal = 0;
  
  for(i=0;i<40;i++) {
    if((i==7)||(i==8))
      writeReVal = ADE7953_Write_Reg(i,true);
    else
      writeReVal = ADE7953_Write_Reg(i,false);
    
    if(writeReVal == -1)
        return -1;
  }
  
  return 1;
}

void ADE7953_Read_Reg (uint8_t Reg_Index, uint32_t* ReVal) {
  uint8_t   SendBuff[8], ReceiveBuff[8], i = 0;
  
  //Read ADE
  SendBuff[0] = ADEReadoutReg[Reg_Index][0];
  SendBuff[1] = ADEReadoutReg[Reg_Index][1];
  SendBuff[2] = 0x80;
  for(i=0;i<ADEReadoutReg[Reg_Index][2];i++)
    SendBuff[i+3] = 0xFF;
  
  ADE7953_CS_OFF;
  HAL_SPI_TransmitReceive(&hspi1,SendBuff,ReceiveBuff,ADEReadoutReg[Reg_Index][2] + 3,100);
  ADE7953_CS_ON;
  
  switch(ADEReadoutReg[Reg_Index][2]){
    case 0x02:
      *ReVal = (ReceiveBuff[3]<<8)|ReceiveBuff[4];
      break;
    case 0x04:
      *ReVal = (ReceiveBuff[3]<<24)|(ReceiveBuff[4]<<16)|(ReceiveBuff[5]<<8)|ReceiveBuff[6];
      break;
    default:
      break;
  }
}

