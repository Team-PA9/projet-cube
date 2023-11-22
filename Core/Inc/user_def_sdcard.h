#ifndef INC_USER_DEF_SDCARD_H_
#define INC_USER_DEF_SDCARD_H_

/* SDCARD includes -----------------------------------------------------------*/
#include "main.h"


/* SDCARD functions declaration ----------------------------------------------*/
void SDCARD_Init(void);
void SDCARD_Actualization(void);
void format(void);
void new_log(FIL *, const char *, const char *);
void add_log(FIL *, const TCHAR *, const void *);

#endif /* INC_USER_DEF_SDCARD_H_ */
