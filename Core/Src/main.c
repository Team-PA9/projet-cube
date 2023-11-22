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

//SENSORS Define
#define ADC_BUF_LEN 1 //Define twice, "main"+"user_def_sensors".

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//GEN Variables
volatile uint8_t cpt_tim7 = 0;

//SENSORS Variables
volatile uint8_t start_measures = 0;
volatile uint8_t tmp_sns_d_rdy = 0;
volatile uint8_t prs_sns_d_rdy = 0;
volatile uint8_t retrieve_wind_speed = 0;
volatile uint8_t retrieve_rainfall = 0;
volatile uint8_t retrieve_wind_dir = 0;
static uint16_t adc_buf[ADC_BUF_LEN]; //Define twice, "main"+"user_def_sensors".

/*STILL TO ORGANIZE ----------------------------------------------------------*/
//SCREEN Variables
uint8_t IRQ_TS = 0;
static TS_StateTypeDef TS_State;
uint16_t x, y;
uint8_t test_mes = 0;
uint8_t refresh = 0;
uint8_t mesure = 0;
char *temperature = "12345";
char *Humidité = "12345";
char *Pression = "1234567";
char *vitesse = "12345";
uint8_t direction = 6;
char *Pluvio = "12345";

//SDCARD Variables
FRESULT res, res2, res3, res4, res5, restest, res6; /* FatFs function common result code */
uint32_t byteswritten, bytesread; /* File write/read counts */
uint8_t wtext[] = "LOG Temp"; /* File write buffer */
uint8_t wtext2[] = "LOG Pres";
uint8_t wtext3[] = "LOG WDir";
uint8_t wtext4[] = "LOG WSpe";
uint8_t wtext5[] = "LOG Rain";
uint8_t wtexttest[] = "asd";
const TCHAR *nom_file = "test.txt";
uint8_t rtext[100];
uint8_t log_temperature[100];
uint8_t log_pression[100];
uint8_t log_direction[100];
uint8_t log_vitesse[100];
uint8_t log_pluviometre[100];
FIL *filetest, *file1, *file2, *file3, *file4, *file5; /* File read buffer */
uint8_t workBuffer[_MAX_SS];
volatile uint8_t Flag_IRQ_Btn = 0;
/*---------------------------------------------------------- STILL TO ORGANIZE*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/*STILL TO ORGANIZE ----------------------------------------------------------*/
//SCREEN PFP
void Display_LCD_Init();
void Display_LCD_Button(int);

//SDCARD PFP
void format(void);
void new_log(FIL *fp, const char *filename, const char *content);
void add_log(FIL *fp, const TCHAR *nom_file, const void *data);

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
	printf("Initialization started : \r\n");

	//GEN PROJECT TIMER Initialization
	printf("\n - General Project Timer \r\n");
	HAL_TIM_Base_Start_IT(&htim7);
	printf("Done. \r\n");

	//SENSORS Initialization
	printf("\n - Sensors \r\n");
	HAL_TIM_Base_Start(&htim8);
	HAL_TIM_Base_Start(&htim2);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_buf, ADC_BUF_LEN);
	SENSOR_lps22hh_Init();
	SENSOR_hts221_Init();
	printf("Done. \r\n");

/*STILL TO ORGANIZE ----------------------------------------------------------*/
	//SCREEN Initialization
	printf("\n - Screen \r\n");
	BSP_LCD_Init();
	Display_LCD_Init();
	Display_LCD_Button(0);
	HAL_GPIO_WritePin(GLED_GPIO_Port, GLED_Pin, 1);
	HAL_GPIO_WritePin(RLED_GPIO_Port, RLED_Pin, 0);
	HAL_GPIO_WritePin(BLED_GPIO_Port, BLED_Pin, 0);
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
	if (f_mount(&SDFatFS, (TCHAR const*) SDPath, 0) != FR_OK) {
		Error_Handler(); /* FatFs Initialization Error */
	}
	else {
		if (f_mkfs((TCHAR const*) SDPath, FM_ANY, 0, workBuffer,
					sizeof(workBuffer)) != FR_OK) {
			Error_Handler(); /* FatFs Format Error */
		}
		else {
			new_log(filetest, "test.txt", "test");
			new_log(file1, "LOG_Temp.txt", "Log temperature");
			new_log(file2, "LOG_Pres.txt", "Log pressure");
			new_log(file3, "LOG_WDir.txt", "Log wind direction");
			new_log(file4, "LOG_WSpe.txt", "Log wind speed");
			new_log(file5, "LOG_Rain.txt" , "Log rainfall");
		}
	}
	FATFS_UnLinkDriver(SDPath);
	printf("Done. \r\n");
