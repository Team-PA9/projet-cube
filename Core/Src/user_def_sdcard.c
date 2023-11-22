/* SDCARD includes -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "fatfs.h"
#include "stm32746g_discovery_sdram.h"
#include "user_def_sdcard.h"

/* SDCARD variables ----------------------------------------------------------*/
FRESULT res, res2, res3, res4, res5, restest, res6; /* FatFs function common result code */
FIL *filetest, *file1, *file2, *file3, *file4, *file5; /* File read buffer */
uint32_t byteswritten, bytesread; /* File write/read counts */
uint8_t wtext[] = "LOG Temp"; /* File write buffer */
uint8_t wtext2[] = "LOG Pres";
uint8_t wtext3[] = "LOG WDir";
uint8_t wtext4[] = "LOG WSpe";
uint8_t wtext5[] = "LOG Rain";
uint8_t wtexttest[] = "asd";
uint8_t rtext[100];
uint8_t log_temperature[100];
uint8_t log_pression[100];
uint8_t log_direction[100];
uint8_t log_vitesse[100];
uint8_t log_pluviometre[100];
uint8_t workBuffer[_MAX_SS];
const TCHAR *nom_file = "test.txt";

/* SDCARD functions ----------------------------------------------------------*/
void SDCARD_Init(void) {
	if (f_mount(&SDFatFS, (TCHAR const*) SDPath, 0) != FR_OK) {
		Error_Handler(); /* FatFs Initialization Error */
	} else {
		if (f_mkfs((TCHAR const*) SDPath, FM_ANY, 0, workBuffer,
				sizeof(workBuffer)) != FR_OK) {
			Error_Handler(); /* FatFs Format Error */
		} else {
			new_log(filetest, "test.txt", "test");
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
	//		int random[10] = {48,49,50,51,52,53,54,55,56,57};
	//		for (int i = 0; i < 10; i++){
	//			char *value = random[i];
	//			log_temperature[i] = *value;
	//		}
	for (char i = 48; i < 58; i++) {
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
					if (f_open(file1, "LOG_Temp.TXT", FA_READ) != FR_OK) {
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
	if (f_mount(&SDFatFS, (TCHAR const*) SDPath, 0) != FR_OK) {
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
