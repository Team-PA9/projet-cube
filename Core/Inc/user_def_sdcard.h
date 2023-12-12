#ifndef INC_USER_DEF_SDCARD_H_
#define INC_USER_DEF_SDCARD_H_

/* SDCARD includes -----------------------------------------------------------*/
#include "main.h"


/* SDCARD functions declaration ----------------------------------------------*/
void SDCARD_Init(void);
void SDCARD_Actualization(void);
void SDCARD_NewLog(FIL*, const char*);
void SDCARD_AddLog_WD(FIL*, const char*, const char*, uint8_t[], int);
void SDCARD_AddLog_Rf(FIL*, const char*, const char*, double[], int);
void SDCARD_AddLog_HTPWS(FIL*, const char*, const char*, float[], int);

#endif /* INC_USER_DEF_SDCARD_H_ */
