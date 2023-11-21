/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "print_uart.h"
#include "lps22hh_reg.h"
#include "hts221_reg.h"

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#define SENSOR_BUS hi2c1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define BOOT_TIME 5
#define TX_BUF_DIM 1000
#define ADC_BUF_LEN 1
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint32_t data_raw_pressure;
static int16_t data_raw_humidity;
static int16_t data_raw_temperature;

static float pressure_hPa;
static float humidity_perc;
static float temperature_degC;
static float windspeed_kph;
static float rainfall_mm;

static uint8_t whoamI, rst;
static uint8_t tx_buffer[TX_BUF_DIM];
static uint16_t adc_buf[ADC_BUF_LEN];

volatile uint8_t start_measures = 0;
volatile uint8_t tmp_sns_d_rdy = 0;
volatile uint8_t prs_sns_d_rdy = 0;
volatile uint8_t retrieve_wind_speed = 0;
volatile uint8_t retrieve_rainfall = 0;
volatile uint8_t retrieve_wind_dir = 0;

// Variable de eliott

uint8_t IRQ_TS = 0;
static TS_StateTypeDef TS_State;
uint16_t x, y;
uint8_t test_mes = 0;
uint8_t refresh = 0;
uint8_t mesure = 0;

char * temperature = "12345" ;
char * Humidité = "12345";
char * Pression = "1234567";
char * vitesse = "12345";
uint8_t direction = 6;
char * Pluvio = "12345";


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void static lps22hh_Init(void);
void static hts221_Init(void);

static int32_t lps22hh_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len);
static int32_t lps22hh_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);

static int32_t hts221_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len);
static int32_t hts221_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);

static void tx_com(uint8_t *tx_buffer, uint16_t len);
static void platform_delay(uint32_t ms);

static float calculateWindSpeed(uint16_t timer_counter);
static float calculateRainfall(uint16_t timer_counter);
static const char* determineDirection(uint16_t adc_value);

static stmdev_ctx_t dev_ctx_lps22hh;
static stmdev_ctx_t dev_ctx_hts221;

typedef struct {
	float x0;
	float y0;
	float x1;
	float y1;
} lin_t;

lin_t lin_hum;
lin_t lin_temp;

