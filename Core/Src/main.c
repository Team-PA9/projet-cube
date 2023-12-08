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
#include "rtc.h"
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
uint32_t rainInterrupt = 0;
uint32_t *rainInterruptPtr = &rainInterrupt;
extern int index_HT;
extern int index_Pr;
extern int index_WS;
extern int index_WD;
extern int index_Rf;

//SCREEN Variables
volatile uint8_t SCREEN_InactivityCpt = 0;
volatile uint8_t SCREEN_State = 0;
volatile uint8_t Flag_UsBtn2 = 0;
volatile uint8_t Flag_TS = 0;
TS_StateTypeDef TS_State;
uint16_t TS_x, TS_y, TS_past_x, TS_past_y;
extern uint8_t currentScreen;

//SDCARD Variables
uint8_t Flag_SaveHT = 0;
uint8_t Flag_SavePr = 0;
uint8_t Flag_SaveWS = 0;
uint8_t Flag_SaveWD = 0;
uint8_t Flag_SaveRf = 0;

// RTC Variables
RTC_DateTypeDef sdatestructure;
RTC_TimeTypeDef stimestructure;
RTC_DateTypeDef gdatestructureget;
RTC_TimeTypeDef gtimestructureget;

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
	MX_RTC_Init();
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
	index_HT = 0;
	index_Pr = 0;
	index_WS = 0;
	index_WD = 0;
	index_Rf = 0;
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
	TS_x = TS_State.touchX[0];
	TS_y = TS_State.touchY[0];
	printf("Done. \r\n");

	//SDCARD Initialization
	printf("\n - SDCard \r\n");
	// SDCARD_Init(); //Comment to test without SDCard insert.
	printf("Done. \r\n");

	printf("\n Initialization completed. \r\n");

	// RTC Initialization
	/*##-1- Configure the Date #################################################*/
	/* Set Date: Monday January 1st 2023 */
	sdatestructure.Year = 0x17;
	sdatestructure.Month = RTC_MONTH_JANUARY;
	sdatestructure.Date = 0x01;
	sdatestructure.WeekDay = RTC_WEEKDAY_MONDAY;

	if (HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BCD) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}

	/*##-2- Configure the Time #################################################*/
	/* Set Time: 00:00:00 */
	stimestructure.Hours = 0x00;
	stimestructure.Minutes = 0x00;
	stimestructure.Seconds = 0x00;

	if (HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BCD) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}
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
				SCREEN_InactivityCpt = 0;
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
			SENSOR_hts221_Add_Data();

			// --- STEP N°32 & N°33 : Screen Refresh
			if (currentScreen == 1 || currentScreen == 2) {
				Display_LCD_Pages(currentScreen);
			}
			// --- STEP N°39 : Reset Flag DataTHRdy
			Flag_DataTHRdy = 0;
		}

		// --- STEP N°40 & N°41 : Flag DataTHRdy -------------------------------
		else if (Flag_DataPressRdy == 1) {
			printf("Pressure sensor OK\r\n");
			SENSOR_lps22hh_Read_Data();
			SENSOR_lps22hh_Add_Data();

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
			SENSOR_WDir_Add_Data();

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
			SENSOR_Rain_Read_Data(rainInterruptPtr);
			SENSOR_Rain_Add_Data();

			// --- STEP N°62 : Screen Refresh
			if (currentScreen == 6) {
				Display_LCD_Pages(currentScreen);
			}
			// --- STEP N°69 : Reset Flag Rainfall
			Flag_Rainfall = 0;
		}

		// --- STEP N°70 : Flag Measure ----------------------------------------
		if (Flag_Measure == 1) {

			// For exemple, to remove when implemented where it should be
			/* Get the RTC current Time */
			HAL_RTC_GetTime(&hrtc, &gtimestructureget, RTC_FORMAT_BCD);
			/* Get the RTC current Date */
			HAL_RTC_GetDate(&hrtc, &gdatestructureget, RTC_FORMAT_BCD);

			/* Display time Format : hh:mm:ss */
			printf("Time: %02d:%02d:%02d\r\n", gtimestructureget.Hours,
					gtimestructureget.Minutes, gtimestructureget.Seconds);
			/* Display date Format : mm-dd-yy */
			printf("Date: %02d-%02d-%02d\r\n", gdatestructureget.Month,
					gdatestructureget.Date, 2000 + gdatestructureget.Year);

			switch (MesCpt) {
			case 0:
				// --- STEP N°71 : Humidity & Temperature
				SENSOR_hts221_Start_Conversion();
				MesCpt = 1;
				break;
			case 1:
				// --- STEP N°72 : Pressure
				SENSOR_lps22hh_Start_Conversion();
				MesCpt = 2;
				break;
			case 2:
				// --- STEP N°73 & N°74 : Wind Speed
				SENSOR_WindSpeed_Read_Data();
				SENSOR_WindSpeed_Add_Data();

				// --- STEP N°75 : Screen Refresh
				if (currentScreen == 4) {
					Display_LCD_Pages(currentScreen);
				}
				MesCpt = 3;
				break;
			case 3:
				// --- STEP N°76 : Wind Direction
				SENSOR_WDir_Start_Conversion();
				MesCpt = 0;
				break;
			}
			// --- STEP N°79 : Reset Flag Measure
			Flag_Measure = 0;
		}

		// --- STEP N°80 to N°89 : Save HT in SDCard ---------------------------
		else if (index_HT == 99) {
			Flag_SaveHT = 1;
			SDCARD_Actualization();
		}
		// --- STEP N°90 to N°99 : Save Pr in SDCard ---------------------------
		else if (index_Pr == 99) {
			Flag_SavePr = 1;
			SDCARD_Actualization();
		}
		// --- STEP N°100 to N°109 : Save WS in SDCard ---------------------------
		else if (index_WS == 99) {
			Flag_SaveWS = 1;
			SDCARD_Actualization();
		}
		// --- STEP N°110 to N°119 : Save WD in SDCard ---------------------------
		else if (index_WD == 99) {
			Flag_SaveWD = 1;
			SDCARD_Actualization();
		}
		// --- STEP N°120 to N°129 : Save Rf in SDCard ---------------------------
		else if (index_Rf == 99) {
			Flag_SaveRf = 1;
			SDCARD_Actualization();
		}

		// --- STEP N°130 : Flag Inactivity ------------------------------------
		else if (SCREEN_InactivityCpt >= 31) {
			if (SCREEN_State == 1) {
				printf("Screen OFF \r\n");
				Display_LCD_OFF();
				SCREEN_State = 0;
			}
			// --- STEP N°139 : Reset Flag Inactivity
			SCREEN_InactivityCpt = 0;
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

	/** Configure LSE Drive Capability
	 */
	HAL_PWR_EnableBkUpAccess();

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI
			| RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
		SCREEN_InactivityCpt++;
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
		rainInterrupt = HAL_GetTick();
	}
	if (GPIO_Pin == BTN2_Pin) {
		SCREEN_InactivityCpt = 0;
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
