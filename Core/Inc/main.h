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
#include "stm32f7xx_hal.h"

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
#define ETR_RAIN_Pin GPIO_PIN_15
#define ETR_RAIN_GPIO_Port GPIOA
#define ETR_WIND_Pin GPIO_PIN_3
#define ETR_WIND_GPIO_Port GPIOI
#define IRQ_PRESS_Pin GPIO_PIN_2
#define IRQ_PRESS_GPIO_Port GPIOI
#define IRQ_PRESS_EXTI_IRQn EXTI2_IRQn
#define uSD_Detect_Pin GPIO_PIN_13
#define uSD_Detect_GPIO_Port GPIOC
#define IRQ_TS_Pin GPIO_PIN_13
#define IRQ_TS_GPIO_Port GPIOI
#define IRQ_TS_EXTI_IRQn EXTI15_10_IRQn
#define IRQ_TEMP_Pin GPIO_PIN_6
#define IRQ_TEMP_GPIO_Port GPIOG
#define IRQ_TEMP_EXTI_IRQn EXTI9_5_IRQn
#define BTN2_Pin GPIO_PIN_9
#define BTN2_GPIO_Port GPIOF
#define BTN2_EXTI_IRQn EXTI9_5_IRQn
#define BTN1_Pin GPIO_PIN_8
#define BTN1_GPIO_Port GPIOF
#define BTN1_EXTI_IRQn EXTI9_5_IRQn
#define WIND_SPEED_Pin GPIO_PIN_0
#define WIND_SPEED_GPIO_Port GPIOA
#define BLED_Pin GPIO_PIN_9
#define BLED_GPIO_Port GPIOH
#define RLED_Pin GPIO_PIN_11
#define RLED_GPIO_Port GPIOH
#define GLED_Pin GPIO_PIN_10
#define GLED_GPIO_Port GPIOH

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
