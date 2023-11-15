/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "print_uart.h"
#include "lps22hh_reg.h"
#include "hts221_reg.h"
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
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint32_t data_raw_pressure;
static int16_t data_raw_humidity;
static int16_t data_raw_temperature;

static float pressure_hPa;
static float humidity_perc;
static float temperature_degC;
float windspeed_kph;
float rainfall_mm;

static uint8_t whoamI, rst;
static uint8_t tx_buffer[TX_BUF_DIM];

volatile uint8_t start_measures = 0;
volatile uint8_t tmp_sns_d_rdy = 0;
volatile uint8_t prs_sns_d_rdy = 0;
volatile uint8_t retrieve_wind_speed = 0;
volatile uint8_t retrieve_rainfall = 0;

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

float calculateWindSpeed(uint16_t timer_counter);
float calculateRainfall(uint16_t timer_counter);

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
	MX_USART1_UART_Init();
	MX_I2C1_Init();
	MX_TIM7_Init();
	MX_TIM8_Init();
	MX_TIM2_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim7);
	HAL_TIM_Base_Start_IT(&htim8);
	HAL_TIM_Base_Start_IT(&htim2);
	lps22hh_Init();
	hts221_Init();
	// print that the initialization is done
	printf("Init done\r\n");

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		if (start_measures == 1) {
			printf("measures conversions started.\r\n");
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
			sprintf((char*) tx_buffer, "pressure [hPa]:%6.2f\r\n", pressure_hPa);
			tx_com(tx_buffer, strlen((char const*) tx_buffer));

			prs_sns_d_rdy = 0;
		}

		if (retrieve_wind_speed == 1) {
			uint16_t timer_counter = __HAL_TIM_GET_COUNTER(&htim2);
			windspeed_kph = calculateWindSpeed(timer_counter);

			printf("Wind speed [kph]: %f\r\n", windspeed_kph);
			__HAL_TIM_SET_COUNTER(&htim2, 0);

			retrieve_wind_speed = 0;
			windspeed_kph = 0;
		}

		if (retrieve_rainfall >= 5) {
			uint16_t timer_counter = __HAL_TIM_GET_COUNTER(&htim8);
			rainfall_mm = calculateRainfall(timer_counter);

			printf("Rainfall [mm]: %f\r\n", rainfall_mm);
			__HAL_TIM_SET_COUNTER(&htim8, 0);

			retrieve_rainfall = 0;
			rainfall_mm = 0;
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
	RCC_OscInitStruct.PLL.PLLQ = 2;
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
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK) {
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
		printf("temp sensor OK\r\n");
		tmp_sns_d_rdy = 1;
	}

	if (GPIO_Pin == IRQ_PRESS_Pin) {
		printf("pressure sensor OK\r\n");
		prs_sns_d_rdy = 1;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim7) {
		printf("timer done\r\n");
		start_measures = 1;
		retrieve_wind_speed = 1;
		retrieve_rainfall++;
	}
}

float calculateWindSpeed(uint16_t switchClosures) {
	// Conversion factor: 1.492 mph corresponds to 1 switch closure per second
	float countsPerSecond = (float) switchClosures / 60.0;
	// Conversion factor: 1 mph = 1.60934 kph
	float windSpeedKph = countsPerSecond * 1.492 * 1.60934;

	return windSpeedKph;
}

float calculateRainfall(uint16_t switchClosures) {
	// Conversion factor: 1 switch closure corresponds to 0.011" (0.2794 mm) of rain
	float rainInches = (float) switchClosures * 0.011;
	// Conversion factor: 1 inch = 25.4 mm
	float rainMillimeters = rainInches * 25.4;

	return rainMillimeters;
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
