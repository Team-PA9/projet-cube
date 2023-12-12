/* SCREEN includes -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include "user_def_screen.h"

/* SCREEN variables ----------------------------------------------------------*/
uint8_t firstTimeOnScreen = 0;
uint8_t currentScreen = 0;
extern uint16_t TS_x, TS_y;

extern int index_HT;
extern int index_Pr;
extern int index_WS;
extern int index_WD;
extern int index_Rf;

extern float log_humidity[];
extern float log_temperature[];
extern float log_pressure[];
extern float log_windspeed[];
extern uint8_t log_wind_direction[];
extern double log_rainfall[];

extern char *compassDirections[];

uint8_t RTC_hour = 5;
uint8_t RTC_minute = 16;
uint8_t RTC_second = 4;
uint8_t RTC_day = 28;
uint8_t RTC_month = 5;
uint8_t RTC_year = 19;
char *Month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep",
		"Oct", "Nov", "Dec" };

/* SCREEN functions ----------------------------------------------------------*/
void Display_LCD_Init(void) {
	BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
	BSP_LCD_SetLayerVisible(LTDC_ACTIVE_LAYER, ENABLE);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "Menu Principal :", CENTER_MODE);
}

void Display_LCD_ON(void) {
	BSP_LCD_DisplayOn();
	HAL_GPIO_WritePin(GLED_GPIO_Port, GLED_Pin, 1);
	HAL_GPIO_WritePin(RLED_GPIO_Port, RLED_Pin, 0);
}

void Display_LCD_OFF(void) {
	BSP_LCD_DisplayOff();
	HAL_GPIO_WritePin(GLED_GPIO_Port, GLED_Pin, 0);
	HAL_GPIO_WritePin(RLED_GPIO_Port, RLED_Pin, 1);
}

void Display_LCD_Btn(int X, int Y, int Z, uint32_t color) {
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillCircle(X, BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * Y / Z),
			20);
	BSP_LCD_SetTextColor(color);
	BSP_LCD_FillCircle(X, BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * Y / Z),
			18);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
}

void Display_LCD_BtnSave(void) {
	BSP_LCD_DisplayStringAt(395,
			BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * 2 / 5),
			(uint8_t*) "SAVE", LEFT_MODE);
	Display_LCD_Btn(425, 1, 2, LCD_COLOR_GREEN);
}

void Display_LCD_BtnHome(void) {
	BSP_LCD_DisplayStringAt(110, BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 5),
			(uint8_t*) "HOME", RIGHT_MODE);
	Display_LCD_Btn(400, 1, 6, LCD_COLOR_GRAY);
}

void Display_LCD_BtnPara(void) {
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DrawRect(30, 10, 20, 20);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);
	BSP_LCD_FillRect(31, 11, 19, 19);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
}

void Display_LCD_ModelTD(uint16_t TS_x, uint16_t TS_y) {
	BSP_LCD_FillCircle(TS_x, (TS_y + 82), 14);
	BSP_LCD_DrawRect((TS_x - 40), (TS_y + 20), 80, 40);
}

void Display_LCD_SavingLog(void) {
	BSP_LCD_SetLayerVisible(LTDC_LAYER_2, ENABLE);
	BSP_LCD_SelectLayer(LTDC_LAYER_2);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(0, 250, (uint8_t*) "Saving...", LEFT_MODE);
}

