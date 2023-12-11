/* SDCARD includes -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "fatfs.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_lcd.h"
#include "user_def_sdcard.h"
#include "user_def_screen.h"

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
	if (f_mount(&SDFatFS, (TCHAR const*) SDPath, 0) != FR_OK) {
		Error_Handler(); /* FatFs Initialization Error */
	} else {
		if (f_mkfs((TCHAR const*) SDPath, FM_ANY, 0, workBuffer,
				sizeof(workBuffer)) != FR_OK) {
			Error_Handler(); /* FatFs Format Error */
		} else {
			SDCARD_NewLog(&file1, "LOG.CSV");
		}
	}
	FATFS_UnLinkDriver(SDPath);
}

void SDCARD_Actualization(void) {
	Display_LCD_SavingLog();
	// Interruption hts221
	if (Flag_SaveHT == 1) {
		// Temperature
		SDCARD_AddLog_flt(&file1, "LOG.CSV", "Temperature (°C)", log_temperature,
				index_HT);
		// Humidity
		SDCARD_AddLog_flt(&file1, "LOG.CSV", "Humidite (%)", log_humidity,
				index_HT);
		index_HT = 0;
		Flag_SaveHT = 0;
	}

	// Interruption lps22hh
	else if (Flag_SavePr == 1) {
		// Pressure
		SDCARD_AddLog_flt(&file1, "LOG.CSV", "Pression (HPa)", log_pressure,
				index_Pr);
		index_Pr = 0;
		Flag_SavePr = 0;
	}

	// Interruption Wind Speed
	else if (Flag_SaveWS == 1) {
		// Wind Speed
		SDCARD_AddLog_flt(&file1, "LOG.CSV", "Vitesse (m/s)", log_windspeed,
				index_WS);
		index_WS = 0;
		Flag_SaveWS = 0;
	}

	// Interruption Rainfall
	else if (Flag_SaveRf == 1) {
		// Rainfall
		SDCARD_AddLog_dbl(&file1, "LOG.CSV", "Pluie (mm)", log_rainfall, index_Rf);
		index_Rf = 0;
		Flag_SaveRf = 0;
	}

	// Interruption Wind Direction
	else if (Flag_SaveWD == 1) {
		// Wind Direction
		SDCARD_AddLog_int(&file1, "LOG.CSV", "Direction", log_wind_direction,
				index_WD);
		index_WD = 0;
		Flag_SaveWD = 0;
	}

	BSP_LCD_DisplayStringAt(0, 250, (uint8_t*) "         ", LEFT_MODE);
}

void SDCARD_NewLog(FIL *fp, const char *filename) {
	if (f_open(fp, filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
		Error_Handler();
	} else {
		f_puts("Capteur, Time,", fp);
		int i = 0;
		while (i <= 99) {
			f_puts("Mesure,", fp);
			i++;
		}
		f_close(fp);
	}
}

void SDCARD_AddLog_int(FIL *fp, const char *filename, const char *sensor,
		uint8_t values[], int index) {
	FATFS_LinkDriver(&SD_Driver, SDPath);
	if (f_mount(&SDFatFS, (TCHAR const*) SDPath, 0) != FR_OK) {
		Error_Handler();
	} else {
		if (f_open(fp, filename, FA_OPEN_EXISTING | FA_WRITE) != FR_OK) {
			Error_Handler();
		} else {
			f_lseek(fp, f_size(fp));
			//fseek(fp, 0, SEEK_END);  si on a besoin, tester
			//f_puts("\r\n", fp) pour aller a gauche et sauter une ligne
			//RTC_TimeTypeDef sTime;
			//HAL_RTC_GetDate(&hrtc, &gtimestructureget, RTC_FORMAT_BCD);
			char timeString[20];
			sprintf(timeString, "Time");
			//sprintf(timeString, "Time: %02d:%02d:%02d\r\n", gtimestructureget.Hours,
			//		gtimestructureget.Minutes, gtimestructureget.Seconds);
			f_puts("\n", fp);
			f_puts(sensor, fp);
			f_puts(",", fp);
			f_puts(timeString, fp);
			for (int i = 0; i < index; i++) {
				char valueString[20];  // Adjust the buffer size as needed
				sprintf(valueString, "%d", values[i]);
				f_puts(",", fp);
				f_puts(valueString, fp);
	//			f_puts(values[i], fp);
			}
			f_close(fp);
		}
	}
	FATFS_UnLinkDriver(SDPath);
}

void SDCARD_AddLog_dbl(FIL *fp, const char *filename, const char *sensor,
		double values[], int index) {
	FATFS_LinkDriver(&SD_Driver, SDPath);
	if (f_mount(&SDFatFS, (TCHAR const*) SDPath, 0) != FR_OK) {
		Error_Handler();
	} else {
		if (f_open(fp, filename, FA_OPEN_EXISTING | FA_WRITE) != FR_OK) {
			Error_Handler();
		} else {
			f_lseek(fp, f_size(fp));
			char timeString[20];
			sprintf(timeString, "Time");
			f_puts("\n", fp);
			f_puts(sensor, fp);
			f_puts(",", fp);
			f_puts(timeString, fp);
			for (int i = 0; i < index; i++) {
				char valueString[20];  // Adjust the buffer size as needed
				sprintf(valueString, "%lf", values[i]);
				f_puts(",", fp);
				f_puts(valueString, fp);
			}
			f_close(fp);
		}
	}
	FATFS_UnLinkDriver(SDPath);
}

void SDCARD_AddLog_flt(FIL *fp, const char *filename, const char *sensor,
		float values[], int index) {
	FATFS_LinkDriver(&SD_Driver, SDPath);
	if (f_mount(&SDFatFS, (TCHAR const*) SDPath, 0) != FR_OK) {
		Error_Handler();
	} else {
		if (f_open(fp, filename, FA_OPEN_EXISTING | FA_WRITE) != FR_OK) {
			Error_Handler();
		} else {
			f_lseek(fp, f_size(fp));
			char timeString[20];
			sprintf(timeString, "Time");
			f_puts("\n", fp);
			f_puts(sensor, fp);
			f_puts(",", fp);
			f_puts(timeString, fp);
			for (int i = 0; i < index; i++) {
				char valueString[20];  // Adjust the buffer size as needed
				sprintf(valueString, "%f", values[i]);
				f_puts(",", fp);
				f_puts(valueString, fp);
			}
			f_close(fp);
		}
	}
	FATFS_UnLinkDriver(SDPath);
}