float linear_interpolation(lin_t *lin, int16_t x) {
	return ((lin->y1 - lin->y0) * x + ((lin->x1 * lin->y0) - (lin->x0 * lin->y1))) / (lin->x1 - lin->x0);
}
void Display_LCD_Init();
void Display_LCD_Button(int);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
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
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_TIM7_Init();
  MX_ADC1_Init();
  MX_TIM8_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_LTDC_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim7);
	HAL_TIM_Base_Start(&htim8);
	HAL_TIM_Base_Start(&htim2);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_buf, ADC_BUF_LEN);

	lps22hh_Init();
	hts221_Init();

	BSP_LCD_Init();
	Display_LCD_Init();
	Display_LCD_Button(0);

	BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
	BSP_TS_ResetTouchData(&TS_State);
	BSP_TS_ITConfig();
	BSP_TS_ITClear();

	x = TS_State.touchX[0];
	y = TS_State.touchY[0];


	// print that the initialization is done
	printf("Init done\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		if (start_measures == 1) {
			printf("Measures conversions started\r\n");
			// start the temperature conversion
			hts221_one_shoot_trigger_set(&dev_ctx_hts221, PROPERTY_ENABLE);

			uint8_t ctrl_reg2 = 0;
			lps22hh_read_reg(&dev_ctx_lps22hh, LPS22HH_CTRL_REG2, &ctrl_reg2, 1);
			ctrl_reg2 |= 1;
			lps22hh_write_reg(&dev_ctx_lps22hh, LPS22HH_CTRL_REG2, &ctrl_reg2, 1);

			start_measures = 0;
		}

		if (tmp_sns_d_rdy == 1) {
			/* Read humidity data */
			memset(&data_raw_humidity, 0x00, sizeof(int16_t));
			hts221_humidity_raw_get(&dev_ctx_hts221, &data_raw_humidity);
			humidity_perc = linear_interpolation(&lin_hum, data_raw_humidity);

			if (humidity_perc < 0) {
				humidity_perc = 0;
			}

			if (humidity_perc > 100) {
				humidity_perc = 100;
			}

			sprintf((char*) tx_buffer, "Humidity [%%]:%3.2f\r\n", humidity_perc);
			tx_com(tx_buffer, strlen((char const*) tx_buffer));

			/* Read temperature data */
			memset(&data_raw_temperature, 0x00, sizeof(int16_t));
			hts221_temperature_raw_get(&dev_ctx_hts221, &data_raw_temperature);
			temperature_degC = linear_interpolation(&lin_temp, data_raw_temperature);
			sprintf((char*) tx_buffer, "Temperature [degC]:%6.2f\r\n", temperature_degC);
			tx_com(tx_buffer, strlen((char const*) tx_buffer));

			tmp_sns_d_rdy = 0;
		}

		if (prs_sns_d_rdy == 1) {
			memset(&data_raw_pressure, 0x00, sizeof(uint32_t));
			lps22hh_pressure_raw_get(&dev_ctx_lps22hh, &data_raw_pressure);
			pressure_hPa = lps22hh_from_lsb_to_hpa(data_raw_pressure);
			sprintf((char*) tx_buffer, "Pressure [hPa]:%6.2f\r\n", pressure_hPa);
			tx_com(tx_buffer, strlen((char const*) tx_buffer));

			prs_sns_d_rdy = 0;
		}

		if (retrieve_wind_speed == 1) {
			uint16_t timer_counter = __HAL_TIM_GET_COUNTER(&htim8);
			windspeed_kph = calculateWindSpeed(timer_counter);

			printf("Wind speed [kph]:%f\r\n", windspeed_kph);
			__HAL_TIM_SET_COUNTER(&htim8, 0);

			retrieve_wind_speed = 0;
		}

		if (retrieve_rainfall >= 5) {
			uint16_t timer_counter = __HAL_TIM_GET_COUNTER(&htim2);
			rainfall_mm = calculateRainfall(timer_counter);

			printf("Rainfall [mm]:%f\r\n", rainfall_mm);
			__HAL_TIM_SET_COUNTER(&htim2, 0);

			retrieve_rainfall = 0;
		}

		if (retrieve_wind_dir == 1) {
			const char *direction = determineDirection(adc_buf[0]);
			printf("Direction: %s\r\n", direction);

			retrieve_wind_dir = 0;
		}


		// Gestion de l'afficage , Interruption Tuch_Screen

		  if (IRQ_TS == 1) {
			  BSP_TS_GetState(&TS_State);
			  if (( (80<x) && (x<120) && (y>50) && (y<90) && (test_mes == 0) ))
			  {
//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				  Display_LCD_Button(1);
				  test_mes = 1;
				  mesure = 1;
			  }
			  else if (((80<x)&&(x<120)&&(y>120)&&(y<160) && (test_mes == 0) ))
			  {
//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				  Display_LCD_Button(2);
				  test_mes = 1;
				  mesure = 2;
			  }
			  else if (((80<x)&&(x<120)&&(y>190)&&(y<230) && (test_mes == 0) ))
			  {
//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				  Display_LCD_Button(3);
				  test_mes = 1;
				  mesure = 3;
			  }

			  else if (((280<x)&&(x<320)&&(y>50)&&(y<90) && (test_mes == 0) ))
			  {
//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				  Display_LCD_Button(4);
				  test_mes = 1;
				  mesure = 4;
			  }
			  else if (((280<x)&&(x<320)&&(y>120)&&(y<160) && (test_mes == 0)))
			  {
//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				  Display_LCD_Button(5);
				  test_mes = 1;
				  mesure = 5;
			  }
			  else if (((280<x)&&(x<320)&&(y>190)&&(y<230) && (test_mes == 0) ))
			  {
//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				  Display_LCD_Button(6);
				  test_mes = 1;
				  mesure = 6;
			  }
			  else
			  {
				  if ( (test_mes == 1) && (380<x)&&(x<420)&&(y>190)&&(y<230) )
				  {
//					  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 0);
					  Display_LCD_Button(0);
					  test_mes = 0;
					  mesure = 0;
				  }
				  else if ( (test_mes == 1) && (30<x)&&(x<80)&&(y>190)&&(y<230) )
				  {
//					  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 0);
					  Display_LCD_Button(7);

						refresh = refresh +1;
						if (mesure == 1)
						{
//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
							Display_LCD_Button(1);
						}
						else if (mesure == 2)
						{
//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
							Display_LCD_Button(2);
						}
						else if (mesure == 3)
						{
//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
							Display_LCD_Button(3);
						}
						else if (mesure == 4)
						{
//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
							Display_LCD_Button(4);
						}
						else if (mesure == 5)
						{
//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
							Display_LCD_Button(5);
						}
						else if (mesure == 6)
						{
//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
							Display_LCD_Button(6);
						}

					  test_mes = 1;

				  }
			  }
			  IRQ_TS = 0;
		  }

		  else { //µP Sleep
			  HAL_SuspendTick();
			  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
PUTCHAR_PROTOTYPE {
	HAL_UART_Transmit(&huart1, (uint8_t*) &ch, 1, 100);
	return ch;
}

static void lps22hh_Init(void) {
	lps22hh_pin_int_route_t int_route;

	/* Initialize mems driver interface */
	dev_ctx_lps22hh.write_reg = lps22hh_write;
	dev_ctx_lps22hh.read_reg = lps22hh_read;
	dev_ctx_lps22hh.handle = &SENSOR_BUS;

	/* Wait sensor boot time */
	platform_delay(BOOT_TIME);
	/* Check device ID */
	whoamI = 0;
	int result = lps22hh_device_id_get(&dev_ctx_lps22hh, &whoamI);

	if (result == 0) {
		printf("WHO_AM_I: %d\r\n", whoamI);
	} else {
		printf("Error reading WHO_AM_I register\r\n");
	}

	if (whoamI != LPS22HH_ID)
		while (1)
			; /*manage here device not found */

	/* Restore default configuration */
	lps22hh_reset_set(&dev_ctx_lps22hh, PROPERTY_ENABLE);

	do {
		lps22hh_reset_get(&dev_ctx_lps22hh, &rst);
	} while (rst);

	lps22hh_int_notification_set(&dev_ctx_lps22hh, LPS22HH_INT_PULSED);

	lps22hh_pin_int_route_get(&dev_ctx_lps22hh, &int_route);
	int_route.drdy_pres = PROPERTY_ENABLE;
	lps22hh_pin_int_route_set(&dev_ctx_lps22hh, &int_route);

	/* Enable Block Data Update */
	lps22hh_block_data_update_set(&dev_ctx_lps22hh, PROPERTY_ENABLE);
	/* Set Output Data Rate */
	lps22hh_data_rate_set(&dev_ctx_lps22hh, LPS22HH_ONE_SHOOT);
}

static int32_t lps22hh_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len) {
	HAL_I2C_Mem_Write(handle, LPS22HH_I2C_ADD_H, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*) bufp, len, 1000);
	return 0;
}

