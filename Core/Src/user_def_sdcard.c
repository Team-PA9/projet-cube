/* SDCARD includes -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "fatfs.h"
#include "stm32746g_discovery_sdram.h"
#include "user_def_sdcard.h"

/* SDCARD variables ----------------------------------------------------------*/
FIL file1, file2, file3, file4, file5; /* File read buffer */
uint32_t byteswritten, bytesread; /* File write/read counts */
uint8_t rtext[100];
const char log_temperature[100];
const char log_pression[100];
const char log_direction[100];
const char log_vitesse[100];
const char log_pluviometre[100];
uint8_t workBuffer[_MAX_SS];
int index_temp;
int index_pres;
int index_dir;
int index_vit;
int index_rain;

/* SDCARD functions ----------------------------------------------------------*/
void SDCARD_Init(void) {
	if (f_mount(&SDFatFS, (TCHAR const*) SDPath, 0) != FR_OK) {
		Error_Handler(); /* FatFs Initialization Error */
	} else {
		if (f_mkfs((TCHAR const*) SDPath, FM_ANY, 0, workBuffer,
				sizeof(workBuffer)) != FR_OK) {
			Error_Handler(); /* FatFs Format Error */
		} else {
			new_log(file1, "LOG_Temp.txt", "Log temperature");
			new_log(file2, "LOG_Pres.txt", "Log pressure");
			new_log(file3, "LOG_WDir.txt", "Log wind direction");
			new_log(file4, "LOG_WSpe.txt", "Log wind speed");
			new_log(file5, "LOG_Rain.txt", "Log rainfall");
		}
	}
	FATFS_UnLinkDriver(SDPath);
}

void SDCARD_Actualization(void) {
		// if interruption temperature
			  add_log(&file1, "LOG_Temp.txt", 'remplacer par data', log_temperature, "°C");
			  //index_temp++
		// if interruption pression
			  add_log(&file2, "LOG_Pres.txt", 'remplacer par data', log_pression, "Pa");
			  //index_pres++
		// if interruption direction
			  add_log(&file3, "LOG_Wdir.txt", 'remplacer par data', log_direction, " ");
			  //index_dir++!
		// if interruption vitesse
			  add_log(&file4, "LOG_WSpe.txt", 'remplacer par data', log_vitesse, "km/h");
			  //index_vit++
		// if interruption pluie
			  add_log(&file5, "LOG_Rain.txt", 'remplacer par data', log_pluviometre, "mm");
			  //index_rain++
}

void new_log(FIL* fp, const char *filename, const char *content){
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

void add_log(FIL *fp, const char *filename, const char *data, const char *tableau[], const char *unit) {
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
			tableau[-1] = data;

			if ((byteswritten == 0) || (res != FR_OK)) {
				Error_Handler();
			} else {
				f_close(fp);
			}
		}
	}
	FATFS_UnLinkDriver(SDPath);
}
