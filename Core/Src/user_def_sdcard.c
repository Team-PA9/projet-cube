/* SDCARD includes -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "fatfs.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_lcd.h"
#include "user_def_sdcard.h"
#include "user_def_screen.h"
#include "user_def_rtc.h"

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

extern RTC_TimeTypeDef sTime1;
extern RTC_DateTypeDef sDate1;
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
}

void SDCARD_Actualization(void) {
	printf("Saving measurements to SD card...\r\n");
	Display_LCD_Saving();
	// Interruption hts221
	if (Flag_SaveHT == 1) {
		SDCARD_AddLog_HTPWS(&file1, "LOG.CSV", "Temperature (°C)",
				log_temperature, index_HT);
		SDCARD_AddLog_HTPWS(&file1, "LOG.CSV", "Humidite (%)", log_humidity,
				index_HT);
		index_HT = 0;
		Flag_SaveHT = 0;
	}

	// Interruption lps22hh
	else if (Flag_SavePr == 1) {
		SDCARD_AddLog_HTPWS(&file1, "LOG.CSV", "Pression (HPa)", log_pressure,
				index_Pr);
		index_Pr = 0;
		Flag_SavePr = 0;
	}

	// Interruption Wind Speed
	else if (Flag_SaveWS == 1) {
		SDCARD_AddLog_HTPWS(&file1, "LOG.CSV", "Vitesse (m/s)", log_windspeed,
				index_WS);
		index_WS = 0;
		Flag_SaveWS = 0;
	}

	// Interruption Rainfall
	else if (Flag_SaveRf == 1) {
		SDCARD_AddLog_Rf(&file1, "LOG.CSV", "Pluie (mm)", log_rainfall,
				index_Rf);
		index_Rf = 0;
		Flag_SaveRf = 0;
	}

	// Interruption Wind Direction
	else if (Flag_SaveWD == 1) {
		SDCARD_AddLog_WD(&file1, "LOG.CSV", "Direction", log_wind_direction,
				index_WD);
		index_WD = 0;
		Flag_SaveWD = 0;
	}
	BSP_LCD_DisplayStringAt(0, 255, (uint8_t*) "         ", RIGHT_MODE);
	BSP_LCD_SetFont(&Font16);
	printf("Measurements successfully saved to SD card.\r\n");
}

void SDCARD_NewLog(FIL *fp, const char *filename) {
	if (f_open(fp, filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
		Error_Handler();
	} else {
		f_puts("Capteur,Time", fp);
		int i = 0;
		while (i <= 99) {
			char titleString[10];
			sprintf(titleString, ",Mesure %d", i + 1);
			f_puts(titleString, fp);
			i++;
		}
		f_puts("\n", fp);
		f_close(fp);
	}
}

void SDCARD_AddLog_WD(FIL *fp, const char *filename, const char *sensor,
		uint8_t values[], int index) {
	if (f_open(fp, filename, FA_OPEN_EXISTING | FA_WRITE) != FR_OK) {
		Error_Handler();
	} else {
		f_lseek(fp, f_size(fp));
		f_puts(sensor, fp);
		f_puts(",", fp);
		char timeString[20];
		RTC_Get_UTC_Timestamp(timeString);
		f_puts(timeString, fp);
		for (int i = 0; i < index; i++) {
			char valueString[20];
			sprintf(valueString, ",%s", compassDirections[values[i]]);
			f_puts(valueString, fp);
		}
		f_puts("\n", fp);
		f_close(fp);
	}
}

void SDCARD_AddLog_Rf(FIL *fp, const char *filename, const char *sensor,
		double values[], int index) {
	if (f_open(fp, filename, FA_OPEN_EXISTING | FA_WRITE) != FR_OK) {
		Error_Handler();
	} else {
		f_lseek(fp, f_size(fp));
		f_puts(sensor, fp);
		f_puts(",", fp);
		char timeString[20];
		RTC_Get_UTC_Timestamp(timeString);
		f_puts(timeString, fp);
		for (int i = 0; i < index; i++) {
			char valueString[20];
			sprintf(valueString, ",%lf", values[i]);
			f_puts(valueString, fp);
		}
		f_puts("\n", fp);
		f_close(fp);
	}
}

void SDCARD_AddLog_HTPWS(FIL *fp, const char *filename, const char *sensor,
		float values[], int index) {
	if (f_open(fp, filename, FA_OPEN_EXISTING | FA_WRITE) != FR_OK) {
		Error_Handler();
	} else {
		f_lseek(fp, f_size(fp));
		f_puts(sensor, fp);
		f_puts(",", fp);
		char timeString[20];
		RTC_Get_UTC_Timestamp(timeString);
		f_puts(timeString, fp);
		for (int i = 0; i < index; i++) {
			char valueString[20];
			sprintf(valueString, ",%f", values[i]);
			f_puts(valueString, fp);
		}
		f_puts("\n", fp);
		f_close(fp);
	}
}