static int32_t lps22hh_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len) {
	HAL_I2C_Mem_Read(handle, LPS22HH_I2C_ADD_H, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
	return 0;
}

static void hts221_Init(void) {
	dev_ctx_hts221.write_reg = hts221_write;
	dev_ctx_hts221.read_reg = hts221_read;
	dev_ctx_hts221.handle = &SENSOR_BUS;
	/* Check device ID */
	whoamI = 0;
	int result = hts221_device_id_get(&dev_ctx_hts221, &whoamI);

	if (result == 0) {
		printf("WHO_AM_I: %d\r\n", whoamI);
	} else {
		printf("Error reading WHO_AM_I register\r\n");
	}

	if (whoamI != HTS221_ID)
		while (1)
			; /*manage here device not found */

	/* Read humidity calibration coefficient */
	hts221_hum_adc_point_0_get(&dev_ctx_hts221, &lin_hum.x0);
	hts221_hum_rh_point_0_get(&dev_ctx_hts221, &lin_hum.y0);
	hts221_hum_adc_point_1_get(&dev_ctx_hts221, &lin_hum.x1);
	hts221_hum_rh_point_1_get(&dev_ctx_hts221, &lin_hum.y1);

	/* Read temperature calibration coefficient */
	hts221_temp_adc_point_0_get(&dev_ctx_hts221, &lin_temp.x0);
	hts221_temp_deg_point_0_get(&dev_ctx_hts221, &lin_temp.y0);
	hts221_temp_adc_point_1_get(&dev_ctx_hts221, &lin_temp.x1);
	hts221_temp_deg_point_1_get(&dev_ctx_hts221, &lin_temp.y1);

	/* Enable Block Data Update */
	hts221_block_data_update_set(&dev_ctx_hts221, PROPERTY_ENABLE);

	/* Set Output Data Rate */
	hts221_data_rate_set(&dev_ctx_hts221, HTS221_ONE_SHOT);

	hts221_drdy_on_int_set(&dev_ctx_hts221, PROPERTY_ENABLE);

	/* Device power on */
	hts221_power_on_set(&dev_ctx_hts221, PROPERTY_ENABLE);
}

static int32_t hts221_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len) {
	reg |= 0x80;
	HAL_I2C_Mem_Write(handle, HTS221_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*) bufp, len, 1000);
	return 0;
}

