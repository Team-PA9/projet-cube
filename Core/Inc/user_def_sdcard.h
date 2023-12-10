#ifndef INC_USER_DEF_SDCARD_H_
#define INC_USER_DEF_SDCARD_H_

/* SDCARD includes -----------------------------------------------------------*/
#include "main.h"


/* SDCARD functions declaration ----------------------------------------------*/
void SDCARD_Init(void);
void SDCARD_Actualization(void);
void saving_log(void);
void new_log(FIL*, const char*);
void add_log(FIL *fp, const char *filename, const char *sensor, double values[], int index);
#endif /* INC_USER_DEF_SDCARD_H_ */