/*---------------------------------------------------------- STILL TO ORGANIZE*/

	printf("\n Initialization completed. \r\n");

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		//SENSORS
		if (start_measures == 1) {
			printf("Measures conversions started\r\n");
			SENSORS_Start_Conversion();
			start_measures = 0;
		}

		if (tmp_sns_d_rdy == 1) {
			SENSOR_hts221_Read_Data();
			tmp_sns_d_rdy = 0;
		}

		if (prs_sns_d_rdy == 1) {
			SENSOR_lps22hh_Read_Data();
			prs_sns_d_rdy = 0;
		}

		if (retrieve_wind_speed == 1) {
			SENSOR_WindSpeed_Read_Data();
			retrieve_wind_speed = 0;
		}

		if (retrieve_wind_dir == 1) {
			SENSOR_WindDir_Read_Data();
			retrieve_wind_dir = 0;
		}

		if (retrieve_rainfall == 1) {
			SENSOR_Rain_Read_Data();
			retrieve_rainfall = 0;
		}

/*STILL TO ORGANIZE ----------------------------------------------------------*/
		// SCREEN
		if (IRQ_TS == 1) {
			BSP_TS_GetState(&TS_State);
			if (((80 < x) && (x < 120) && (y > 50) && (y < 90)
					&& (test_mes == 0))) {
				//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				Display_LCD_Button(1);
				test_mes = 1;
				mesure = 1;
			} else if (((80 < x) && (x < 120) && (y > 120) && (y < 160)
					&& (test_mes == 0))) {
				//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				Display_LCD_Button(2);
				test_mes = 1;
				mesure = 2;
			} else if (((80 < x) && (x < 120) && (y > 190) && (y < 230)
					&& (test_mes == 0))) {
				//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				Display_LCD_Button(3);
				test_mes = 1;
				mesure = 3;
			} else if (((280 < x) && (x < 320) && (y > 50) && (y < 90)
					&& (test_mes == 0))) {
				//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				Display_LCD_Button(4);
				test_mes = 1;
				mesure = 4;
			} else if (((280 < x) && (x < 320) && (y > 120) && (y < 160)
					&& (test_mes == 0))) {
				//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				Display_LCD_Button(5);
				test_mes = 1;
				mesure = 5;
			} else if (((280 < x) && (x < 320) && (y > 190) && (y < 230)
					&& (test_mes == 0))) {
				//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				Display_LCD_Button(6);
				test_mes = 1;
				mesure = 6;
			} else {
				if ((test_mes == 1) && (380 < x) && (x < 420) && (y > 190)
						&& (y < 230)) {
					//					  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 0);
					Display_LCD_Button(0);
					test_mes = 0;
					mesure = 0;
				} else if ((test_mes == 1) && (30 < x) && (x < 80) && (y > 190)
						&& (y < 230)) {
					//					  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 0);
					Display_LCD_Button(7);

					refresh = refresh + 1;
					if (mesure == 1) {
						//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
						Display_LCD_Button(1);
					} else if (mesure == 2) {
						//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
						Display_LCD_Button(2);
					} else if (mesure == 3) {
						//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
						Display_LCD_Button(3);
					} else if (mesure == 4) {
						//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
						Display_LCD_Button(4);
					} else if (mesure == 5) {
						//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
						Display_LCD_Button(5);
					} else if (mesure == 6) {
						//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
						Display_LCD_Button(6);
					}

					test_mes = 1;

				}
			}
			IRQ_TS = 0;
		} else { //µP Sleep
			HAL_SuspendTick();
			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		}

		//SDCARD
		//	int random[10] = {48,49,50,51,52,53,54,55,56,57};
		//	for (int i = 0; i < 10; i++)
		//	{
		//	char *value = random[i];
		//log_temperature[i] = *value;
		//}
		if (Flag_IRQ_Btn == 0) {
			HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_1);
			HAL_Delay(500);
		} else if (Flag_IRQ_Btn == 1) {
			for (char i = 48; i < 58; i++)
			//for(int i = 0; i < 10; i++)
					{
				HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_1);
				FATFS_LinkDriver(&SD_Driver, SDPath);
				if (f_mount(&SDFatFS, (TCHAR
				const*) SDPath, 0) != FR_OK) {
					/* FatFs Initialization Error */
					Error_Handler();
				} else {
					if (f_open(file1, "LOG_Temp.TXT",
							FA_OPEN_APPEND | FA_WRITE) != FR_OK) {
						/* 'STM32.TXT' file Open for write Error */
						Error_Handler();
					} else {
						/*##-5- Write data to the text file ################################*/
						f_puts(" \n", file1);
						//restest = f_write(&file1, wtexttest, sizeof(wtexttest), (void *)&byteswritten);
						//restest = f_write(&file1, log_temperature[i], sizeof(log_temperature[i]), (void *)&byteswritten);
						char value = i;
						restest = f_write(file1, &value, sizeof(&value),
								(void*) &byteswritten);
						//f_write(&file1, space, sizeof(space), (void *)&byteswritten);

						if ((byteswritten == 0) || (restest != FR_OK)) {
							/* 'STM32.TXT' file Write or EOF Error */
							Error_Handler();
						} else {
							/*##-6- Close the open text file #################################*/
							f_close(file1);

							/*##-7- Open the text file object with read access ###############*/
							//if(f_open(&file1, "STM32.TXT", FA_READ) != FR_OK)
							if (f_open(file1, "LOG_Temp.TXT", FA_READ)
									!= FR_OK) {
								/* 'STM32.TXT' file Open for read Error */
								Error_Handler();
							} else {
								/*##-8- Read data from the text file ###########################*/
								restest = f_read(file1, rtext, sizeof(rtext),
										(UINT*) &bytesread);

								if ((bytesread == 0) || (restest != FR_OK)) {
									/* 'STM32.TXT' file Read or EOF Error */
									Error_Handler();
								} else {
									/*##-9- Close the open text file #############################*/
									f_close(file1);
									Flag_IRQ_Btn = 0;
									//
									//						  /*##-10- Compare read data with the expected data ############*/
									//						  if((bytesread != byteswritten))
									//						  {
									//							  /* Read data is different from the expected data */
									//							  Error_Handler();
									//
									//						  }
									//						  else
									//						  {
									//						  }
								}
							}
						}
					}
				}
				FATFS_UnLinkDriver(SDPath);
			}
		}
