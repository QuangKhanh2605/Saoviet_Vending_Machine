#include "user_app_ade7953.h"
#include "ade7953.h"
#include "user_app_slave.h"
/*=============Function Static==============*/
static uint8_t fevent_ade_entry(uint8_t event);
static uint8_t fevent_ade_check_source(uint8_t event);
static uint8_t fevent_ade_handle(uint8_t event);
static uint8_t fevent_ade_save_energy(uint8_t event);

static uint8_t fevent_ctrl_led_status(uint8_t event);
/*================ Struct =================*/
sEvent_struct         sEventAppAde7953[] =
{   
  { _EVENT_ADE_ENTRY,           1, 0, 5,                    fevent_ade_entry}, 
  { _EVENT_ADE_CHECK_SCOURCE,   0, 0, 5,                    fevent_ade_check_source},
  { _EVENT_ADE_HANDLE,          1, 0, 10,                   fevent_ade_handle},
  
  { _EVENT_ADE_SAVE_ENERGY,     1, 5, TIME_SAVE_ENERGY,     fevent_ade_save_energy},
  
  { _EVENT_CTRL_LED_STATUS,     1, 5, TIME_LED_STATUS,      fevent_ctrl_led_status},
};

Struct_Infor_Electric       sInforElectric={0};
Struct_Infor_Energy         sInforEnergy  ={0};
/*=============== Function Static =================*/
static uint8_t fevent_ade_entry(uint8_t event)
{   
    fevent_enable(sEventAppAde7953, event);
    return 1;
}

static uint8_t fevent_ade_check_source(uint8_t event)
{
    static uint8_t once_handle = 0;
    ADE7953_Read_Reg(VRMS,&sADE.vRMS_u32);
    sInforElectric.Voltage = ((sADE.vRMS_u32 * 384)/10000000);
    if(sInforElectric.Voltage > VOLTAGE_ACTIVE_ADE)
    {
        if(once_handle != 0)
        {
            Init_Ade7953();
        }
        fevent_active(sEventAppAde7953, _EVENT_ADE_HANDLE);
        return 1;
    }
    else
    {
        once_handle = 1;
    }
    fevent_enable(sEventAppAde7953, event);
    return 1;
}
    
static uint8_t fevent_ade_handle(uint8_t event)
{
    static uint32_t CountPowerOff = 0;
    ADE7953_Read_Reg(AENERGYA, &sADE.active_En_u32);
    if(sADE.active_En_u32 > 4294967296 - 100000000)
    {
        sADE.active_En_u32 = 4294967296 - sADE.active_En_u32;
    }
    else if(sADE.active_En_u32 < 100000000)
    {
        sADE.active_En_u32 = sADE.active_En_u32;
    }
    else sADE.active_En_u32 = 0;
    
    if(sADE.active_En_u32 != 0)
    sADE.active_En_Noload_u8 = 0;

    sInforEnergy.Reg_Energy = sInforEnergy.Reg_Energy + sADE.active_En_u32;

    sInforEnergy.Pre_Energy = ((sInforEnergy.Reg_Energy*1000/(18879-1))/1600);

    sInforEnergy.Dis_Energy = sInforElectric.Energy + sInforEnergy.Pre_Energy;
    
    sADE.active_En_u32 = 0;

    if(sMeter_Status.adeIntFlag == 1)
    {
      sMeter_Status.adeIntFlag = 0; //Clear flag 
      ADE7953_Read_Reg(RSTIRQSTATA,&sADE.interrup_Status_u32); //Read interrup status register
      //Handle ADE7953 interrup
      if(sADE.interrup_Status_u32&0x00000200)	//Active power changed sign
      {
//        ADE7953_Read_Reg(AENERGYA,&sADE.active_En_u32);

//        if(sADE.active_En_u32&0x80000000)
//              Save_RV_Active_En(sTariffManage.RateNow);
//        else
//              Save_FW_Active_En(sTariffManage.RateNow);
      }
      if(sADE.interrup_Status_u32&0x00000400)	//Reactive power changed sign
      {
        ADE7953_Read_Reg(RENERGYA,&sADE.reactive_En_u32);
        

//        if(sADE.reactive_En_u32&0x80000000)
//            Save_RV_Reactive_En(sTariffManage.RateNow);
//        else
//            Save_FW_Reactive_En(sTariffManage.RateNow);
      }
      if(sADE.interrup_Status_u32&0x00000040)	//Active power noload
      {
          ADE7953_Read_Reg(ACCMODE,&sADE.accmode_Status_u32);
          if(sADE.accmode_Status_u32&0x10000)
              sADE.active_En_Noload_u8 = 1;
      }
      if(sADE.interrup_Status_u32&0x00000080)	//Reactive power noload
      {
          ADE7953_Read_Reg(ACCMODE,&sADE.accmode_Status_u32);
          if(sADE.accmode_Status_u32&0x40000)
          {
              sADE.reactive_En_Noload_u8 = 1;
          }
          
      }
    }
          
    sMeter_Real_Value.countADE7953++;
    if(sMeter_Real_Value.countADE7953 > 100) // Chu ki` do gia tri, bao nhieu la hop li, ko bi mat data
    {
          ADE7953_Read_Reg(VRMS,&sADE.vRMS_u32);
          ADE7953_Read_Reg(IRMSA,&sADE.i1RMS_u32);
          ADE7953_Read_Reg(PFA, &sADE.power_Factor_u32);
          
          ADE7953_Read_Reg(AWATT, &sADE.instan_ActPw_u32);
          sADE.instan_ActPw_u32 = 4294967296 - sADE.instan_ActPw_u32;
          int32_t StampPower = 0;
          
          uint64_t Voltage = 0;
          uint64_t Current = 0;
          uint64_t Power   = 0;
          
          StampPower = (int32_t)sADE.instan_ActPw_u32;
            
          Voltage = (((uint64_t)sADE.vRMS_u32 * 38424)/100000000);
          Current = (((uint64_t)sADE.i1RMS_u32 * 9088)/1000000);
          Power   = (((int64_t)StampPower * 5085)/1000000);
          
          if(Voltage < 5) Voltage = 0;
          
          sInforElectric.Voltage = Voltage;
          sInforElectric.Current = Current;
          sInforElectric.Power   = Power;
          
          //sInforElectric.Voltage = (sADE.vRMS_u32 * 257) >> 16;
          //sInforElectric.Current = (sADE.i1RMS_u32 * 64)>>16;
          
          
          if(sADE.active_En_Noload_u8 == 0)
          {
//            sMeter_Real_Value.instan_ActPw_i32 = (((int32_t)sADE.instan_ActPw_u32)/256)*(sMeter_Constant.W_Var_Constant>>8);
            CountPowerOff = 0;
          }
          else
          {
            sInforElectric.Current  = 0;
            CountPowerOff++;
          }
          
          if(CountPowerOff == NUMBER_SAVE_ENERGY)
          {
            fevent_active(sEventAppAde7953, _EVENT_ADE_SAVE_ENERGY);
          }
          

          
          sMeter_Real_Value.countADE7953 = 0;
    }
    
//    ADE7953_Read_Reg(AENERGYA, &sADE.active_En_u32);
//    sADE.active_En_u32 = 4294967295 - sADE.active_En_u32;
//    
//    sInforEnergy.Reg_Energy = sInforEnergy.Reg_Energy + sADE.active_En_u32;
//    sADE.active_En_u32 = 0;
//    sInforEnergy.Pre_Energy = ((sInforEnergy.Reg_Energy*1000/18879)/1600);

    fevent_enable(sEventAppAde7953, event);
    return 1;
}

