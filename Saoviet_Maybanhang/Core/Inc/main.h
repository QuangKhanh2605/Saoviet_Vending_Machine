/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ON_OFF_Fridge_Pin GPIO_PIN_2
#define ON_OFF_Fridge_GPIO_Port GPIOE
#define ON_OFF_IR_Pin GPIO_PIN_3
#define ON_OFF_IR_GPIO_Port GPIOE
#define IR_Sensor_Irq_Pin GPIO_PIN_4
#define IR_Sensor_Irq_GPIO_Port GPIOE
#define IR_Sensor_Irq_EXTI_IRQn EXTI4_IRQn
#define IR_Sensor_OUT_Pin GPIO_PIN_5
#define IR_Sensor_OUT_GPIO_Port GPIOE
#define Motor_Pin GPIO_PIN_6
#define Motor_GPIO_Port GPIOE
#define Toggle_Reset_Pin GPIO_PIN_0
#define Toggle_Reset_GPIO_Port GPIOC
#define Vib_Sensor_Irq_3_Pin GPIO_PIN_1
#define Vib_Sensor_Irq_3_GPIO_Port GPIOC
#define Vib_Sensor_Irq_3_EXTI_IRQn EXTI1_IRQn
#define NET485IO_Pin GPIO_PIN_6
#define NET485IO_GPIO_Port GPIOA
#define Signal_TEM_Pin GPIO_PIN_0
#define Signal_TEM_GPIO_Port GPIOB
#define ADC_Vcc_in_Pin GPIO_PIN_1
#define ADC_Vcc_in_GPIO_Port GPIOB
#define Led_3_Pin GPIO_PIN_7
#define Led_3_GPIO_Port GPIOE
#define Led_2_Pin GPIO_PIN_8
#define Led_2_GPIO_Port GPIOE
#define Led_1_Pin GPIO_PIN_9
#define Led_1_GPIO_Port GPIOE
#define Vib_Sensor_Irq_1_Pin GPIO_PIN_12
#define Vib_Sensor_Irq_1_GPIO_Port GPIOE
#define Vib_Sensor_Irq_1_EXTI_IRQn EXTI15_10_IRQn
#define Door_Sensor_2_Pin GPIO_PIN_13
#define Door_Sensor_2_GPIO_Port GPIOE
#define Door_Sensor_1_Pin GPIO_PIN_14
#define Door_Sensor_1_GPIO_Port GPIOE
#define HOLD_RESET_Pin GPIO_PIN_8
#define HOLD_RESET_GPIO_Port GPIOD
#define CS_Pin GPIO_PIN_9
#define CS_GPIO_Port GPIOD
#define RESET_USB_Pin GPIO_PIN_8
#define RESET_USB_GPIO_Port GPIOC
#define Vib_Sensor_Irq_2_Pin GPIO_PIN_9
#define Vib_Sensor_Irq_2_GPIO_Port GPIOC
#define Vib_Sensor_Irq_2_EXTI_IRQn EXTI9_5_IRQn
#define PGOOD_Pin GPIO_PIN_10
#define PGOOD_GPIO_Port GPIOA
#define Count_Pin GPIO_PIN_11
#define Count_GPIO_Port GPIOA
#define Count_EXTI_IRQn EXTI15_10_IRQn
#define Layer_7_Pin GPIO_PIN_15
#define Layer_7_GPIO_Port GPIOA
#define Layer_6_Pin GPIO_PIN_10
#define Layer_6_GPIO_Port GPIOC
#define Layer_5_Pin GPIO_PIN_11
#define Layer_5_GPIO_Port GPIOC
#define Layer_4_Pin GPIO_PIN_12
#define Layer_4_GPIO_Port GPIOC
#define Layer_3_Pin GPIO_PIN_0
#define Layer_3_GPIO_Port GPIOD
#define Layer_2_Pin GPIO_PIN_1
#define Layer_2_GPIO_Port GPIOD
#define Layer_1_Pin GPIO_PIN_2
#define Layer_1_GPIO_Port GPIOD
#define Slot_1_Pin GPIO_PIN_3
#define Slot_1_GPIO_Port GPIOD
#define Slot_2_Pin GPIO_PIN_4
#define Slot_2_GPIO_Port GPIOD
#define Slot_3_Pin GPIO_PIN_5
#define Slot_3_GPIO_Port GPIOD
#define Slot_4_Pin GPIO_PIN_6
#define Slot_4_GPIO_Port GPIOD
#define Slot_5_Pin GPIO_PIN_7
#define Slot_5_GPIO_Port GPIOD
#define Slot_6_Pin GPIO_PIN_3
#define Slot_6_GPIO_Port GPIOB
#define Slot_7_Pin GPIO_PIN_4
#define Slot_7_GPIO_Port GPIOB
#define Slot_8_Pin GPIO_PIN_5
#define Slot_8_GPIO_Port GPIOB
#define Slot_9_Pin GPIO_PIN_6
#define Slot_9_GPIO_Port GPIOB
#define Slot_10_Pin GPIO_PIN_7
#define Slot_10_GPIO_Port GPIOB
#define ON_OFF_Alarm_Pin GPIO_PIN_8
#define ON_OFF_Alarm_GPIO_Port GPIOB
#define ON_OFF_Relay_5_Pin GPIO_PIN_9
#define ON_OFF_Relay_5_GPIO_Port GPIOB
#define ON_OFF_PC_Pin GPIO_PIN_0
#define ON_OFF_PC_GPIO_Port GPIOE
#define ON_OFF_Screen_Pin GPIO_PIN_1
#define ON_OFF_Screen_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
