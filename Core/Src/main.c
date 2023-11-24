/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "dma2d.h"
#include "fatfs.h"
#include "i2c.h"
#include "ltdc.h"
#include "sdmmc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "hts221_reg.h"
#include "lps22hh_reg.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_ts.h"
#include "user_def_screen.h"
#include "user_def_sdcard.h"
#include "user_def_sensors.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//UART PRINTF Define
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE * f)
#endif /* __GNUC__ */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//GEN Variables
volatile uint8_t IRQs = 0;

//SENSORS Variables
volatile uint8_t cpt_measures = 0;
volatile uint8_t start_measures = 0;
volatile uint8_t tmp_sns_d_rdy = 0;
volatile uint8_t prs_sns_d_rdy = 0;
volatile uint8_t retrieve_wind_speed = 0;
volatile uint8_t retrieve_rainfall = 0;
volatile uint8_t retrieve_wind_dir = 0;

//SCREEN Variables
volatile uint8_t cpt_inactivity = 0;
volatile uint8_t SCREEN_State = 0;
volatile uint8_t IRQ_BTN2 = 0;
volatile uint8_t IRQ_TS = 0;
TS_StateTypeDef TS_State;
uint16_t x, y;

//SDCARD Variables
uint8_t save_temp_rdy = 0;
uint8_t save_pres_rdy = 0;
uint8_t save_wind_rdy = 0;
uint8_t save_dir_rdy = 0;
uint8_t save_rain_rdy = 0;

extern int currentScreen;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_TIM2_Init();
	MX_TIM7_Init();
	MX_TIM8_Init();
	MX_ADC1_Init();
	MX_I2C1_Init();
	MX_USART1_UART_Init();
	MX_SDMMC1_SD_Init();
	MX_DMA2D_Init();
	MX_FMC_Init();
	MX_LTDC_Init();
	MX_FATFS_Init();
	/* USER CODE BEGIN 2 */
	//STEP 0 : Initialization
	printf("Initialization started : \r\n");

	//GEN PROJECT TIMER Initialization
	printf("\n - General Project Timer \r\n");
	HAL_TIM_Base_Start_IT(&htim7);
	printf("Done. \r\n");

	//SENSORS Initialization
	printf("\n - Sensors \r\n");
	HAL_TIM_Base_Start(&htim8);
	HAL_TIM_Base_Start(&htim2);
	SENSOR_lps22hh_Init();
	SENSOR_hts221_Init();
	printf("Done. \r\n");

	//SCREEN Initialization
	printf("\n - Screen \r\n");
	BSP_LCD_Init();
	Display_LCD_Init();
	Display_LCD_Pages(0);
	HAL_GPIO_WritePin(GLED_GPIO_Port, GLED_Pin, 1);
	HAL_GPIO_WritePin(RLED_GPIO_Port, RLED_Pin, 0);
	HAL_GPIO_WritePin(BLED_GPIO_Port, BLED_Pin, 0);
	SCREEN_State = 1;
	printf("Done. \r\n");

	printf("\n - Touch Screen \r\n");
	BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
	BSP_TS_ResetTouchData(&TS_State);
	BSP_TS_ITConfig();
	BSP_TS_ITClear();
	x = TS_State.touchX[0];
	y = TS_State.touchY[0];
	printf("Done. \r\n");

	//SDCARD Initialization
	printf("\n - SDCard \r\n");
	SDCARD_Init(); //Comment to test without SDCard insert.
	printf("Done. \r\n");

	printf("\n Initialization completed. \r\n");

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		//STEP 1 : Waiting for IRQs

		if (IRQs == 1) {
			//STEP 2 : IRQ Measures
			if (cpt_measures % 2 == 0) {
				if (start_measures == 1) {
					printf("\n - Measures conversions started\r\n");
					SENSORS_Start_Conversion();
					start_measures = 0;
				}
				//Get & Print a measure from every sensors
				if (tmp_sns_d_rdy == 1) {
					printf("Temperature & Humidity sensor OK\r\n");
					SENSOR_hts221_Read_Data();
					save_temp_rdy = 1;
					tmp_sns_d_rdy = 0;
					Display_LCD_Pages(currentScreen);
				}
				if (prs_sns_d_rdy == 1) {
					printf("Pressure sensor OK\r\n");
					SENSOR_lps22hh_Read_Data();
					save_pres_rdy = 1;
					prs_sns_d_rdy = 0;
					Display_LCD_Pages(currentScreen);
				}
				if (retrieve_wind_speed == 1) {
					SENSOR_WindSpeed_Read_Data();
					save_wind_rdy = 1;
					retrieve_wind_speed = 0;
					Display_LCD_Pages(currentScreen);
				}
				if (retrieve_wind_dir == 1) {
					SENSOR_WindDir_Read_Data();
					save_dir_rdy = 1;
					retrieve_wind_dir = 0;
					Display_LCD_Pages(currentScreen);
				}
				if (retrieve_rainfall == 1) {
					SENSOR_Rain_Read_Data();
					cpt_measures = 0;
					save_rain_rdy = 1;
					retrieve_rainfall = 0;
					Display_LCD_Pages(currentScreen);
				}

				//Screen Management & Actualization
				/* Need to separate SCREEN_Actualization into SwCa of Pages and
				 * Measures actualizations on the selected screen.
				 * -- TO DO INSIDE .h.c of Screen --
				 */
				SCREEN_Actualization();

				// Add every measure from a sensor to the correspondent array.
				// Save every last measure from a sensor in SDCard.
				SDCARD_Actualization(); //Comment to test without SDCard insert.
			}

			//STEP 3 : IRQ Screen triggered
			if (SCREEN_State == 1) {
				if (IRQ_TS == 1) {
					printf("\n - TouchScreen Triggered \r\n");
					SCREEN_Actualization();
					IRQ_TS = 0;
				} else if (cpt_inactivity >= 3) {
					printf("Screen OFF \r\n");
					BSP_LCD_DisplayOff();
					HAL_GPIO_WritePin(GLED_GPIO_Port, GLED_Pin, 0);
					HAL_GPIO_WritePin(RLED_GPIO_Port, RLED_Pin, 1);
					cpt_inactivity = 0;
					SCREEN_State = 0;
				}
			} else if (SCREEN_State == 0) {
				cpt_inactivity = 0;
				if ((IRQ_BTN2 == 1) || (IRQ_TS == 1)) {
					if (IRQ_BTN2 == 1) {
						printf("\n - Button Triggered \r\n");
					} else {
						printf("\n - TouchScreen Triggered \r\n");
					}
					printf("Screen ON \r\n");
					BSP_LCD_DisplayOn();
					HAL_GPIO_WritePin(GLED_GPIO_Port, GLED_Pin, 1);
					HAL_GPIO_WritePin(RLED_GPIO_Port, RLED_Pin, 0);
					SCREEN_Actualization();
					IRQ_BTN2 = 0;
					IRQ_TS = 0;
					SCREEN_State = 1;
				}
			}

			IRQs = 0;
		}

		//STEP 10 : µP Sleep
		else {
			HAL_SuspendTick();
			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		}
	}
	/* USER CODE END WHILE */

	/* USER CODE BEGIN 3 */

	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 200;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 8;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Activate the Over-Drive mode
	 */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