void Display_LCD_Pages(int page) {
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	char displayString[20];
	switch (page) {
	case 0:
		currentScreen = 0;
		BSP_LCD_Clear(LCD_COLOR_BLACK);
		//Display all titles
		BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "Menu Principal :",
				CENTER_MODE);
		BSP_LCD_DisplayStringAt(110,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * 3 / 4),
				(uint8_t*) "P1:Temp", LEFT_MODE);
		BSP_LCD_DisplayStringAt(110,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * 2 / 4),
				(uint8_t*) "P2:Hum", LEFT_MODE);
		BSP_LCD_DisplayStringAt(110,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * 1 / 4),
				(uint8_t*) "P3:Pres", LEFT_MODE);
		BSP_LCD_DisplayStringAt(310,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * 3 / 4),
				(uint8_t*) "P4:Vit", LEFT_MODE);
		BSP_LCD_DisplayStringAt(310,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * 2 / 4),
				(uint8_t*) "P5:Dir", LEFT_MODE);
		BSP_LCD_DisplayStringAt(310,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * 1 / 4),
				(uint8_t*) "P6:Pluv", LEFT_MODE);
		//Display all buttons
		Display_LCD_Btn(80, 29, 40, LCD_COLOR_RED);
		Display_LCD_Btn(80, 19, 40, LCD_COLOR_ORANGE);
		Display_LCD_Btn(80, 9, 40, LCD_COLOR_YELLOW);
		Display_LCD_Btn(280, 29, 40, LCD_COLOR_GREEN);
		Display_LCD_Btn(280, 19, 40, LCD_COLOR_BLUE);
		Display_LCD_Btn(280, 9, 40, LCD_COLOR_MAGENTA);
		Display_LCD_BtnPara();
		break;
	case 1:
		if (firstTimeOnScreen == 1) {
			currentScreen = 1;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "TEMPERATURE :",
					CENTER_MODE);
			Display_LCD_BtnHome();
			firstTimeOnScreen = 0;
		}
		sprintf(displayString, "[ %6.2f 'C ]", log_temperature[index_HT - 1]);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 2),
				(uint8_t*) displayString, CENTER_MODE);
		break;
	case 2:
		if (firstTimeOnScreen == 1) {
			currentScreen = 2;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "HUMIDITE :",
					CENTER_MODE);
			Display_LCD_BtnHome();
			firstTimeOnScreen = 0;
		}
		sprintf(displayString, "[ %6.2f %% ]", log_humidity[index_HT - 1]);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 2),
				(uint8_t*) displayString, CENTER_MODE);
		break;
	case 3:
		if (firstTimeOnScreen == 1) {
			currentScreen = 3;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "PRESSION :",
					CENTER_MODE);
			Display_LCD_BtnHome();
			firstTimeOnScreen = 0;
		}
		sprintf(displayString, "[ %6.2f hPa ]", log_pressure[index_Pr - 1]);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 2),
				(uint8_t*) displayString, CENTER_MODE);
		break;
	case 4:
		if (firstTimeOnScreen == 1) {
			currentScreen = 4;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "VITESSE :", CENTER_MODE);
			Display_LCD_BtnHome();
			firstTimeOnScreen = 0;
		}
		sprintf(displayString, "[ %6.2f km/h ]", log_windspeed[index_WS - 1]);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 2),
				(uint8_t*) displayString, CENTER_MODE);
		break;
	case 5:
		if (firstTimeOnScreen == 1) {
			currentScreen = 5;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) " DIRECTION :",
					CENTER_MODE);
			Display_LCD_BtnHome();
			firstTimeOnScreen = 0;
		}
		sprintf(displayString, "[ %s ]",
				compassDirections[log_wind_direction[index_WD - 1]]);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 2),
				(uint8_t*) displayString, CENTER_MODE);
		break;
	case 6:
		if (firstTimeOnScreen == 1) {
			currentScreen = 6;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "PLUVIOMETRIE :",
					CENTER_MODE);
			Display_LCD_BtnHome();
			firstTimeOnScreen = 0;
		}
		sprintf(displayString, "[ %6.2f mm/h ]", log_rainfall[index_Rf - 1]);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 2),
				(uint8_t*) displayString, CENTER_MODE);
		break;
	case 10:
		if (firstTimeOnScreen == 1) {
			currentScreen = 10;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "Time & Date settings :",
					CENTER_MODE);
			Display_LCD_ModelTD(85, 60);
			Display_LCD_ModelTD(195, 60);
			Display_LCD_ModelTD(305, 60);
			Display_LCD_ModelTD(85, 160);
			Display_LCD_ModelTD(195, 160);
			Display_LCD_ModelTD(305, 160);
			Display_LCD_BtnSave();
			firstTimeOnScreen = 0;
		}
		Display_LCD_ActuPara(0);
		Display_LCD_ActuPara(1);
		Display_LCD_ActuPara(2);
		Display_LCD_ActuPara(3);
		Display_LCD_ActuPara(4);
		Display_LCD_ActuPara(5);
		break;
	}
}