static int32_t hts221_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len) {
	/* Read multiple command */
	reg |= 0x80;
	HAL_I2C_Mem_Read(handle, HTS221_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
	return 0;
}

static void tx_com(uint8_t *tx_buffer, uint16_t len) {
	HAL_UART_Transmit(&huart1, tx_buffer, len, 1000);
}

static void platform_delay(uint32_t ms) {
	HAL_Delay(ms);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == IRQ_TEMP_Pin) {
		printf("Temp sensor OK\r\n");
		tmp_sns_d_rdy = 1;
	}

	if (GPIO_Pin == IRQ_PRESS_Pin) {
		printf("Pressure sensor OK\r\n");
		prs_sns_d_rdy = 1;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim7) {
		printf("Timer done\r\n");
		start_measures = 1;
		retrieve_wind_speed = 1;
		retrieve_rainfall++;
		retrieve_wind_dir = 1;
	}
}

static float calculateWindSpeed(uint16_t switchClosures) {
	// Conversion factor: 1.492 mph corresponds to 1 switch closure per second
	float countsPerSecond = (float) switchClosures / 60.0;
	// Conversion factor: 1 mph = 1.60934 kph
	float windSpeedKph = countsPerSecond * 1.492 * 1.60934;

	return windSpeedKph;
}

static float calculateRainfall(uint16_t switchClosures) {
	// Conversion factor: 1 switch closure corresponds to 0.011" (0.2794 mm) of rain
	float rainInches = (float) switchClosures * 0.011;
	// Conversion factor: 1 inch = 25.4 mm
	float rainMillimeters = rainInches * 25.4;

	return rainMillimeters;
}

static const char* determineDirection(uint16_t adcValue) {
	// Calculate the sector size based on the number of directions
	const int numDirections = 16;
	const int sectorSize = 4096 / numDirections;

	// Calculate the sector index based on the ADC value
	int sectorIndex = (adcValue + sectorSize / 2) / sectorSize;

	// Define the compass directions
	const char *compassDirections[] = { "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSO", "SO", "OSO", "O", "ONO", "NO", "NNO" };

	// Map the sector index to the corresponding direction
	return compassDirections[sectorIndex % numDirections];
}

void Display_LCD_Init(){
	BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
	BSP_LCD_SetLayerVisible(LTDC_ACTIVE_LAYER, ENABLE);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)"Menu Principal :", CENTER_MODE);
}

