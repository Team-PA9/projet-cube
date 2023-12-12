#ifndef INC_USER_DEF_RTC_H_
#define INC_USER_DEF_RTC_H_

/* SDCARD includes -----------------------------------------------------------*/
#include "main.h"

/* SDCARD functions declaration ----------------------------------------------*/
void RTC_Init(void);
void RTC_Get_UTC_Timestamp(char *);
void RTC_Get_Split(uint8_t *, uint8_t *, uint8_t *,
        uint8_t *, uint8_t *, uint8_t *);
void RTC_Set(uint8_t, uint8_t, uint8_t,
             uint8_t, uint8_t, uint8_t);
uint8_t BCD_to_Dec(uint8_t);
uint8_t Dec_to_BCD(uint8_t);

#endif /* INC_USER_DEF_RTC_H_ */
