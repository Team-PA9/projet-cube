/* SDCARD includes -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "fatfs.h"
#include "stm32746g_discovery_sdram.h"
#include "user_def_sdcard.h"

/* SDCARD variables ----------------------------------------------------------*/
FIL file1, file2, file3, file4, file5, file6; /* File read buffer */
uint32_t byteswritten; /* File write/read counts */
float log_temperature[100];
float log_pression[100];
float log_direction[100];
float log_vitesse[100];
float log_pluviometre[100];
float log_humidite[100];
uint8_t workBuffer[_MAX_SS];
int index_temp;
int index_pres;
int index_dir;
int index_wind;
int index_rain;
int index_humi;

extern float humidity_perc;
extern float temperature_degC;
extern float pressure_hPa;
extern float windspeed_kph;
extern float rainfall_mm;

extern uint8_t save_temp_rdy;
extern uint8_t save_pres_rdy;
extern uint8_t save_wind_rdy;
extern uint8_t save_dir_rdy;
extern uint8_t save_rain_rdy;

/* SDCARD functions ----------------------------------------------------------*/
void SDCARD_Init(void) {
	if (f_mount(&SDFatFS, (TCHAR const*) SDPath, 0) != FR_OK) {
		Error_Handler(); /* FatFs Initialization Error */
	} else {
		if (f_mkfs((TCHAR const*) SDPath, FM_ANY, 0, workBuffer,
				sizeof(workBuffer)) != FR_OK) {
			Error_Handler(); /* FatFs Format Error */
		} else {
			new_log(&file1, "LOG_Temp.txt", "Log temperature");
			new_log(&file2, "LOG_Pres.txt", "Log pressure");
			new_log(&file3, "LOG_WDir.txt", "Log wind direction");
			new_log(&file4, "LOG_WSpe.txt", "Log wind speed");
			new_log(&file5, "LOG_Rain.txt", "Log rainfall");
			new_log(&file6, "LOG_Humi.txt", "Log humidite");
		}
	}
	FATFS_UnLinkDriver(SDPath);
}

void SDCARD_Actualization(void) {
		//if (save_temp_rdy == 1) // Interruption temperature
		//{
			// Pour la temperature
			  char displayString[20];
			  sprintf(displayString, "[ %6.2f 'C ]", temperature_degC);
			  add_log(&file1, "LOG_Temp.txt", displayString, log_temperature, temperature_degC, "°C");
			  index_temp++;
			  char displayString2[20];
			// Pour l'humidite
			  sprintf(displayString2, "[ %6.2f %% ]", humidity_perc);
			  add_log(&file6, "LOG_Humi.txt", displayString2, log_humidite, humidity_perc, "%");
			  index_rain++;
			  save_temp_rdy = 0;
		//}
		if (save_pres_rdy == 1) // Interruption pression
		{
			  char displayString[20];
			  sprintf(displayString, "[ %6.2f Pa ]", pressure_hPa);
			  add_log(&file2, "LOG_Pres.txt", displayString, log_pression, pressure_hPa, "hPa");
			  index_pres++;
			  save_pres_rdy = 0;
		}
		else if (save_wind_rdy == 1) // Interruption vitesse
		{
			  char displayString[20];
			  sprintf(displayString, "[ %6.2f m/s ]", windspeed_kph);
			  add_log(&file3, "LOG_Wspe.txt", displayString, log_vitesse, windspeed_kph, "km/h");
			  index_wind++;
			  save_wind_rdy = 0;
		}
		else if (save_rain_rdy == 1) // Interruption pluviometre
		{
			  char displayString[20];
			  sprintf(displayString, "[ %6.2f mm/H ]", rainfall_mm);
			  add_log(&file5, "LOG_Rain.txt", displayString, log_pluviometre, rainfall_mm, "mm");
			  index_rain++;
			  save_rain_rdy = 0;
		}

}

void new_log(FIL *fp, const char *filename, const char *content){
	FRESULT res;
	uint32_t byteswritten;
	if(f_open(fp, filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	{
	Error_Handler();
	}
	else
	{
	res = f_write(fp, content, strlen(content), (void *)&byteswritten);

	if((byteswritten == 0) || (res != FR_OK))
	{
		Error_Handler();
	}
	else
	{
		f_close(fp);
	}
	}
}

void add_log(FIL *fp, const char *filename, char *data, float tableau[], float value, const char *unit) {
	FRESULT res;
	uint32_t byteswritten;
	FATFS_LinkDriver(&SD_Driver, SDPath);
	if (f_mount(&SDFatFS, (TCHAR
	const*) SDPath, 0) != FR_OK) {
		Error_Handler();
	} else {
		if (f_open(fp, filename, FA_OPEN_APPEND | FA_WRITE) != FR_OK) {
			Error_Handler();
		} else {
			f_puts("\n", fp);
			res = f_write(fp, data, sizeof(data), (void*) &byteswritten);
			f_puts(unit, fp);
			tableau[-1] = value;

			if ((byteswritten == 0) || (res != FR_OK)) {
				Error_Handler();
			} else {
				f_close(fp);
			}
		}
	}
	FATFS_UnLinkDriver(SDPath);
}

