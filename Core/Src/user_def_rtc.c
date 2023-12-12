/* SDCARD includes -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "user_def_rtc.h"

/* RTC variables ----------------------------------------------------------*/
extern RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef sTime1;
RTC_DateTypeDef sDate1;
/* SDCARD functions ----------------------------------------------------------*/

void RTC_Init(void) {
	sTime1.Hours = 0x00;
	sTime1.Minutes = 0x00;
	sTime1.SecondFraction = 0x00;

	sDate1.Date = 0x01;
	sDate1.Month = 0x01;
	sDate1.Year = 0x23;

	if (HAL_RTC_SetTime(&hrtc, &sTime1, RTC_FORMAT_BCD) != HAL_OK) {
			Error_Handler(); /* Initialization Error */
		}

	if (HAL_RTC_SetDate(&hrtc, &sDate1, RTC_FORMAT_BCD) != HAL_OK) {
		Error_Handler(); /* Initialization Error */
	}

    // TODO: Alarm A setup
}

void RTC_Get_UTC_Timestamp(char *buffer) {
	HAL_RTC_GetTime(&hrtc, &sTime1, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &sDate1, RTC_FORMAT_BCD);

	sprintf(buffer, "%02u-%02u-%04uT%02u:%02u:%02uZ",
		BCD_to_Dec(sDate1.Date),
		BCD_to_Dec(sDate1.Month),
		BCD_to_Dec(sDate1.Year) + 2000,
		BCD_to_Dec(sTime1.Hours),
		BCD_to_Dec(sTime1.Minutes),
		BCD_to_Dec(sTime1.Seconds)
	);
}

// To be tested
void RTC_Set(uint8_t hours, uint8_t minutes, uint8_t seconds,
             uint8_t day, uint8_t month, uint8_t year) {
    sTime1.Hours = hours;
    sTime1.Minutes = minutes;
    sTime1.Seconds = seconds;

    sDate1.Date = day;
    sDate1.Month = month;
    sDate1.Year = year;

    if (HAL_RTC_SetTime(&hrtc, &sTime1, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler(); /* Set Time Error */
    }

    if (HAL_RTC_SetDate(&hrtc, &sDate1, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler(); /* Set Date Error */
    }
}

uint8_t BCD_to_Dec(uint8_t bcd_value) {
    return ((bcd_value >> 4) * 10) + (bcd_value & 0x0F);
}
