#include "user_app_ade7953.h"
#include "ade7953.h"
/*=============Function Static==============*/
static uint8_t fevent_ade_entry(uint8_t event);
static uint8_t fevent_ade_check_source(uint8_t event);
static uint8_t fevent_ade_handle(uint8_t event);
static uint8_t fevent_ctrl_led_status(uint8_t event);
/*================ Struct =================*/
sEvent_struct         sEventAppAde7953[] =
{   
  { _EVENT_ADE_ENTRY,           1, 0, 5,                fevent_ade_entry}, 
  { _EVENT_ADE_CHECK_SCOURCE,   1, 0, 5,                fevent_ade_check_source},
  { _EVENT_ADE_HANDLE,          0, 0, 5,                fevent_ade_handle},
  
  { _EVENT_CTRL_LED_STATUS,     1, 5, TIME_LED_STATUS,  fevent_ctrl_led_status},
};

Struct_Infor_Electric       sInforElectric={0};
uint64_t Energy = 0;
uint32_t GetTick = 0;
/*=============== Function Static =================*/
static uint8_t fevent_ade_entry(uint8_t event)
{   
    GetTick = HAL_GetTick();
    fevent_enable(sEventAppAde7953, event);
    return 1;
}

static uint8_t fevent_ade_check_source(uint8_t event)
{
    static uint8_t once_handle = 0;
    ADE7953_Read_Reg(VRMS,&sADE.vRMS_u32);
    sInforElectric.Voltage = ((sADE.vRMS_u32 * 383)/1000000);
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

uint32_t count_irq = 0;
          
static uint8_t fevent_ade_handle(uint8_t event)
{
    sMeter_Real_Value.countADE7953++; 
    ADE7953_Read_Reg(AENERGYA,&sADE.active_En_u32);
//          ADE7953_Read_Reg(RENERGYA,&sADE.reactive_En_u32);
  sMeter_Real_Value.fw_Active_En_u64 += (int32_t)sADE.active_En_u32;
  //Check if  fw_Active_En_i32 large enought -> convert to Wh -> add to EEPROM
  if(sMeter_Real_Value.fw_Active_En_u64>2000000)
  {
      Energy += (sMeter_Real_Value.fw_Active_En_u64*4343)>>20;
      sMeter_Real_Value.fw_Active_En_u64 = 0;
  }
    if(sMeter_Status.adeIntFlag == 1)
    {
      count_irq++;
      sMeter_Status.adeIntFlag = 0; //Clear flag 
      ADE7953_Read_Reg(RSTIRQSTATA,&sADE.interrup_Status_u32); //Read interrup status register
      //Handle ADE7953 interrup
      if(sADE.interrup_Status_u32&0x00000200)	//Active power changed sign
      {
        ADE7953_Read_Reg(AENERGYA,&sADE.active_En_u32);

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

          
    if(sMeter_Real_Value.countADE7953 > 100) // Chu ki` do gia tri, bao nhieu la hop li, ko bi mat data
    {
            
          ADE7953_Read_Reg(VRMS,&sADE.vRMS_u32);
          ADE7953_Read_Reg(IRMSA,&sADE.i1RMS_u32);
          
          uint64_t Voltage = 0;
          uint64_t Current = 0;
            
          Voltage = ((sADE.vRMS_u32 * 384)/1000000);
          Current = (((uint64_t)sADE.i1RMS_u32 * 5661)/10000000);
          
          sInforElectric.Voltage = Voltage;
          sInforElectric.Current = Current;
          
          //sInforElectric.Voltage = (sADE.vRMS_u32 * 257) >> 16;
          //sInforElectric.Current = (sADE.i1RMS_u32 * 64)>>16;
          
          if(sADE.active_En_u32 != 0)
          sADE.active_En_Noload_u8 = 0;             // 
          
          if(sADE.active_En_Noload_u8 == 0)
          {
            ADE7953_Read_Reg(AWATT,&sADE.instan_ActPw_u32);
            sMeter_Real_Value.instan_ActPw_i32 = (((int32_t)sADE.instan_ActPw_u32)/256)*(sMeter_Constant.W_Var_Constant>>8);
          }
          else
          {
            sInforElectric.Current  = 0;
          }
          
          sMeter_Real_Value.countADE7953 = 0;
    }
  
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
    HAL_GPIO_WritePin(RESET_ADE_GPIO_Port, RESET_ADE_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(RESET_ADE_GPIO_Port, RESET_ADE_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
    ADE7953_Init();
    //ADE7953_Read_Reg(RSTIRQSTATA,&sADE.interrup_Status_u32);
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

