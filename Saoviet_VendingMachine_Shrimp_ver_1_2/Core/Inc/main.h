/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#define CTRL_Motor_9_Pin GPIO_PIN_2
#define CTRL_Motor_9_GPIO_Port GPIOE
#define CTRL_Motor_10_Pin GPIO_PIN_3
#define CTRL_Motor_10_GPIO_Port GPIOE
#define Led_3_Pin GPIO_PIN_4
#define Led_3_GPIO_Port GPIOE
#define Led_2_Pin GPIO_PIN_5
#define Led_2_GPIO_Port GPIOE
#define Led_1_Pin GPIO_PIN_6
#define Led_1_GPIO_Port GPIOE
#define Input_1_Pin GPIO_PIN_2
#define Input_1_GPIO_Port GPIOC
#define Input_2_Pin GPIO_PIN_3
#define Input_2_GPIO_Port GPIOC
#define Toggle_Reset_Pin GPIO_PIN_0
#define Toggle_Reset_GPIO_Port GPIOA
#define RELAY_5_Pin GPIO_PIN_1
#define RELAY_5_GPIO_Port GPIOA
#define TX_PCBOX_Pin GPIO_PIN_2
#define TX_PCBOX_GPIO_Port GPIOA
#define RX_PCBOX_Pin GPIO_PIN_3
#define RX_PCBOX_GPIO_Port GPIOA
#define RELAY_4_Pin GPIO_PIN_4
#define RELAY_4_GPIO_Port GPIOA
#define RELAY_3_Pin GPIO_PIN_5
#define RELAY_3_GPIO_Port GPIOA
#define RELAY_2_Pin GPIO_PIN_6
#define RELAY_2_GPIO_Port GPIOA
#define RELAY_1_Pin GPIO_PIN_7
#define RELAY_1_GPIO_Port GPIOA
#define RESET_USB_Pin GPIO_PIN_4
#define RESET_USB_GPIO_Port GPIOC
#define Vib_Sensor_IN_Pin GPIO_PIN_7
#define Vib_Sensor_IN_GPIO_Port GPIOE
#define Vib_Sensor_IN_EXTI_IRQn EXTI9_5_IRQn
#define Door_Sensor_1_Pin GPIO_PIN_9
#define Door_Sensor_1_GPIO_Port GPIOE
#define Door_Sensor_2_Pin GPIO_PIN_10
#define Door_Sensor_2_GPIO_Port GPIOE
#define ON_OFF_V2_Pin GPIO_PIN_11
#define ON_OFF_V2_GPIO_Port GPIOE
#define ON_OFF_V1_Pin GPIO_PIN_12
#define ON_OFF_V1_GPIO_Port GPIOE
#define ON_OFF_LED_Pin GPIO_PIN_13
#define ON_OFF_LED_GPIO_Port GPIOE
#define FB_LOCK_Pin GPIO_PIN_14
#define FB_LOCK_GPIO_Port GPIOE
#define PW_LOCK_Pin GPIO_PIN_15
#define PW_LOCK_GPIO_Port GPIOE
#define TX_DEBUG_Pin GPIO_PIN_10
#define TX_DEBUG_GPIO_Port GPIOB
#define RX_DEBUG_Pin GPIO_PIN_11
#define RX_DEBUG_GPIO_Port GPIOB
#define FLASH_RESET_Pin GPIO_PIN_12
#define FLASH_RESET_GPIO_Port GPIOB
#define FLASH_CS_Pin GPIO_PIN_8
#define FLASH_CS_GPIO_Port GPIOD
#define ON_OFF_Pos_Elevator_Pin GPIO_PIN_9
#define ON_OFF_Pos_Elevator_GPIO_Port GPIOD
#define IN_B_Pin GPIO_PIN_10
#define IN_B_GPIO_Port GPIOD
#define IN_A_Pin GPIO_PIN_11
#define IN_A_GPIO_Port GPIOD
#define LOCK_Pin GPIO_PIN_12
#define LOCK_GPIO_Port GPIOD
#define FB_Bot_Elevator_Pin GPIO_PIN_13
#define FB_Bot_Elevator_GPIO_Port GPIOD
#define FB_Bot_Elevator_EXTI_IRQn EXTI15_10_IRQn
#define FB_Top_Elevator_Pin GPIO_PIN_14
#define FB_Top_Elevator_GPIO_Port GPIOD
#define FB_Top_Elevator_EXTI_IRQn EXTI15_10_IRQn
#define FB_Pos_Elevator_Pin GPIO_PIN_6
#define FB_Pos_Elevator_GPIO_Port GPIOC
#define FB_Pos_Elevator_EXTI_IRQn EXTI9_5_IRQn
#define SEL0_Pin GPIO_PIN_7
#define SEL0_GPIO_Port GPIOC
#define Encoder_C2_Pin GPIO_PIN_8
#define Encoder_C2_GPIO_Port GPIOC
#define Encoder_C2_EXTI_IRQn EXTI9_5_IRQn
#define Encoder_C1_Pin GPIO_PIN_9
#define Encoder_C1_GPIO_Port GPIOC
#define Encoder_C1_EXTI_IRQn EXTI9_5_IRQn
#define RS485_TXDE_Pin GPIO_PIN_8
#define RS485_TXDE_GPIO_Port GPIOA
#define TX_RS485_Pin GPIO_PIN_9
#define TX_RS485_GPIO_Port GPIOA
#define RX_RS485_Pin GPIO_PIN_10
#define RX_RS485_GPIO_Port GPIOA
#define FB_Motor_1_Pin GPIO_PIN_15
#define FB_Motor_1_GPIO_Port GPIOA
#define FB_Motor_1_EXTI_IRQn EXTI15_10_IRQn
#define FB_Motor_2_Pin GPIO_PIN_10
#define FB_Motor_2_GPIO_Port GPIOC
#define FB_Motor_2_EXTI_IRQn EXTI15_10_IRQn
#define FB_Motor_3_Pin GPIO_PIN_11
#define FB_Motor_3_GPIO_Port GPIOC
#define FB_Motor_3_EXTI_IRQn EXTI15_10_IRQn
#define FB_Motor_4_Pin GPIO_PIN_12
#define FB_Motor_4_GPIO_Port GPIOC
#define FB_Motor_4_EXTI_IRQn EXTI15_10_IRQn
#define FB_Motor_5_Pin GPIO_PIN_0
#define FB_Motor_5_GPIO_Port GPIOD
#define FB_Motor_5_EXTI_IRQn EXTI0_IRQn
#define FB_Motor_6_Pin GPIO_PIN_1
#define FB_Motor_6_GPIO_Port GPIOD
#define FB_Motor_6_EXTI_IRQn EXTI1_IRQn
#define FB_Motor_7_Pin GPIO_PIN_2
#define FB_Motor_7_GPIO_Port GPIOD
#define FB_Motor_7_EXTI_IRQn EXTI2_IRQn
#define FB_Motor_8_Pin GPIO_PIN_3
#define FB_Motor_8_GPIO_Port GPIOD
#define FB_Motor_8_EXTI_IRQn EXTI3_IRQn
#define FB_Motor_9_Pin GPIO_PIN_4
#define FB_Motor_9_GPIO_Port GPIOD
#define FB_Motor_9_EXTI_IRQn EXTI4_IRQn
#define FB_Motor_10_Pin GPIO_PIN_5
#define FB_Motor_10_GPIO_Port GPIOD
#define FB_Motor_10_EXTI_IRQn EXTI9_5_IRQn
#define ON_OFF_SW_PC_Pin GPIO_PIN_6
#define ON_OFF_SW_PC_GPIO_Port GPIOD
#define RESET_SW_PC_Pin GPIO_PIN_7
#define RESET_SW_PC_GPIO_Port GPIOD
#define GLASS_Pin GPIO_PIN_3
#define GLASS_GPIO_Port GPIOB
#define CTRL_Motor_1_Pin GPIO_PIN_4
#define CTRL_Motor_1_GPIO_Port GPIOB
#define CTRL_Motor_2_Pin GPIO_PIN_5
#define CTRL_Motor_2_GPIO_Port GPIOB
#define CTRL_Motor_3_Pin GPIO_PIN_6
#define CTRL_Motor_3_GPIO_Port GPIOB
#define CTRL_Motor_4_Pin GPIO_PIN_7
#define CTRL_Motor_4_GPIO_Port GPIOB
#define CTRL_Motor_5_Pin GPIO_PIN_8
#define CTRL_Motor_5_GPIO_Port GPIOB
#define CTRL_Motor_6_Pin GPIO_PIN_9
#define CTRL_Motor_6_GPIO_Port GPIOB
#define CTRL_Motor_7_Pin GPIO_PIN_0
#define CTRL_Motor_7_GPIO_Port GPIOE
#define CTRL_Motor_8_Pin GPIO_PIN_1
#define CTRL_Motor_8_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