/*---------------------------------------------------------- STILL TO ORGANIZE*/

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
	if (htim == &htim7) {
		printf("Timer 7 Triggered \r\n");
		cpt_tim7++;
		if (cpt_tim7%2 == 0){
			start_measures = 1;
			retrieve_wind_speed = 1;
			retrieve_wind_dir = 1;
			if (cpt_tim7 == 10){
				retrieve_rainfall = 1;
				cpt_tim7 = 0;
			}
		}
	}
}

//GPIO EXTI Interruption Callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == IRQ_TEMP_Pin) {
		printf("Temperature sensor OK\r\n");
		tmp_sns_d_rdy = 1;
	}

	if (GPIO_Pin == IRQ_PRESS_Pin) {
		printf("Pressure sensor OK\r\n");
		prs_sns_d_rdy = 1;
	}
	if (GPIO_Pin == IRQ_TS_Pin) {
		HAL_ResumeTick();
		BSP_TS_GetState(&TS_State);
		x = TS_State.touchX[0];
		y = TS_State.touchY[0];
		IRQ_TS = 1;
	}
}

/*STILL TO ORGANIZE ----------------------------------------------------------*/
//SCREEN
void Display_LCD_Init() {
	BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
	BSP_LCD_SetLayerVisible(LTDC_ACTIVE_LAYER, ENABLE);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "Menu Principal :", CENTER_MODE);
}

