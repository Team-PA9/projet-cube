/* SDCARD includes -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "fatfs.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_lcd.h"
#include "user_def_sdcard.h"

/* SDCARD variables ----------------------------------------------------------*/
FIL file1; /* File read buffer */
uint32_t byteswritten; /* File write/read counts */
uint8_t workBuffer[_MAX_SS];

extern int index_HT;
extern int index_Pr;
extern int index_WS;
extern int index_WD;
extern int index_Rf;

extern float log_humidity[];
extern float log_temperature[];
extern float log_pressure[];
extern float log_windspeed[];
extern uint8_t log_wind_direction[];
extern double log_rainfall[];

extern char *compassDirections[];

extern uint8_t Flag_SaveHT;
extern uint8_t Flag_SavePr;
extern uint8_t Flag_SaveWS;
extern uint8_t Flag_SaveWD;
extern uint8_t Flag_SaveRf;

extern RTC_TimeTypeDef stimestructure;
extern RTC_TimeTypeDef gtimestructureget;
extern RTC_HandleTypeDef hrtc;



/* SDCARD functions ----------------------------------------------------------*/
void SDCARD_Init(void) {
	saving_log();
	if (f_mount(&SDFatFS, (TCHAR const*) SDPath, 0) != FR_OK) {
		Error_Handler(); /* FatFs Initialization Error */
	} else {
		if (f_mkfs((TCHAR const*) SDPath, FM_ANY, 0, workBuffer,
				sizeof(workBuffer)) != FR_OK) {
			Error_Handler(); /* FatFs Format Error */
		} else {
			new_log(&file1, "LOG.csv");
		}
	}
	FATFS_UnLinkDriver(SDPath);
}

void SDCARD_Actualization(void) {
	saving_log();
	// Interruption hts221
	if (Flag_SaveHT == 1) {
	// Temperature
		add_log(&file1, "LOG.csv", "Temperature (°C)", log_temperature, index_HT);
	// Humidite
		add_log(&file1, "LOG.csv", "Humidite (%)", log_humidity, index_HT);
		index_HT = 0;
		Flag_SaveHT = 0;
	}

	// Interruption lps22hh
	else if (Flag_SavePr == 1) {
		// Pression
		add_log(&file1, "LOG.csv", "Pression (HPa)", log_pressure, index_Pr);
		index_Pr = 0;
		Flag_SavePr = 0;
	}

	// Interruption Wind Speed
	else if (Flag_SaveWS == 1) {
		// Temperature
		add_log(&file1, "LOG.csv", "Vitesse (m/s)", log_windspeed, index_WS);
		index_WS = 0;
		Flag_SaveWS = 0;
	}

	// Interruption Rainfall
	else if (Flag_SaveRf == 1) {
		// Temperature
		add_log(&file1, "LOG.csv", "Pluie (mm)", log_rainfall, index_Rf);
		index_Rf = 0;
		Flag_SaveRf = 0;
	}

	// Interruption Wind Direction
	else if (Flag_SaveWD == 1) {
		// Temperature
		add_log(&file1, "LOG.csv", "Direction", log_wind_direction, index_WD);
		index_WD = 0;
		Flag_SaveWD = 0;
	}
	BSP_LCD_DisplayStringAt(0, 250, (uint8_t *)"         ", LEFT_MODE);
}