void Display_LCD_Button(int color){
	if (color == 1)	{

		if (refresh == 0)
		{
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)"TEMPERATURE :", CENTER_MODE);


			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4 ), (uint8_t *)"REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4 ), (uint8_t *)"HOME", RIGHT_MODE);
		}

			BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t *)"[         'C ]", CENTER_MODE);

			BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t*) &temperature , CENTER_MODE);


		refresh = 1;


	}
	else if (color == 2)
	{
		if (refresh ==0)
		{
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)"HUMIDITE :", CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4 ), (uint8_t *)"REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4 ), (uint8_t *)"HOME", RIGHT_MODE);
		}
		// Affichage de la mesure

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t *)"[         % ]", CENTER_MODE);

		BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t*) &Humidité , CENTER_MODE);

		refresh = 1;

	}
	else if (color == 3)
	{
		if (refresh == 0)
		{
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)"PRESSION :", CENTER_MODE);


			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4 ), (uint8_t *)"REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4 ), (uint8_t *)"HOME", RIGHT_MODE);
		}

		//Affichage de la mesure

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t *)"[            Pa ]", CENTER_MODE);
		BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t*) &Pression , CENTER_MODE);

		refresh = 1;
	}
	else if (color == 4)
	{
		if (refresh == 0)
		{
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)"VITESSE :", CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4 ), (uint8_t *)"REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4 ), (uint8_t *)"HOME", RIGHT_MODE);
		}

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t *)"[         m/s ]", CENTER_MODE);
		BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t*) &vitesse , CENTER_MODE);
		refresh = 1;

	}
	else if (color == 5)
	{
		if (refresh == 0)
		{
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)" DIRECTION :", CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4 ), (uint8_t *)"REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4 ), (uint8_t *)"HOME", RIGHT_MODE);
		}

		// Affichage de la mesure

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

		if ( direction == 0 )
		{
			BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t *)"[ N ]", CENTER_MODE);
		}
		else if ( direction == 1 )
		{
			BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t *)"[ NE ]", CENTER_MODE);
		}
		else if ( direction == 2 )
		{
			BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t *)"[ E ]", CENTER_MODE);
		}
		else if ( direction == 3 )
		{
			BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t *)"[ SE ]", CENTER_MODE);
		}
		else if ( direction == 4 )
		{
			BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t *)"[ S ]", CENTER_MODE);
		}
		else if ( direction == 5 )
		{
			BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t *)"[ SW ]", CENTER_MODE);
		}
		else if ( direction == 6 )
		{
			BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t *)"[ W ]", CENTER_MODE);
		}
		else if ( direction == 7 )
		{
			BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t *)"[ NW ]", CENTER_MODE);
		}
		refresh = 1;

	}
	else if (color == 6)
	{
		if (refresh == 0)
		{
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)"PLUVIOMETRIE :", CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4 ), (uint8_t *)"REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4 ), (uint8_t *)"HOME", RIGHT_MODE);
		}
		// Affichage de la mesure

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t *)"[         mm/H ]", CENTER_MODE);
		BSP_LCD_DisplayStringAt( 10 ,BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/2) , (uint8_t*) &Pluvio , CENTER_MODE);
		refresh = 1;

	}
	else if (color ==7)
	{
		refresh = 1;
		if (mesure == 1)
		{
//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(1);
		}
		else if (mesure == 2)
		{
//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(2);
		}
		else if (mesure == 3)
		{
//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(3);
		}
		else if (mesure == 4)
		{
//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(4);
		}
		else if (mesure == 5)
		{
//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(5);
		}
		else if (mesure == 6)
		{
//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(6);
		}
	}
	else
	{

		// Titre

		BSP_LCD_Clear(LCD_COLOR_BLACK);
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)"Menu Principal :", CENTER_MODE);
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

		// BTN P1

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(100 , BSP_LCD_GetYSize()-(3*BSP_LCD_GetYSize()/4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_FillCircle(100 , BSP_LCD_GetYSize()-(3*BSP_LCD_GetYSize()/4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130 , BSP_LCD_GetYSize()-(3*BSP_LCD_GetYSize()/4 ), (uint8_t *)"P1:Temp", LEFT_MODE);

		// BTN P2

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(100 , BSP_LCD_GetYSize()-(2*BSP_LCD_GetYSize()/4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
		BSP_LCD_FillCircle(100 , BSP_LCD_GetYSize()-(2*BSP_LCD_GetYSize()/4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130 , BSP_LCD_GetYSize()-(2*BSP_LCD_GetYSize()/4 ), (uint8_t *)"P2:Hum ", LEFT_MODE);


		// BTN P3

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(100 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
		BSP_LCD_FillCircle(100 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4 ), (uint8_t *)"P3:Pres", LEFT_MODE);


		// BTN P4

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(300 , BSP_LCD_GetYSize()-(3*BSP_LCD_GetYSize()/4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
		BSP_LCD_FillCircle(300 , BSP_LCD_GetYSize()-(3*BSP_LCD_GetYSize()/4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330 , BSP_LCD_GetYSize()-(3*BSP_LCD_GetYSize()/4 ), (uint8_t *)"P4:Vit", LEFT_MODE);


		// BTN P5

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(300 , BSP_LCD_GetYSize()-(2*BSP_LCD_GetYSize()/4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
		BSP_LCD_FillCircle(300 , BSP_LCD_GetYSize()-(2*BSP_LCD_GetYSize()/4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330 , BSP_LCD_GetYSize()-(2*BSP_LCD_GetYSize()/4 ), (uint8_t *)"P5:Dir", LEFT_MODE);


		// BTN P6

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(300 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
		BSP_LCD_FillCircle(300 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330 , BSP_LCD_GetYSize()-(1*BSP_LCD_GetYSize()/4 ), (uint8_t *)"P6:Pluv", LEFT_MODE);


		refresh =0;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	HAL_ResumeTick();
	BSP_TS_GetState(&TS_State);
	x = TS_State.touchX[0];
	y = TS_State.touchY[0];
	IRQ_TS = 1;

}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
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