//UART PRINTF Declaration
PUTCHAR_PROTOTYPE {
	HAL_UART_Transmit(&huart1, (uint8_t*) &ch, 1, 100);
	return ch;
}

//HAL TIM PeriodElapsed Callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	HAL_ResumeTick();
	IRQs = 1;
	if (htim == &htim7) {
		printf("\n - Timer 7 Triggered \r\n");
		cpt_measures++;
		if (cpt_measures % 2 == 0) {
			start_measures = 1;
			retrieve_wind_speed = 1;
			if (cpt_measures == 10) {
				retrieve_rainfall = 1;
			}
		}

		cpt_inactivity++;

		printf("Measure CPT : %d \r\n", cpt_measures);
		printf("Inactivity CPT : %d \r\n", cpt_inactivity);
	}
}

//GPIO EXTI Interruption Callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	HAL_ResumeTick();
	IRQs = 1;
	if (GPIO_Pin == IRQ_TEMP_Pin) {
		tmp_sns_d_rdy = 1;
	}
	if (GPIO_Pin == IRQ_PRESS_Pin) {
		prs_sns_d_rdy = 1;
	}
	if (GPIO_Pin == IRQ_TS_Pin) {
		cpt_inactivity = 0;
		BSP_TS_GetState(&TS_State);
		x = TS_State.touchX[0];
		y = TS_State.touchY[0];
		IRQ_TS = 1;
	}
	if (GPIO_Pin == BTN2_Pin) {
		IRQ_BTN2 = 1;
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	retrieve_wind_dir = 1;
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