void Display_LCD_ActuPara(uint8_t type) {
	char displayString[10];
	switch (type) {
	case 0:
		sprintf(displayString, "%02d", RTC_hour);
		BSP_LCD_DisplayStringAt(65, 90, (uint8_t*) displayString, LEFT_MODE);
		break;
	case 1:
		sprintf(displayString, "%02d", RTC_minute);
		BSP_LCD_DisplayStringAt(175, 90, (uint8_t*) displayString, LEFT_MODE);
		break;
	case 2:
		sprintf(displayString, "%02d", RTC_second);
		BSP_LCD_DisplayStringAt(285, 90, (uint8_t*) displayString, LEFT_MODE);
		break;
	case 3:
		sprintf(displayString, "%02d", RTC_day);
		BSP_LCD_DisplayStringAt(65, 190, (uint8_t*) displayString, LEFT_MODE);
		break;
	case 4:
		sprintf(displayString, "%s", Month[RTC_month - 1]);
		BSP_LCD_DisplayStringAt(165, 190, (uint8_t*) displayString, LEFT_MODE);
		break;
	case 5:
		sprintf(displayString, "20%02d", RTC_year);
		BSP_LCD_DisplayStringAt(273, 190, (uint8_t*) displayString, LEFT_MODE);
		break;
	}
}

void Incr_Value(uint8_t *value, uint8_t min, uint8_t max) {
	if (*value >= max) {
		*value = min;
	} else {
		*value = *value + 1;
	}
	printf("Incr : %d \r\n", (int) *value);
}

void TS_Actualization(void) {
	if (currentScreen == 0) { //TS for Main Screen
		firstTimeOnScreen = 1;
		if ((50 < TS_x) && (TS_x < 110) && (TS_y > 42) && (TS_y < 102)) {
			Display_LCD_Pages(1);
		} else if ((50 < TS_x) && (TS_x < 110) && (TS_y > 122)
				&& (TS_y < 182)) {
			Display_LCD_Pages(2);
		} else if ((50 < TS_x) && (TS_x < 110) && (TS_y > 202)
				&& (TS_y < 262)) {
			Display_LCD_Pages(3);
		} else if ((250 < TS_x) && (TS_x < 310) && (TS_y > 42)
				&& (TS_y < 102)) {
			Display_LCD_Pages(4);
		} else if ((250 < TS_x) && (TS_x < 310) && (TS_y > 122)
				&& (TS_y < 182)) {
			Display_LCD_Pages(5);
		} else if ((250 < TS_x) && (TS_x < 310) && (TS_y > 202)
				&& (TS_y < 262)) {
			Display_LCD_Pages(6);
		} else if ((25 < TS_x) && (TS_x < 55) && (TS_y > 5) && (TS_y < 35)) {
			Display_LCD_Pages(10);
		}
	} else if (currentScreen == 10) { //TS for Time & Date Parameters
		if ((410 < TS_x) && (TS_x < 450) && (TS_y > 108) && (TS_y < 148)) {
			Display_LCD_Pages(0);
		} else if ((75 < TS_x) && (TS_x < 95) && (TS_y > 125) && (TS_y < 145)) {
			Incr_Value(&RTC_hour, 0, 23);
			Display_LCD_ActuPara(0);
		} else if ((185 < TS_x) && (TS_x < 205) && (TS_y > 125)
				&& (TS_y < 145)) {
			Incr_Value(&RTC_minute, 0, 59);
			Display_LCD_ActuPara(1);
		} else if ((295 < TS_x) && (TS_x < 315) && (TS_y > 125)
				&& (TS_y < 145)) {
			Incr_Value(&RTC_second, 0, 59);
			Display_LCD_ActuPara(2);
		} else if ((75 < TS_x) && (TS_x < 95) && (TS_y > 225) && (TS_y < 245)) {
			Incr_Value(&RTC_day, 1, 31);
			Display_LCD_ActuPara(3);
		} else if ((185 < TS_x) && (TS_x < 205) && (TS_y > 225)
				&& (TS_y < 245)) {
			Incr_Value(&RTC_month, 1, 12);
			Display_LCD_ActuPara(4);
		} else if ((295 < TS_x) && (TS_x < 315) && (TS_y > 225)
				&& (TS_y < 245)) {
			Incr_Value(&RTC_year, 0, 99);
			Display_LCD_ActuPara(5);
		}
	} else { //TS for Measures Screens
		if ((370 < TS_x) && (TS_x < 430) && (TS_y > 206) && (TS_y < 266)) {
			Display_LCD_Pages(0);
		}
	}
}
