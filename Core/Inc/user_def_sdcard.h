#ifndef INC_USER_DEF_SDCARD_H_
#define INC_USER_DEF_SDCARD_H_

/* SDCARD includes -----------------------------------------------------------*/
#include "main.h"


/* SDCARD functions declaration ----------------------------------------------*/
void SDCARD_Init(void);
void SDCARD_Actualization(void);
void SDCARD_NewLog(FIL*, const char*);
void SDCARD_AddLog_int(FIL*, const char*, const char*, uint8_t[], int);
void SDCARD_AddLog_dbl(FIL*, const char*, const char*, double[], int);
void SDCARD_AddLog_flt(FIL*, const char*, const char*, float[], int);

#endif /* INC_USER_DEF_SDCARD_H_ */
