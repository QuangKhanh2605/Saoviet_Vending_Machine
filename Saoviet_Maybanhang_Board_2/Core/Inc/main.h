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
#include "stm32l0xx_hal.h"

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
#define RESET_ADE_Pin GPIO_PIN_14
#define RESET_ADE_GPIO_Port GPIOC
#define LED_Pin GPIO_PIN_15
#define LED_GPIO_Port GPIOC
#define NET485IO_Pin GPIO_PIN_4
#define NET485IO_GPIO_Port GPIOA
#define CS_MCU_2_Pin GPIO_PIN_1
#define CS_MCU_2_GPIO_Port GPIOB
#define IRQ_ADE_Pin GPIO_PIN_9
#define IRQ_ADE_GPIO_Port GPIOA
#define IRQ_ADE_EXTI_IRQn EXTI4_15_IRQn
#define REVP_MCU_Pin GPIO_PIN_10
#define REVP_MCU_GPIO_Port GPIOA
#define REVP_MCU_EXTI_IRQn EXTI4_15_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
