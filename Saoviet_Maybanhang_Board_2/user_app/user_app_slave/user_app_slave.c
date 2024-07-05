#include "user_app_slave.h"
#include "user_modbus_rtu.h"
#include "user_app_ade7953.h"
/*============== Function Static =========*/
static uint8_t fevent_slave_entry(uint8_t event);
static uint8_t fevent_slave_receive_handle(uint8_t event);
static uint8_t fevent_slave_complete_receive(uint8_t event);
static uint8_t fevent_init_uart(uint8_t event);
/*=============== Struct =================*/
sEvent_struct               sEventAppSlave[]=
{
  {_EVENT_SLAVE_ENTRY,                 0, 5, 0,                 fevent_slave_entry},
  {_EVENT_SLAVE_RECEIVE_HANDLE,        1, 0, 5,                 fevent_slave_receive_handle},
  {_EVENT_SLAVE_COMPLETE_RECEIVE,      0, 0, 5,                 fevent_slave_complete_receive},
  
  {_EVENT_INIT_UART,                   1, 5, TIME_INIT_UART,    fevent_init_uart},
};

Struct_Infor_Slave          sInforSlave = {ID_DEFAULT, BAUDRATE_DEFAULT};

uint32_t baud_rate_value[8]={1200,2400,4800,9600,19200,38400,57600,115200};
/*========================================*/

static uint8_t fevent_slave_entry(uint8_t event)
{
    fevent_enable(sEventAppSlave, event);
    return 1;
}

static uint8_t fevent_slave_receive_handle(uint8_t event)
{
    static uint16_t countBuffer_uart = 0;

    if(sUart485.Length_u16 != 0)
    {
        if(countBuffer_uart == sUart485.Length_u16)
        {
            countBuffer_uart = 0;
            fevent_active(sEventAppSlave, _EVENT_SLAVE_COMPLETE_RECEIVE);
            return 1;
        }
        else
        {
            countBuffer_uart = sUart485.Length_u16;
        }
    }
    
    fevent_enable(sEventAppSlave, event);
    return 1;
}

static uint8_t fevent_slave_complete_receive(uint8_t event)
{
    Change_Baudrate_AddrSlave_Calib_ATCommand();
    ModbusRTU_Slave();
  
    UTIL_MEM_set(sUart485.Data_a8 , 0x00, sUart485.Length_u16);
    sUart485.Length_u16 = 0;
    fevent_active(sEventAppSlave, _EVENT_SLAVE_RECEIVE_HANDLE);
    return 1;
}

static uint8_t fevent_init_uart(uint8_t event)
{
    Init_AppSlave();
    return 1;
}

/*================= Function Handle ================*/
void Init_AppSlave(void)
{
  if(FLASH_ReadData32(FLASH_ID_BAURATE) == DEFAUL_READ_FLASH) 
  {
    sInforSlave.ID       = FLASH_ReadData32(FLASH_ID_BAURATE+4);
    sInforSlave.Baudrate = FLASH_ReadData32(FLASH_ID_BAURATE+8); 
  }
  
  Uart485_Init(sInforSlave.Baudrate);
  Init_Uart_485_Rx_IT();
}