void new_log(FIL *fp, const char *filename) {
	if (f_open(fp, filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
		Error_Handler();
	} else {
		f_puts("Capteur, Time,", fp);
		int i = 0;
		while(i <= 99)
		{
			f_puts("Mesure,", fp);
			i++;
		}
		f_close(fp);
		}
}


void add_log(FIL *fp, const char *filename, const char *sensor, double values[], int index) {
	    if (f_open(fp, filename, FA_OPEN_EXISTING | FA_WRITE) != FR_OK) {
	        Error_Handler();
	    } else {
	    	//fseek(fp, 0, SEEK_END);  si on a besoin, tester
	    	//f_lseek(fp, f_size(fp));
	    	//f_puts("\r\n", fp) pour aller a gauche et sauter une ligne
	    	RTC_TimeTypeDef sTime;
	    	HAL_RTC_GetDate(&hrtc, &gtimestructureget, RTC_FORMAT_BCD);
			char timeString[20];
			sprintf(timeString, "Time: %02d:%02d:%02d\r\n", gtimestructureget.Hours,gtimestructureget.Minutes, gtimestructureget.Seconds);
	    	f_puts("\n", fp);
	    	f_puts(sensor, fp);
	    	f_puts(",", fp);
	    	f_puts(timeString, fp);
	    	for (int i = 0; i < index; i++) {
	    		char valueString[20];  // Adjust the buffer size as needed
				sprintf(valueString, "%lf", values[i]);
				f_puts(",", fp);
				f_puts(valueString, fp);
//				f_puts(values[i], fp);
			}
	        f_close(fp);
	    }
}

void saving_log(void)
{
	BSP_LCD_SetLayerVisible(LTDC_LAYER_2, ENABLE);
	BSP_LCD_SelectLayer(LTDC_LAYER_2);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(0, 250, (uint8_t *)"Saving...", LEFT_MODE);
}
//void new_log(FIL *fp, const char *filename, const char *content) {
//	FRESULT res;
//	uint32_t byteswritten;
//	if (f_open(fp, filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
//		Error_Handler();
//	} else {
//		res = f_write(fp, content, strlen(content), (void*) &byteswritten);
//
//		if ((byteswritten == 0) || (res != FR_OK)) {
//			Error_Handler();
//		} else {
//			f_close(fp);
//		}
//	}
//}
//
//void add_log(FIL *fp, const char *filename, char *data, float value,
//		const char *unit) {
//	FATFS_LinkDriver(&SD_Driver, SDPath);
//	if (f_mount(&SDFatFS, (TCHAR const*) SDPath, 0) != FR_OK) {
//		Error_Handler();
//	} else {
//		if (f_open(fp, filename, FA_OPEN_APPEND | FA_WRITE) != FR_OK) {
//			Error_Handler();
//		} else {
//			f_puts("\n", fp);
//			f_puts(data, fp);
//			f_close(fp);
//		}
//	}
//	FATFS_UnLinkDriver(SDPath);
//}

//void SDCARD_Actualization(void) {
//	// Interruption hts221
//	if (Flag_SaveHT == 1) {
//		for (int i = 0; i <= index_HT; i++) {
//			// Temperature
//			char displayString[20];
//			sprintf(displayString, "[ %6.2f 'C ]", log_temperature[i]);
//			add_log(&file1, "LOG_Temp.txt", displayString, log_temperature[i],
//					"°C");
//			// Humidity
//			char displayString2[20];
//			sprintf(displayString2, "[ %6.2f %% ]", log_humidity[i]);
//			add_log(&file6, "LOG_Humi.txt", displayString2, log_humidity[i],
//					"%");
//		}
//		index_HT = 0;
//		Flag_SaveHT = 0;
//	}
//
//	// Interruption lps22hh
//	else if (Flag_SavePr == 1) {
//		for (int i = 0; i <= index_Pr; i++) {
//			char displayString[20];
//			sprintf(displayString, "[ %6.2f hPa ]", log_pressure[i]);
//			add_log(&file2, "LOG_Pres.txt", displayString, log_pressure[i],
//					"hPa");
//		}
//		Flag_SavePr = 0;
//	}
//
//	// Interruption Wind Speed
//	else if (Flag_SaveWS == 1) {
//		for (int i = 0; i <= index_WS; i++) {
//			char displayString[20];
//			sprintf(displayString, "[ %6.2f km/h ]", log_windspeed[i]);
//			add_log(&file3, "LOG_Wspe.txt", displayString, log_windspeed[i],
//					"km/h");
//		}
//		Flag_SaveWS = 0;
//	}
//
//	// Interruption Rainfall
//	else if (Flag_SaveRf == 1) {
//		for (int i = 0; i <= index_Rf; i++) {
//			char displayString[20];
//			sprintf(displayString, "[ %6.2f mm/h ]", log_rainfall[i]);
//			add_log(&file5, "LOG_Rain.txt", displayString, log_rainfall[i],
//					"mm/h");
//		}
//		Flag_SaveRf = 0;
//	}
//
//	// Interruption Wind Direction
//	else if (Flag_SaveWD == 1) {
//		for (int i = 0; i <= index_HT; i++) {
//			char displayString[20];
//			sprintf(displayString, "[ %s ]",
//					compassDirections[log_wind_direction[i]]);
//			add_log(&file5, "LOG_WDir.txt", displayString,
//					sprintf("%s", compassDirections[log_wind_direction[i]]),
//					"");
//		}
//		Flag_SaveWD = 0;
//	}
//
//}
