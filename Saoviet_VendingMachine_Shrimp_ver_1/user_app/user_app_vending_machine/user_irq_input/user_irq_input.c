

#include "user_irq_input.h"
#include "user_inc_vending_machine.h"

uint32_t gettick_floor = 0;
uint32_t gettick_top_bot = 0;
uint32_t gettick_motor = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin)
  {
      case FB_Motor_1_Pin:
        if(HAL_GetTick() - gettick_motor > 500)
        {
            gettick_motor = HAL_GetTick();
            IRQ_Pluse_OffMotorPush(1);
        }
        break;
        
      case FB_Motor_2_Pin:
        if(HAL_GetTick() - gettick_motor > 500)
        {
            gettick_motor = HAL_GetTick();
            IRQ_Pluse_OffMotorPush(2);
        }
        break;
        
      case FB_Motor_3_Pin:
        if(HAL_GetTick() - gettick_motor > 500)
        {
            gettick_motor = HAL_GetTick();
            IRQ_Pluse_OffMotorPush(3);
        }
        break;
        
      case FB_Motor_4_Pin:
        if(HAL_GetTick() - gettick_motor > 500)
        {
            gettick_motor = HAL_GetTick();
            IRQ_Pluse_OffMotorPush(4);
        }
        break;
        
      case FB_Motor_5_Pin:
        if(HAL_GetTick() - gettick_motor > 500)
        {
            gettick_motor = HAL_GetTick();
            IRQ_Pluse_OffMotorPush(5);
        }
        break;
        
      case FB_Motor_6_Pin:
        if(HAL_GetTick() - gettick_motor > 500)
        {
            gettick_motor = HAL_GetTick();
            IRQ_Pluse_OffMotorPush(6);
        }
        break;
        
      case FB_Motor_7_Pin:
        if(HAL_GetTick() - gettick_motor > 500)
        {
            gettick_motor = HAL_GetTick();
            IRQ_Pluse_OffMotorPush(7);
        }
        break;
        
      case FB_Motor_8_Pin:
        if(HAL_GetTick() - gettick_motor > 500)
        {
            gettick_motor = HAL_GetTick();
            IRQ_Pluse_OffMotorPush(8);
        }
        break;
        
      case FB_Motor_9_Pin:
        if(HAL_GetTick() - gettick_motor > 500)
        {
            gettick_motor = HAL_GetTick();
            IRQ_Pluse_OffMotorPush(9);
        }
        break;
        
      case FB_Motor_10_Pin:
        if(HAL_GetTick() - gettick_motor > 500)
        {
            gettick_motor = HAL_GetTick();
            IRQ_Pluse_OffMotorPush(10);
        }
        break;
        
      case FB_Top_Elevator_Pin:
        if(HAL_GetTick() - gettick_top_bot > 1000)
        {
            gettick_top_bot = HAL_GetTick();
            sElevator.FloorCurrent = NUMBER_MAX_FLOOR;
            if(sElevator.State == ELEVATOR_UP)
            {
                Control_Elevator(ELEVATOR_STOP, LEVEL_PWM_ELEVATOR_OFF);
                Debug_Floor_Elevator(sElevator.FloorCurrent);
            }
        }
        break;
        
      case FB_Bot_Elevator_Pin:
        if(HAL_GetTick() - gettick_top_bot > 1000)
        {
            gettick_top_bot = HAL_GetTick();
            sElevator.FloorCurrent = 0;
            if(sElevator.State == ELEVATOR_DOWN)
            {
                Control_Elevator(ELEVATOR_STOP, LEVEL_PWM_ELEVATOR_OFF);
                Debug_Floor_Elevator(sElevator.FloorCurrent);
            }
        }
        break;
        
      case FB_Pos_Elevator_Pin:
        if(HAL_GetTick() - gettick_floor > 500)
        {
            gettick_floor = HAL_GetTick();
            
            switch(sElevator.State)
            {
                case ELEVATOR_STOP:
                  break;
                  
                case ELEVATOR_UP:
                    if((HAL_GetTick() - sElevator.TimeOnMotor > 2000) || sElevator.FloorCurrent == 0)
                    {
                        sElevator.FloorCurrent++;
//                        Debug_Encoder();
                    }
                    
                    if(sParamDelivery.StateHanlde == DELIVERY_PURCHASE)
                    {
                        if(sElevator.FloorCurrent == sElevator.FloorHandle)
                            Control_Elevator(ELEVATOR_STOP, LEVEL_PWM_ELEVATOR_OFF);
                    }
                    
                    Debug_Floor_Elevator(sElevator.FloorCurrent);
                  break;
                  
                case ELEVATOR_DOWN:
                    if(sElevator.FloorCurrent > 0)
                    {
                        if((HAL_GetTick() - sElevator.TimeOnMotor > 2000) || sElevator.FloorCurrent == NUMBER_MAX_FLOOR)
                        {
                            sElevator.FloorCurrent--;
//                            Debug_Encoder();
                        }
                    }
                    Debug_Floor_Elevator(sElevator.FloorCurrent);
                  break;
                  
                default:
                  break;
            }
        }
        break;
        
      case Encoder_C2_Pin:
        sElevator.Encoder_C2++;
        break;
        
      case Encoder_C1_Pin:
        sElevator.Encoder_C1++;
        break;
      
      default:
        break;
  }
  
  
//     if (GPIO_Pin == IR_Sensor_Irq_Pin)
//     {
//        sPushMotor.IrSensor++;
//     }
//     
//     if(GPIO_Pin == Count_Pin)
//     {
//        sPushMotor.PulseCount++;
//     }
//     
//     if (GPIO_Pin == Vib_Sensor_Irq_3_Pin)
//     {
//        sStatusVib_Scan.Sensor3++;
//     }
//     
//     if (GPIO_Pin == Vib_Sensor_Irq_2_Pin)
//     {
//        sStatusVib_Scan.Sensor2++;
//     }
//     
//     if (GPIO_Pin == Vib_Sensor_Irq_1_Pin)
//     {
//        sStatusVib_Scan.Sensor1++;
//     }
}

