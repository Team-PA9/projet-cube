/* SDCARD includes -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "fatfs.h"
#include "stm32746g_discovery_sdram.h"
#include "user_def_sdcard.h"

/* SDCARD variables ----------------------------------------------------------*/
FIL file1, file2, file3, file4, file5, file6; /* File read buffer */
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

/* SDCARD functions ----------------------------------------------------------*/
void SDCARD_Init(void) {
	if (f_mount(&SDFatFS, (TCHAR const*) SDPath, 0) != FR_OK) {
		Error_Handler(); /* FatFs Initialization Error */
	} else {
		if (f_mkfs((TCHAR const*) SDPath, FM_ANY, 0, workBuffer,
				sizeof(workBuffer)) != FR_OK) {
			Error_Handler(); /* FatFs Format Error */
		} else {
			new_log(&file1, "LOG_Temp.txt", "Log Temperature");
			new_log(&file2, "LOG_Pres.txt", "Log Pressure");
			new_log(&file3, "LOG_WDir.txt", "Log Wind Direction");
			new_log(&file4, "LOG_WSpe.txt", "Log Wind Speed");
			new_log(&file5, "LOG_Rain.txt", "Log Rainfall");
			new_log(&file6, "LOG_Humi.txt", "Log Humidity");
		}
	}
	FATFS_UnLinkDriver(SDPath);
}

void SDCARD_Actualization(void) {
	// Interruption hts221
	if (Flag_SaveHT == 1) {
		for (int i = 0; i <= index_HT; i++) {
			// Temperature
			char displayString[20];
			sprintf(displayString, "[ %6.2f 'C ]", log_temperature[i]);
			add_log(&file1, "LOG_Temp.txt", displayString, log_temperature[i],
					"°C");
			// Humidity
			char displayString2[20];
			sprintf(displayString2, "[ %6.2f %% ]", log_humidity[i]);
			add_log(&file6, "LOG_Humi.txt", displayString2, log_humidity[i],
					"%");
		}
		index_HT = 0;
		Flag_SaveHT = 0;
	}

	// Interruption lps22hh
	else if (Flag_SavePr == 1) {
		for (int i = 0; i <= index_Pr; i++) {
			char displayString[20];
			sprintf(displayString, "[ %6.2f hPa ]", log_pressure[i]);
			add_log(&file2, "LOG_Pres.txt", displayString, log_pressure[i],
					"hPa");
		}
		Flag_SavePr = 0;
	}

	// Interruption Wind Speed
	else if (Flag_SaveWS == 1) {
		for (int i = 0; i <= index_WS; i++) {
			char displayString[20];
			sprintf(displayString, "[ %6.2f km/h ]", log_windspeed[i]);
			add_log(&file3, "LOG_Wspe.txt", displayString, log_windspeed[i],
					"km/h");
		}
		Flag_SaveWS = 0;
	}

	// Interruption Rainfall
	else if (Flag_SaveRf == 1) {
		for (int i = 0; i <= index_Rf; i++) {
			char displayString[20];
			sprintf(displayString, "[ %6.2f mm/h ]", log_rainfall[i]);
			add_log(&file5, "LOG_Rain.txt", displayString, log_rainfall[i],
					"mm/h");
		}
		Flag_SaveRf = 0;
	}

	// Interruption Wind Direction
	else if (Flag_SaveWD == 1) {
		for (int i = 0; i <= index_HT; i++) {
			char displayString[20];
			sprintf(displayString, "[ %s ]",
					compassDirections[log_wind_direction[i]]);
			add_log(&file5, "LOG_WDir.txt", displayString,
					sprintf("%s", compassDirections[log_wind_direction[i]]),
					"");
		}
		Flag_SaveWD = 0;
	}

}

void new_log(FIL *fp, const char *filename, const char *content) {
	FRESULT res;
	uint32_t byteswritten;
	if (f_open(fp, filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
		Error_Handler();
	} else {
		res = f_write(fp, content, strlen(content), (void*) &byteswritten);

		if ((byteswritten == 0) || (res != FR_OK)) {
			Error_Handler();
		} else {
			f_close(fp);
		}
	}
}

void add_log(FIL *fp, const char *filename, char *data, float value,
		const char *unit) {
	FATFS_LinkDriver(&SD_Driver, SDPath);
	if (f_mount(&SDFatFS, (TCHAR const*) SDPath, 0) != FR_OK) {
		Error_Handler();
	} else {
		if (f_open(fp, filename, FA_OPEN_APPEND | FA_WRITE) != FR_OK) {
			Error_Handler();
		} else {
			f_puts("\n", fp);
			f_puts(data, fp);
			f_close(fp);
		}
	}
	FATFS_UnLinkDriver(SDPath);
}

