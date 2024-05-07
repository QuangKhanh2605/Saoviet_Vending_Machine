

#include "user_irq_input.h"
#include "user_inc_vending_machine.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin)
  {
      case FB_Motor_1_Pin:
        IRQ_Pluse_OffMotorPush(1);
        break;
        
      case FB_Motor_2_Pin:
        IRQ_Pluse_OffMotorPush(2);
        break;
        
      case FB_Motor_3_Pin:
        IRQ_Pluse_OffMotorPush(3);
        break;
        
      case FB_Motor_4_Pin:
        IRQ_Pluse_OffMotorPush(4);
        break;
        
      case FB_Motor_5_Pin:
        IRQ_Pluse_OffMotorPush(5);
        break;
        
      case FB_Motor_6_Pin:
        IRQ_Pluse_OffMotorPush(6);
        break;
        
      case FB_Motor_7_Pin:
        IRQ_Pluse_OffMotorPush(7);
        break;
        
      case FB_Motor_8_Pin:
        IRQ_Pluse_OffMotorPush(8);
        break;
        
      case FB_Motor_9_Pin:
        IRQ_Pluse_OffMotorPush(9);
        break;
        
      case FB_Motor_10_Pin:
        IRQ_Pluse_OffMotorPush(10);
        break;
        
      case FB_Top_Elevator_Pin:
        if(sElevator.State != ELEVATOR_STOP)
            OFF_Elevator();
        
        sElevator.Floor = 5;
        break;
        
      case FB_Bot_Elevator_Pin:
        if(sElevator.State != ELEVATOR_STOP)
            OFF_Elevator();

        sElevator.Floor = 0;
        break;
        
      case FB_Pos_Elevator_Pin:
        if(sElevator.State == ELEVATOR_UP)
        {
            sElevator.Floor++;
        }
        else if(sElevator.State == ELEVATOR_DOWN)
        {
            if(sElevator.Floor > 0)
              sElevator.Floor--;
        }
        
        if(sElevator.Floor == sParamDelivery.FloorHandle)
        {
            if(sParamDelivery.StateHanlde == DELIVERY_PURCHASE)
                OFF_Elevator();
        }
        
        Debug_Floor_Elevator(sElevator.Floor);
        break;
        
      case Encoder_C2_Pin:
        sParamDelivery.Encoder_C2++;
        break;
        
      case Encoder_C1_Pin:
        sParamDelivery.Encoder_C1++;
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

