#ifndef  __ADE7953_H__
#define  __ADE7953_H__
#include "stm32l0xx_hal.h"
#include "stdlib.h"
#include "stdbool.h"

#define 	ADE7953_CS_Pin		CS_MCU_2_Pin
#define 	ADE7953_CS_Port		CS_MCU_2_GPIO_Port
#define 	ADE7953_Reset_Pin	RESET_ADE_Pin
#define 	ADE7953_Reset_Port	RESET_ADE_GPIO_Port
#define 	ADE7953_IRQ_Pin		IRQ_ADE_Pin
#define 	ADE7953_IRQ_Port	IRQ_ADE_GPIO_Port

#define	    ADE7953_CS_ON		HAL_GPIO_WritePin(ADE7953_CS_Port,ADE7953_CS_Pin,GPIO_PIN_SET)
#define	    ADE7953_CS_OFF		HAL_GPIO_WritePin(ADE7953_CS_Port,ADE7953_CS_Pin,GPIO_PIN_RESET)
#define	    ADE7953_RESET_ON	HAL_GPIO_WritePin(ADE7953_Reset_Port,ADE7953_Reset_Pin,GPIO_PIN_SET)
#define	    ADE7953_RESET_OFF	HAL_GPIO_WritePin(ADE7953_Reset_Port,ADE7953_Reset_Pin,GPIO_PIN_RESET)

#define	    GET_ADE7953_IRQ		HAL_GPIO_ReadPin(ADE7953_IRQ_Port,ADE7953_IRQ_Pin)

//ADE Constant /* Gia tri Maximun cua cac thong so (Full scale) */
#define  ADE_VOLTAGE_FULL_SCALE           9032007
#define  ADE_CURRENT_FULL_SCALE           9032007
#define  ADE_POWERFACTOR_FULL_SCALE       0x7FFF

//Meter gains
#define  VOLATE_GAIN              6553600   //100*2^16
#define  CURRENT_GAIN             6553600   //100*2^16
#define  WH_VARH_GAIN             6553600   //100*2^20
#define  W_VAR_GAIN               6553600   //100*2^16

#define  METTER_CONSTANT          3600

#define  SHUNT_VALUE_1000000      250
#define  CURRENT_CHANNEL_GAIN     1
#define  VOLTAGE_DEVIDER_1        1           //Tu so
#define  VOLTAGE_DEVIDER_2        1000        //Mau so

#define  V_RMS_MAX_GAINED         2551814428         //389.3759808*100*2^16  ((0.5/(1/1000))/sqrt(2)/ADE_POWERFACTOR_FULL_SCALEx100x2^16)
#define  I_RMS_MAX_GAINED         526601705          //80.35304332*100*2^16

#define  CFADEN_VALUE             15105   //(6048000*SHUNT_VALUE_1000000*CURRENT_CHANNEL_GAIN*VOLTAGE_DEVIDER_1)/(METTER_CONSTANT*VOLTAGE_DEVIDER_2)
#define  V_CONSTANT_VALUE         257		//(V_RMS_MAX_GAINED/ADE_VOLTAGE_FULL_SCALE)
#define  I1_CONSTANT_VALUE        64		//(I_RMS_MAX_GAINED/ADE_CURRENT_FULL_SCALE)
#define  I2_CONSTANT_VALUE        75
#define  WH_VARH_CONSTANT_VALUE   4343		//(V_RMS_MAX_GAINED*I_RMS_MAX_GAINED*40544*WH_VARH_GAIN)/(8388607*3600000*VOLATE_GAIN*CURRENT_GAIN)
#define  W_VAR_CONSTANT_VALUE     42161	//(V_RMS_MAX_GAINED*I_RMS_MAX_GAINED*W_VAR_GAIN)/(4862401*VOLATE_GAIN*CURRENT_GAIN)

#define  NUMBER_TARIFF     8              //real: 8 = 9-1

typedef enum _ade_regs {
     PFA
     ,PFB
     ,ANGLE_A
     ,ANGLE_B
     ,PERIOD
     ,AVA
     ,BVA
     ,AWATT
     ,BWATT
     ,AVAR
     ,BVAR
     ,IRMSA
     ,IRMSB
     ,VRMS
     ,AENERGYA
     ,AENERGYB
     ,RENERGYA
     ,RENERGYB
     ,APENERGYA
     ,APENERGYB
     ,RSTIRQSTATA
	 ,ACCMODE
}ade_regs_t;

extern uint8_t ADEConfigDefault[40][3];

typedef struct {
  uint32_t  i1RMS_u32;
  uint32_t  i2RMS_u32;
  uint32_t  vRMS_u32;
  
  uint32_t  instan_AppPw_u32;
  uint32_t  instan_ActPw_u32;
  uint32_t  instan_ReaPw_u32;
  
  uint32_t  apparent_En_u32;
  uint32_t  active_En_u32;
  uint32_t  reactive_En_u32;
  
  uint32_t  power_Factor_u32;
  uint32_t  period_u32;
  uint32_t  angle_u32;
  
  uint32_t  interrup_Status_u32;
  uint32_t  accmode_Status_u32;
  uint8_t	app_En_Noload_u8;
  uint8_t	active_En_Noload_u8;
  uint8_t	reactive_En_Noload_u8;
} sADE_Struct;
extern 	sADE_Struct 	sADE;