void Display_LCD_Button(int color) {
	if (color == 1) {

		if (refresh == 0) {
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "TEMPERATURE :",
					CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "HOME", RIGHT_MODE);
		}

		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) "[         'C ]", CENTER_MODE);

		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) &temperature, CENTER_MODE);

		refresh = 1;

	} else if (color == 2) {
		if (refresh == 0) {
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "HUMIDITE :",
					CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "HOME", RIGHT_MODE);
		}
		// Affichage de la mesure

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) "[         % ]", CENTER_MODE);

		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) &Humidité, CENTER_MODE);

		refresh = 1;

	} else if (color == 3) {
		if (refresh == 0) {
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "PRESSION :",
					CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "HOME", RIGHT_MODE);
		}

		//Affichage de la mesure

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) "[            Pa ]", CENTER_MODE);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) &Pression, CENTER_MODE);

		refresh = 1;
	} else if (color == 4) {
		if (refresh == 0) {
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "VITESSE :", CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "HOME", RIGHT_MODE);
		}

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) "[         m/s ]", CENTER_MODE);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) &vitesse, CENTER_MODE);
		refresh = 1;

	} else if (color == 5) {
		if (refresh == 0) {
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) " DIRECTION :",
					CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "HOME", RIGHT_MODE);
		}

		// Affichage de la mesure

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

		if (direction == 0) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ N ]", CENTER_MODE);
		} else if (direction == 1) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ NE ]", CENTER_MODE);
		} else if (direction == 2) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ E ]", CENTER_MODE);
		} else if (direction == 3) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ SE ]", CENTER_MODE);
		} else if (direction == 4) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ S ]", CENTER_MODE);
		} else if (direction == 5) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ SW ]", CENTER_MODE);
		} else if (direction == 6) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ W ]", CENTER_MODE);
		} else if (direction == 7) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ NW ]", CENTER_MODE);
		}
		refresh = 1;

	} else if (color == 6) {
		if (refresh == 0) {
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "PLUVIOMETRIE :",
					CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "HOME", RIGHT_MODE);
		}
		// Affichage de la mesure

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) "[         mm/H ]", CENTER_MODE);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) &Pluvio, CENTER_MODE);
		refresh = 1;

	} else if (color == 7) {
		refresh = 1;
		if (mesure == 1) {
			//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(1);
		} else if (mesure == 2) {
			//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(2);
		} else if (mesure == 3) {
			//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(3);
		} else if (mesure == 4) {
			//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(4);
		} else if (mesure == 5) {
			//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(5);
		} else if (mesure == 6) {
			//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(6);
		}
	} else {

		// Titre

		BSP_LCD_Clear(LCD_COLOR_BLACK);
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "Menu Principal :",
				CENTER_MODE);
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

		// BTN P1

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize() - (3 * BSP_LCD_GetYSize() / 4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize() - (3 * BSP_LCD_GetYSize() / 4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130,
				BSP_LCD_GetYSize() - (3 * BSP_LCD_GetYSize() / 4),
				(uint8_t*) "P1:Temp", LEFT_MODE);

		// BTN P2

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize() - (2 * BSP_LCD_GetYSize() / 4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize() - (2 * BSP_LCD_GetYSize() / 4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130,
				BSP_LCD_GetYSize() - (2 * BSP_LCD_GetYSize() / 4),
				(uint8_t*) "P2:Hum ", LEFT_MODE);

		// BTN P3

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
				(uint8_t*) "P3:Pres", LEFT_MODE);

		// BTN P4

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize() - (3 * BSP_LCD_GetYSize() / 4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize() - (3 * BSP_LCD_GetYSize() / 4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330,
				BSP_LCD_GetYSize() - (3 * BSP_LCD_GetYSize() / 4),
				(uint8_t*) "P4:Vit", LEFT_MODE);

		// BTN P5

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize() - (2 * BSP_LCD_GetYSize() / 4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize() - (2 * BSP_LCD_GetYSize() / 4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330,
				BSP_LCD_GetYSize() - (2 * BSP_LCD_GetYSize() / 4),
				(uint8_t*) "P5:Dir", LEFT_MODE);

		// BTN P6

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
				(uint8_t*) "P6:Pluv", LEFT_MODE);

		refresh = 0;
	}
}

