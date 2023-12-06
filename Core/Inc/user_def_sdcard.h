#ifndef INC_USER_DEF_SDCARD_H_
#define INC_USER_DEF_SDCARD_H_

/* SDCARD includes -----------------------------------------------------------*/
#include "main.h"


/* SDCARD functions declaration ----------------------------------------------*/
void SDCARD_Init(void);
void SDCARD_Actualization(void);
void new_log(FIL *fp, const char *filename, const char *content);
void add_log(FIL *fp, const char *filename, char *data, float tableau[], float value, const char *unit);

#endif /* INC_USER_DEF_SDCARD_H_ */