typedef struct {
  uint32_t  countADE7953;
  uint32_t  i1RMS_u32;
  uint32_t  i2RMS_u32;
  uint32_t  vRMS_u32;
  
  int32_t   instan_AppPw_i32;
  int32_t   instan_ActPw_i32;
  int32_t   instan_ReaPw_i32;
  
  int16_t   power_Factor_i16;
  int16_t   frequency_i16;
  int16_t   cos_i16;
  
  int64_t   apparent_En_i64;
  uint64_t  fw_Active_En_u64;
  uint64_t  rv_Active_En_u64;
  uint64_t  fw_Reactive_En_u64;
  uint64_t  rv_Reactive_En_u64;
  
  int64_t   eeprom_Apparent_En_VA_i64;
  uint64_t  eeprom_Fw_Active_En_W_i64;
  uint64_t  eeprom_Rv_Active_En_W_i64;
  uint64_t  eeprom_Fw_Reactive_En_Var_i64;
  uint64_t  eeprom_Rv_Reactive_En_Var_i64;
  
  int64_t   display_Apparent_En_VA_i64;
  uint64_t  display_Fw_Active_En_W_i64;
  uint64_t  display_Rv_Active_En_W_i64;
  uint64_t  display_Fw_Reactive_En_Var_i64;
  uint64_t  display_Rv_Reactive_En_Var_i64;
  
  uint64_t  fw_Active_En_Tariff_u64[NUMBER_TARIFF];
  uint64_t  rv_Active_En_Tariff_u64[NUMBER_TARIFF];
  uint64_t  fw_Reactive_En_Tariff_u64[NUMBER_TARIFF];
  uint64_t  rv_Reactive_En_Tariff_u64[NUMBER_TARIFF];
  
  uint64_t  eeprom_Fw_ActEn_Tf_W_i64[NUMBER_TARIFF];
  uint64_t  eeprom_Rv_ActEn_Tf_W_i64[NUMBER_TARIFF];
  uint64_t  eeprom_Fw_ReaEn_Tf_Var_i64[NUMBER_TARIFF];
  uint64_t  eeprom_Rv_ReaEn_Tf_Var_i64[NUMBER_TARIFF];
  
  uint64_t  display_Fw_ActEn_Tf_W_i64[NUMBER_TARIFF];
  uint64_t  display_Rv_ActEn_Tf_W_i64[NUMBER_TARIFF];
  uint64_t  display_Fw_ReaEn_Tf_Var_i64[NUMBER_TARIFF];
  uint64_t  display_Rv_ReaEn_Tf_Var_i64[NUMBER_TARIFF];
  // Khai bao gia tri Maxdemand
  int32_t  display_Value_Maxdemand_i32[NUMBER_TARIFF];
  uint8_t  display_Time_Maxdemand_u8[NUMBER_TARIFF][8];// s/m/h d/m/y
  uint64_t  Latch_Fw_Active_En_W_i64; //
  int32_t   Value_Load_Profile_Demand_i32; //
  uint8_t   Time_Load_Profile_u8[8];
  uint16_t  Freq_load_profile; // 
} sMeter_Struct;
extern 	sMeter_Struct 	sMeter_Real_Value;

typedef struct {
  uint32_t  V_Constant;
  uint32_t  I1_Constant;
  uint32_t  I2_Constant;
  uint32_t  W_Var_Constant;
  uint32_t  Wh_Varh_Constant;
} sMeter_Constant_Struct;
extern 	sMeter_Constant_Struct 	sMeter_Constant;

typedef struct {
  uint8_t   clearMeterDataFlag;
  uint8_t   clearCalibValue;
  
  uint8_t   adeIntFlag;				//1:Have interrupt
  uint8_t   adeErrorFlag;			//1:Init error - 2:
  uint8_t	metterErrorFlag;		//
  uint8_t	activePowerDirect;	//1:Forward	- 2:Reverse
  uint8_t	reactivePowerDirect;	//1:Forward	- 2:Reverse

  uint8_t	calibVolFlag;
  uint8_t	calibCur1Flag;
  uint8_t	calibCur2Flag;
  uint8_t	calibWFlag;
  uint8_t	calibVARFlag;
  uint8_t	calibVAFlag;
  uint8_t	calibPhaseFlag;
  uint8_t	writeCFDENxFlag;
  
  uint32_t	voltageGain;
  uint32_t	current1Gain;
  uint32_t	current2Gain;
  uint32_t	phaGain;
  uint32_t	wGain;
  uint32_t	varGain;
  uint32_t	vaGain;
} sMeter_Status_Struct;
extern 	sMeter_Status_Struct 	sMeter_Status;

void ADE7953_Init(void);
void ADE7953_RecommendedSetting (void);
int8_t ADE7953_Write_Reg (uint8_t Command_Index, bool SequenWrite);
int8_t ADE7953_Write_Config (void);
void ADE7953_Read_Reg (uint8_t Reg_Index, uint32_t* ReVal);
#endif