uint8_t AppSlave_Task(void)
{
    uint8_t i = 0;
    uint8_t Result = false;
    
    for(i = 0; i < _EVENT_SLAVE_END ; i++)
    {
        if(sEventAppSlave[i].e_status == 1)
        {
            Result = true;
            if(sEventAppSlave[i].e_systick == 0 || 
               ((HAL_GetTick() - sEventAppSlave[i].e_systick) >= sEventAppSlave[i].e_period))
            {
                sEventAppSlave[i].e_status = 0;  //Disable event
				sEventAppSlave[i].e_systick = HAL_GetTick();
				sEventAppSlave[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}

/*================ Function Slave Modbus RTU ===============*/
uint8_t ModbusRTU_Slave(void)
{
	uint8_t answer=0;
	if(sUart485.Data_a8[0] == sInforSlave.ID)
	{
		uint8_t frame[50]={0}; 
		sData sFrame;
		sFrame.Data_a8 = frame;
		uint16_t CRC_rx = sUart485.Data_a8[sUart485.Length_u16-1] << 8 | sUart485.Data_a8[sUart485.Length_u16-2];
		uint16_t CRC_check = ModRTU_CRC(&sUart485.Data_a8[0], sUart485.Length_u16-2);
		uint8_t FunCode = sUart485.Data_a8[1];
		if(CRC_check == CRC_rx)
		{
            fevent_enable(sEventAppSlave, _EVENT_INIT_UART);
			answer=1;
			uint16_t addr_data = sUart485.Data_a8[2] << 8 | sUart485.Data_a8[3];
			uint8_t data_frame[40]={0};
			if(FunCode == 0x03)
			{
//				if(sInforElectric.Voltage != 0x7FFF && sInforElectric.Current !=0x7FFF)
//				{
					if(addr_data <= NUMBER_REGISTER_SLAVE -1)
					{
						uint16_t length_register = (sUart485.Data_a8[4] << 8 | sUart485.Data_a8[5])*2;
						uint8_t length_check = (NUMBER_REGISTER_SLAVE - addr_data) * 2;
						if(length_register >= 1 && length_register <= length_check)
						{
							Packing_Frame(data_frame, addr_data, length_register);
							ModRTU_Slave_ACK_Read_Frame(&sFrame, sInforSlave.ID, FunCode, addr_data, length_register/2, data_frame);
						}
						else
						{
							Response_Error(&sFrame, sInforSlave.ID, (uint16_t) (0x80 + FunCode), ERROR_CODE_ADDRESS_OR_QUANTITY);
						}
					}
					else
					{
						Response_Error(&sFrame, sInforSlave.ID, (uint16_t) (0x80 + FunCode), ERROR_CODE_ADDRESS_OR_QUANTITY);
					}
//				}
//				else
//				{
//					Response_Error(&sFrame, sInforSlave.ID, (uint16_t) (0x80 + FunCode), ERROR_CODE_I2C_OR_SENSOR);
//				}
			}
			else if(FunCode == 0x06)
			{
				if(addr_data == 0x0000)
				{
					int16_t addr = sUart485.Data_a8[4] << 8 | sUart485.Data_a8[5];
					if(addr > 0 && addr <= 255)
					{
						sInforSlave.ID = addr;
						FLASH_WritePage(FLASH_ID_BAURATE, sInforSlave.ID, sInforSlave.Baudrate);
						ModRTU_Slave_ACK_Write_Frame(&sFrame, sInforSlave.ID, FunCode, addr_data, 1, data_frame);
					}
					else
					{
						Response_Error(&sFrame, sInforSlave.ID, (uint16_t) (0x80 + FunCode), ERROR_CODE_ADDRESS_OR_QUANTITY);
					}
				}
				else if(addr_data == 0x0001)
				{
					int16_t tmp_baud_rate = sUart485.Data_a8[4] << 8 | sUart485.Data_a8[5];
					if(tmp_baud_rate >= 0 && tmp_baud_rate <= 7)
					{
						sInforSlave.Baudrate = baud_rate_value[tmp_baud_rate];
						Uart485_Init(sInforSlave.Baudrate);
                        Init_Uart_485_Rx_IT();
						FLASH_WritePage(FLASH_ID_BAURATE, sInforSlave.ID, sInforSlave.Baudrate);
						ModRTU_Slave_ACK_Write_Frame(&sFrame, sInforSlave.ID, FunCode, addr_data, 1, data_frame);
					}
					else
					{
						Response_Error(&sFrame, sInforSlave.ID, (uint16_t) (0x80 + FunCode), ERROR_CODE_ADDRESS_OR_QUANTITY);
					}
				}
				else
				{
					Response_Error(&sFrame, sInforSlave.ID, (uint16_t) (0x80 + FunCode), ERROR_CODE_ADDRESS_OR_QUANTITY);
				}
			}
			else
			{
				Response_Error(&sFrame, sInforSlave.ID, (uint16_t) (0x80 + FunCode), ERROR_CODE_FUNCTION_CODE);
			}
                HAL_GPIO_WritePin(NET485IO_GPIO_Port, NET485IO_Pin, GPIO_PIN_SET);
                HAL_Delay(10);
                HAL_UART_Transmit(&uart_485, sFrame.Data_a8, sFrame.Length_u16, 1000);
                HAL_GPIO_WritePin(NET485IO_GPIO_Port, NET485IO_Pin, GPIO_PIN_RESET);
		}
//		else
//		{
//			Response_Error(&sFrame, sInforSlave.ID, (uint16_t) (0x80 + FunCode), ERROR_CODE_CHECK_CRC);
//            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
//            HAL_Delay(10);
//            HAL_UART_Transmit(&uart_485, sFrame.Data_a8, sFrame.Length_u16, 1000);
//            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
//		}
	}
	return answer;
}

void Change_Baudrate_AddrSlave_Calib_ATCommand(void)
{
	int8_t receive_ctrl=0;
	receive_ctrl = Terminal_Receive();
	if(receive_ctrl == -1)
	{
		Send_Data_Terminal((uint8_t*)"ERROR", 5);
	}
	else if(receive_ctrl == 1)
	{
		sInforSlave.ID       = ID_DEFAULT;
		sInforSlave.Baudrate = BAUDRATE_DEFAULT;
		Send_Data_Terminal((uint8_t*)"SUCCESS", 7);
		
		Uart485_Init(sInforSlave.Baudrate);
		Init_Uart_485_Rx_IT();
		FLASH_WritePage(FLASH_ID_BAURATE, sInforSlave.ID, sInforSlave.Baudrate);
	}
	else if(receive_ctrl == -2)
	{
		AT_Command_IF();
	}
	else if(receive_ctrl == 2)
	{
		uint8_t i=0;
		uint8_t count=0;
        while( i < sUart485.Length_u16)
        {
            if(sUart485.Data_a8[i++] == ' ')  break;
        }
		while( sUart485.Data_a8[i] >= '0' && sUart485.Data_a8[i] <= '9')
		{
			i++;
			count++;
			if(i == sUart485.Length_u16) break;
		}
		i--;
		if(count >0 && count <=3 && i == (sUart485.Length_u16 - 1))
		{
			uint16_t tmp=0;
			if(count == 1) tmp = (sUart485.Data_a8[i] -48);
			if(count == 2) tmp = (sUart485.Data_a8[i] -48) + (sUart485.Data_a8[i-1] -48)*10 ;
			if(count == 3) tmp = (sUart485.Data_a8[i] -48) + (sUart485.Data_a8[i-1] -48)*10 + (sUart485.Data_a8[i-2] -48)*100;
			if(tmp > 0 && tmp <= 255)
			{
				sInforSlave.ID = tmp;
				Send_Data_Terminal((uint8_t*)"SUCCESS", 7);
				FLASH_WritePage(FLASH_ID_BAURATE, sInforSlave.ID, sInforSlave.Baudrate);
			}
			else
			{
				Send_Data_Terminal((uint8_t*)"ERROR", 5);
			}
		}
		else
		{
			Send_Data_Terminal((uint8_t*)"ERROR", 5);
		}
	}
	else if(receive_ctrl == 3)
	{
		uint8_t i=0;
		uint8_t count=0;
        while( i < sUart485.Length_u16)
        {
            if(sUart485.Data_a8[i++] == ' ')  break;
        }
		while( sUart485.Data_a8[i] >= '0' && sUart485.Data_a8[i] <= '7')
		{
			i++;
			count++;
			if(i == sUart485.Length_u16) break;
		}
		i--;
		if(count == 1 && i == (sUart485.Length_u16 - 1))
		{
			uint8_t tmp = sUart485.Data_a8[i] - 48;
			sInforSlave.Baudrate=baud_rate_value[tmp];
			Send_Data_Terminal((uint8_t*)"SUCCESS", 7);
			
            Uart485_Init(sInforSlave.Baudrate);
			Init_Uart_485_Rx_IT();
			FLASH_WritePage(FLASH_ID_BAURATE, sInforSlave.ID, sInforSlave.Baudrate);
		}
		else
		{
			Send_Data_Terminal((uint8_t*)"ERROR", 5);
		}
	}
}

void AT_Command_IF(void)
{
	uint8_t i=0;
	uint8_t j=0;
	uint8_t length_addr=1;
	uint8_t length_baud_rate=1;
    uint8_t Slave_IF[15]={0};
    uint8_t Slave_ID = sInforSlave.ID;
    uint32_t Slave_BR = sInforSlave.Baudrate;
	
	Get_Length_Variable(&length_addr, Slave_ID);
	Get_Length_Variable(&length_baud_rate, Slave_BR);
	
	Slave_IF[i]='I'; Slave_IF[i+1]='F'; Slave_IF[i+2]= '='; 
	i += 3 + length_addr; j = i - 1;
	while(length_addr > 0)
	{
		Slave_IF[j] = (Slave_ID % 10) + ASCII_NUMBER_VALUE;
		Slave_ID = Slave_ID /10;
		length_addr--;
		j--;
	}
	Slave_IF[i]=','; i += 1 + length_baud_rate; j = i - 1;
	
	while(length_baud_rate > 0)
	{
		Slave_IF[j] = (Slave_BR % 10) + ASCII_NUMBER_VALUE;
		Slave_BR = Slave_BR /10;
		length_baud_rate--;
		j--;
	}
	
	Send_Data_Terminal((uint8_t*)Slave_IF, i);
	Send_Data_Terminal((uint8_t*)"SUCCESS", 7);
}

/*
	@brief  Lay do dai chuoi thu duoc
	@retval None
*/
void Get_Length_Variable(uint8_t *length, uint32_t variable)
{
	while(variable/10 >=1)
	{
		(*length)++;
		variable = variable / 10;
	}
}

int8_t Terminal_Receive(void)
{
	uint8_t i=0;
	if(sUart485.Data_a8[i] == 'A' && sUart485.Data_a8[i+1] == 'T') i=i+2;
	else return 0;
	
	if(sUart485.Data_a8[i] == '+') i++;
	else return 0;
	
	if(sUart485.Data_a8[i] == 'R' && sUart485.Data_a8[i+1] == 'E' && sUart485.Data_a8[i+2] == 'S' && sUart485.Data_a8[i+3] == 'E' && sUart485.Data_a8[i+4] == 'T') 
	{
		if(i+5 == sUart485.Length_u16) return 1;
		else
		{
			return 0;
		}
	}
	
	if(sUart485.Data_a8[i] == 'I' && sUart485.Data_a8[i+1] == 'D')
	{
		i=i+2;
        if(sUart485.Data_a8[i++] == '=') 
        {
            if(Get_Uint_In_String(sUart485.Data_a8, i, sUart485.Length_u16) == sInforSlave.ID)
            {
                return 2;
            }
        }  
        return 0;
	}
	if(sUart485.Data_a8[i] == 'I' && sUart485.Data_a8[i+1] == 'F' && sUart485.Data_a8[i+2] == '=') 
	{
		i=i+2;
        if(Get_Uint_In_String(sUart485.Data_a8, i, sUart485.Length_u16) == sInforSlave.ID)
        {
            return -2;
        }
        return 0;
	}
	
	if(sUart485.Data_a8[i] == 'B' && sUart485.Data_a8[i+1] == 'R')
	{
		i=i+2;
        if(sUart485.Data_a8[i++] == '=') 
        {
            if(Get_Uint_In_String(sUart485.Data_a8, i, sUart485.Length_u16) == sInforSlave.ID)
            {
                return 3;
            }
        }  
        return 0;
	}
	return 0;
}


void Send_Data_Terminal(uint8_t data[], uint8_t Length)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_UART_Transmit(&uart_485,(uint8_t*)"\r\n",(uint16_t)sizeof("\r\n"),1000);
	HAL_UART_Transmit(&uart_485,(uint8_t*)data, Length,1000);
	HAL_UART_Transmit(&uart_485,(uint8_t*)"\r\n",(uint16_t)sizeof("\r\n"),1000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);	
}

void Packing_Frame(uint8_t data_frame[], uint16_t addr_register, uint16_t length )
{				
	uint8_t i=0;
	// address slave
	if(addr_register <= 0 && i < length)
	{
		data_frame[i++] = 0x00;
		data_frame[i++] = sInforSlave.ID;
	}
	// baudrate
	if(addr_register <= 1 && i < length)
	{
		uint8_t j=0;
		while(j<8)
		{
			if(baud_rate_value[j] == sInforSlave.Baudrate)
			{
				break;
			}
			else j++;
		}
		data_frame[i++] = 0x00;
		data_frame[i++] = j;
	}
	// Voltage value
	if(addr_register <=2 && i<length)
	{
		data_frame[i++] = sInforElectric.Voltage >> 8;
		data_frame[i++] = sInforElectric.Voltage;
	}
	// Voltage Unit
	if(addr_register <=3 && i<length)
	{
		data_frame[i++] = 0x00;
		data_frame[i++] = 0x00;
	}
	// Voltage decimal points
	if(addr_register <=4 && i<length)
	{
		data_frame[i++] = 0x00;
		data_frame[i++] = 0x01;
	}
	// Current value
	if(addr_register <=5 && i<length)
	{
		data_frame[i++] = sInforElectric.Current >> 8;
		data_frame[i++] = sInforElectric.Current;
	}
	// Current Uint
	if(addr_register <=6 && i<length)
	{
		data_frame[i++] = 0x00;
		data_frame[i++] = 0x00;
	}
	// Current decimal points 
	if(addr_register <=7 && i<length)
	{
		data_frame[i++] = 0x00;
		data_frame[i++] = 0x01;
	}
    // power value high
	if(addr_register <=8 && i<length)
	{
		data_frame[i++] = sInforElectric.Power >> 24;
		data_frame[i++] = sInforElectric.Power >> 16;
	}
	// power value low
	if(addr_register <=9 && i<length)
	{
		data_frame[i++] = sInforElectric.Power >> 8;
		data_frame[i++] = sInforElectric.Power ;
	}
	// power unit 
	if(addr_register <=10 && i<length)
	{
		data_frame[i++] = 0x00;
		data_frame[i++] = 0x00;
	}
	// power decimal points 
	if(addr_register <=11 && i<length)
	{
		data_frame[i++] = 0x00;
		data_frame[i++] = 0x03;
	}
    
    // energy value high
	if(addr_register <=12 && i<length)
	{
		data_frame[i++] = sInforEnergy.Dis_Energy >> 24;
		data_frame[i++] = sInforEnergy.Dis_Energy >> 16;
	}
	// energy value low
	if(addr_register <=13 && i<length)
	{
		data_frame[i++] = sInforEnergy.Dis_Energy >> 8;
		data_frame[i++] = sInforEnergy.Dis_Energy ;
	}
	// energy unit 
	if(addr_register <=14 && i<length)
	{
		data_frame[i++] = 0x00;
		data_frame[i++] = 0x00;
	}
	// energy decimal points 
	if(addr_register <=15 && i<length)
	{
		data_frame[i++] = 0x00;
		data_frame[i++] = 0x03;
	}
}


/*==================== Function Handle =====================*/
uint32_t Get_Uint_In_String(uint8_t aData[], uint16_t posStart, uint16_t posEnd)
{
    if(posStart >= posEnd) return 0;
    uint16_t Length = 0;
    uint32_t number = 0;
    uint32_t division = 1;
    
    while(posStart <= posEnd)
    {
        if(aData[posStart] < '0' || aData[posStart] > '9') posStart++;
        else
        {
            break;
        }
    }
    
    while(posStart <= posEnd)
    {
        if(aData[posStart] >= '0' && aData[posStart] <= '9') 
        {
            posStart++;
            Length++;
        }
        else
        {
            posStart--;
            break;
        }
    }
    if(Length < 9)
    {
        while(Length > 0)
        {
            number += (aData[posStart] - '0') * division;
            posStart--;
            Length--;
            division = division*10;
        }
    }
    else
    {
        return 0;
    }
    return number;
}


/**
  @brief  Khoi tao Uart2
  @param  sUart2 Struct uart muon lam viec
	@param  baud_rate toc do baud rate
  @retval None
  */
void Uart485_Init(uint32_t baud_rate)
{
  uart_485.Instance = USART2;
  uart_485.Init.BaudRate = baud_rate;
  uart_485.Init.WordLength = UART_WORDLENGTH_8B;
  uart_485.Init.StopBits = UART_STOPBITS_1;
  uart_485.Init.Parity = UART_PARITY_NONE;
  uart_485.Init.Mode = UART_MODE_TX_RX;
  uart_485.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart_485.Init.OverSampling = UART_OVERSAMPLING_16;
  uart_485.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  uart_485.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&uart_485) != HAL_OK)
  {
     __disable_irq();
		while (1)
		{
		}
  }
}

/*
	@brief  Ghi du lieu vao Flash
	@param  Check sau kiem tra xem du lieu da duoc nap chua
	@param  data1, data2, data3, data4 du lieu muon ghi vao flash
	@retval None
*/
void FLASH_WritePage(uint32_t address_flash, uint32_t data1, uint32_t data2)
{
  HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef EraseInit;
	EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInit.PageAddress = address_flash;
	EraseInit.NbPages = (1024)/FLASH_PAGE_SIZE;
	uint32_t PageError = 0;
	HAL_FLASHEx_Erase(&EraseInit, &PageError);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address_flash , DEFAUL_READ_FLASH);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address_flash + 4, data1); //4 byte dau tien
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address_flash + 8, data2); //4 byte tiep theo
  HAL_FLASH_Lock();
}

/*
	@brief  Doc byte tu Flash
	@param  addr dia chi byte muon doc trong flash
	@return du lieu tai dia chi muon doc
*/
uint32_t FLASH_ReadData32(uint32_t addr)
{
	uint32_t data = *(__IO uint32_t *)(addr);
	return data;
}

uint8_t Reset_Chip (void)
{
    __disable_irq();
    //Reset
    NVIC_SystemReset(); 
}