//SDCARD
void format(void) {
	/*##-2- Register the file system object to the FatFs module ##############*/
	if (f_mount(&SDFatFS, (TCHAR
	const*) SDPath, 0) != FR_OK) {
		/* FatFs Initialization Error */
		Error_Handler();
	} else {
		/*##-3- Create a FAT file system (format) on the logical drive #########*/
		/* WARNING: Formatting the uSD card will delete all content on the device */
		if (f_mkfs((TCHAR
		const*) SDPath, FM_ANY, 0, workBuffer, sizeof(workBuffer)) != FR_OK) {
			/* FatFs Format Error */
			Error_Handler();
		}
	}
}

void new_log(FIL *fp, const char *filename, const char *content) {
	FRESULT res;
	uint32_t byteswritten; //, bytesread;
	/*##-2- Register the file system object to the FatFs module ##############*/
	//	      if(f_mount(&SDFatFS, (TCHAR const*)SDPath, 0) != FR_OK)
	//	      {
	//	        /* FatFs Initialization Error */
	//	        Error_Handler();
	//	      }
	//		  else
	//		  {
	/*##-4- Create and Open a new text file object with write access #####*/
	//if(f_open(&SDFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	if (f_open(fp, filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
		/* 'STM32.TXT' file Open for write Error */
		Error_Handler();
	} else {
		/*##-5- Write data to the text file ################################*/
		res = f_write(fp, content, strlen(content), (void*) &byteswritten);

		if ((byteswritten == 0) || (res != FR_OK)) {
			/* 'STM32.TXT' file Write or EOF Error */
			Error_Handler();
		} else {
			f_close(fp);
		}
	}

}

void add_log(FIL *fp, const TCHAR *nom_file, const void *data) {
	FRESULT res;
	uint32_t byteswritten;
	HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_1);
	FATFS_LinkDriver(&SD_Driver, SDPath);
	if (f_mount(&SDFatFS, (TCHAR
	const*) SDPath, 0) != FR_OK) {
		/* FatFs Initialization Error */
		Error_Handler();
	} else {
		if (f_open(fp, nom_file, FA_OPEN_APPEND | FA_WRITE) != FR_OK) {
			/* 'STM32.TXT' file Open for write Error */
			Error_Handler();
		} else {
			/*##-5- Write data to the text file ################################*/
			f_puts("  \n", fp);
			res = f_write(fp, data, sizeof(data), (void*) &byteswritten);

			if ((byteswritten == 0) || (res != FR_OK)) {
				/* 'STM32.TXT' file Write or EOF Error */
				Error_Handler();
			} else {
				/*##-6- Close the open text file #################################*/
				f_close(fp);

				/*##-7- Open the text file object with read access ###############*/
				//if(f_open(&file1, "STM32.TXT", FA_READ) != FR_OK)
				if (f_open(fp, "LOG_Temp.TXT", FA_READ) != FR_OK) {
					/* 'STM32.TXT' file Open for read Error */
					Error_Handler();
				} else {
					/*##-8- Read data from the text file ###########################*/
					//					  restest = f_read(&fp, rtext, sizeof(rtext), (UINT*)&bytesread);
					//
					//					  if((bytesread == 0) || (restest != FR_OK))
					//					  {
					//						  /* 'STM32.TXT' file Read or EOF Error */
					//						  Error_Handler();
					//					  }
					//					  else
					//					  {
					/*##-9- Close the open text file #############################*/
					f_close(fp);
					Flag_IRQ_Btn = 0;
					//
					//						  /*##-10- Compare read data with the expected data ############*/
					//						  if((bytesread != byteswritten))
					//						  {
					//							  /* Read data is different from the expected data */
					//							  Error_Handler();
					//
					//						  }
					//						  else
					//						  {
					//						  }
				}
			}
		}
	}
	FATFS_UnLinkDriver(SDPath);
}
/*---------------------------------------------------------- STILL TO ORGANIZE*/

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