static uint8_t fevent_ade_save_energy(uint8_t event)
{
    sInforElectric.Energy = sInforEnergy.Dis_Energy;
    FLASH_WritePage(FLASH_ENERGY_METER, sInforElectric.Energy, DEFAUL_READ_FLASH);
    
    sInforEnergy.Reg_Energy = sInforEnergy.Reg_Energy % (18879-1);
    sInforEnergy.Pre_Energy = 0;
    
    fevent_enable(sEventAppAde7953, event);
    return 1;
}

static uint8_t fevent_ctrl_led_status(uint8_t event)
{
    if(sEventAppAde7953[_EVENT_CTRL_LED_STATUS].e_period == TIME_LED_STATUS)
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        sEventAppAde7953[_EVENT_CTRL_LED_STATUS].e_period = 40;
    }
    else
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        sEventAppAde7953[_EVENT_CTRL_LED_STATUS].e_period = TIME_LED_STATUS;
    }

    fevent_enable(sEventAppAde7953, event);
    return 1;
}
/*=================== Fucntion Handle ====================*/

void Init_Ade7953(void)
{
//    HAL_GPIO_WritePin(RESET_ADE_GPIO_Port, RESET_ADE_Pin, GPIO_PIN_SET);
//    HAL_Delay(1);
    HAL_GPIO_WritePin(RESET_ADE_GPIO_Port, RESET_ADE_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
    ADE7953_Init();
    //ADE7953_Read_Reg(RSTIRQSTATA,&sADE.interrup_Status_u32);
}

void Init_Energy(void)
{
  if(FLASH_ReadData32(FLASH_ENERGY_METER) == DEFAUL_READ_FLASH) 
  {
    sInforElectric.Energy   = FLASH_ReadData32(FLASH_ENERGY_METER+4);
    sInforEnergy.Dis_Energy = sInforElectric.Energy;
  }
}

uint8_t AppAde7953_Task(void)
{
	uint8_t i = 0;
	uint8_t Result = false;

	for (i = 0; i < _EVENT_ADE_END; i++)
	{
		if (sEventAppAde7953[i].e_status == 1)
		{
            Result = true;

			if ((sEventAppAde7953[i].e_systick == 0) ||
					((HAL_GetTick() - sEventAppAde7953[i].e_systick)  >=  sEventAppAde7953[i].e_period))
			{
                sEventAppAde7953[i].e_status = 0;  //Disable event
				sEventAppAde7953[i].e_systick= HAL_GetTick();
				sEventAppAde7953[i].e_function_handler(i);
			}
		}
	}
    
	return Result;
}

