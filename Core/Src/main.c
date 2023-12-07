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

//SENSORS Variables
volatile uint8_t Flag_Measure = 0;
volatile uint8_t Flag_DataTHRdy = 0;
volatile uint8_t Flag_DataPressRdy = 0;
volatile uint8_t Flag_Rainfall = 0;
volatile uint8_t Flag_DataWDirRdy = 0;
uint8_t MesCpt = 0;

//SCREEN Variables
volatile uint8_t cpt_inactivity = 0;
volatile uint8_t SCREEN_State = 0;
volatile uint8_t Flag_UsBtn2 = 0;
volatile uint8_t Flag_TS = 0;
TS_StateTypeDef TS_State;
uint16_t TS_x, TS_y, TS_past_x, TS_past_y;
extern uint8_t currentScreen;

//SDCARD Variables
uint8_t save_temp_rdy = 0;
uint8_t save_pres_rdy = 0;
uint8_t save_wind_rdy = 0;
uint8_t save_dir_rdy = 0;
uint8_t save_rain_rdy = 0;

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
	MX_TIM6_Init();
	/* USER CODE BEGIN 2 */
	// --- STEP N°0 : Initialization -------------------------------------------
	printf("Initialization started : \r\n");

	//SENSORS Initialization
	printf("\n - Sensors \r\n");
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_TIM_Base_Start_IT(&htim7);
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
	TS_x = TS_State.touchX[0];
	TS_y = TS_State.touchY[0];
	printf("Done. \r\n");

	//SDCARD Initialization
	printf("\n - SDCard \r\n");
	// SDCARD_Init(); //Comment to test without SDCard insert.
	printf("Done. \r\n");

	printf("\n Initialization completed. \r\n");

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	// --- STEP N°1 : Waiting for any Flag -------------------------------------
	while (1) {

		// --- STEP N°10 : Flag TS ---------------------------------------------
		if (Flag_TS == 1) {
			TS_past_x = TS_x;
			TS_past_y = TS_y;
			BSP_TS_GetState(&TS_State);
			TS_x = TS_State.touchX[0];
			TS_y = TS_State.touchY[0];

			// --- STEP N°12 : Compare TS with previous TS data
			if (TS_x != TS_past_x || TS_y != TS_past_y) {
				printf("\n - TouchScreen Triggered \r\n");

				// --- STEP N°11 : Power ON Screen
				if (SCREEN_State == 0) {
					printf("Screen ON \r\n");
					Display_LCD_ON();
					SCREEN_State = 1;
				}

				// --- STEP N°13 : Refresh Screen
				else if (SCREEN_State == 1) {
					TS_Actualization();
				}
				cpt_inactivity = 0;
			}
			// --- STEP N°19 : Reset Flag TS
			Flag_TS = 0;
		}

		// --- STEP N°20 : Flag UserButton 2 -----------------------------------
		else if (Flag_UsBtn2 == 1) {
			printf("\n - Button Triggered \r\n");

			// --- STEP N°21 : Power ON Screen
			if (SCREEN_State == 0) {
				printf("Screen ON \r\n");
				Display_LCD_ON();
				SCREEN_State = 1;
			}
			// --- STEP N°29 : Reset Flag UserButton2
			Flag_UsBtn2 = 0;
		}

		// --- STEP N°30 & N°31 : Flag DataTHRdy -------------------------------
		else if (Flag_DataTHRdy == 1) {
			printf("Temperature & Humidity sensor OK \r\n");
			SENSOR_hts221_Read_Data();
			save_temp_rdy = 1;

			// --- STEP N°32 & N°33 : Screen Refresh
			if (currentScreen == 1 || currentScreen == 2) {
				printf("Display Page, %d", currentScreen);
				Display_LCD_Pages(currentScreen);
			}
			// --- STEP N°39 : Reset Flag DataTHRdy
			Flag_DataTHRdy = 0;
		}

		// --- STEP N°40 & N°41 : Flag DataTHRdy -------------------------------
		else if (Flag_DataPressRdy == 1) {
			printf("Pressure sensor OK\r\n");
			SENSOR_lps22hh_Read_Data();
			save_pres_rdy = 1;

			// --- STEP N°42 : Screen Refresh
			if (currentScreen == 3) {
				Display_LCD_Pages(currentScreen);
			}
			// --- STEP N°49 : Reset Flag DataPressRdy
			Flag_DataPressRdy = 0;
		}

		// --- STEP N°50 & N°51 : Flag DataWDirRdy -----------------------------
		else if (Flag_DataWDirRdy == 1) {
			printf("Wind Direction sensor OK\r\n");
			SENSOR_WDir_Read_Data();
			save_dir_rdy = 1;

			// --- STEP N°52 : Screen Refresh
			if (currentScreen == 5) {
				Display_LCD_Pages(currentScreen);
			}
			// --- STEP N°59 : Reset Flag DataWDirRdy
			Flag_DataWDirRdy = 0;
		}

		// --- STEP N°60 & N°61 : Flag Rainfall --------------------------------
		else if (Flag_Rainfall == 1) {
			printf("Rainfall sensor OK\r\n");
			SENSOR_Rain_Read_Data();
			save_rain_rdy = 1;

			// --- STEP N°62 : Screen Refresh
			if (currentScreen == 6) {
				Display_LCD_Pages(currentScreen);
			}
			// --- STEP N°69 : Reset Flag Rainfall
			Flag_Rainfall = 0;
		}

		// --- STEP N°70 : Flag Measure ----------------------------------------
		if (Flag_Measure == 1) {
			switch (MesCpt) {
			case 0:
				// --- STEP N°71 : Humidity & Temperature
				SENSORS_Start_hts221_Conversion();
				MesCpt = 1;
				break;
			case 1:
				// --- STEP N°72 : Pressure
				SENSORS_Start_lps22hh_Conversion();
				MesCpt = 2;
				break;
			case 2:
				// --- STEP N°73 & N°74 : Wind Speed
				SENSOR_WindSpeed_Read_Data();
				save_wind_rdy = 1;

				// --- STEP N°75 : Screen Refresh
				if (currentScreen == 6) {
					Display_LCD_Pages(currentScreen);
				}
				MesCpt = 3;
				break;
			case 3:
				// --- STEP N°76 : Wind Direction
				SENSORS_Start_WDir_Conversion();
				MesCpt = 0;
				break;
			}
			// --- STEP N°79 : Reset Flag Measure
			Flag_Measure = 0;
		}

		// --- STEP N°80 to 129 : Save in SDCard (Still to do) -----------------
		// SDCARD_Actualization(); //Comment to test without SDCard insert.

		// --- STEP N°130 : Flag Inactivity ------------------------------------
		else if (cpt_inactivity >= 31) {
			if (SCREEN_State == 1) {
				printf("Screen OFF \r\n");
				Display_LCD_OFF();
				SCREEN_State = 0;
			}
			// --- STEP N°139 : Reset Flag Inactivity
			cpt_inactivity = 0;
		}

		// --- STEP N°2 : µP Sleep ---------------------------------------------
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
	if (htim == &htim7) {
		printf("\n - Timer 7 Triggered \r\n");
		Flag_Measure = 1;
		cpt_inactivity++;
		printf("\n CPT : %d \r\n", cpt_inactivity);
	}
	if (htim == &htim6) {
		Flag_TS = 1;
	}
}

//GPIO EXTI Interruption Callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	HAL_ResumeTick();
	if (GPIO_Pin == IRQ_TEMP_Pin) {
		Flag_DataTHRdy = 1;
	}
	if (GPIO_Pin == IRQ_PRESS_Pin) {
		Flag_DataPressRdy = 1;
	}
	if (GPIO_Pin == IRQ_RAIN_Pin) {
		Flag_Rainfall = 1;
	}
	if (GPIO_Pin == BTN2_Pin) {
		cpt_inactivity = 0;
		Flag_UsBtn2 = 1;
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	Flag_DataWDirRdy = 1;
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
