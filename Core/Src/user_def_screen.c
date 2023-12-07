/* SCREEN includes -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include "user_def_screen.h"

/* SCREEN variables ----------------------------------------------------------*/
uint8_t FirstTimeOnScreen = 0;
uint8_t currentScreen = 0;

extern float humidity_perc;
extern float temperature_degC;
extern float pressure_hPa;
extern float windspeed_kph;
extern float rainfall_mm;
extern uint8_t wind_direction;
extern char *compassDirections[];
uint8_t direction = 6;

extern TS_StateTypeDef TS_State;
extern uint16_t TS_x, TS_y;

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
}

void Display_LCD_BtnRefresh(void) {
	Display_LCD_Btn(30, 1, 6, LCD_COLOR_BROWN);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(60, BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 4),
			(uint8_t*) "REFRESH", LEFT_MODE);
}

void Display_LCD_BtnHome(void) {
	Display_LCD_Btn(400, 1, 6, LCD_COLOR_GRAY);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(100, BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 4),
			(uint8_t*) "HOME", RIGHT_MODE);
}

void Display_LCD_Pages(int page) {
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	char displayString[20];
	switch (page) {
	case 0:
		currentScreen = 0;
		BSP_LCD_Clear(LCD_COLOR_BLACK);
		BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "Menu Principal :",
				CENTER_MODE);

		// BTN P1
		Display_LCD_Btn(100, 3, 4, LCD_COLOR_RED);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * 3 / 4),
				(uint8_t*) "P1:Temp", LEFT_MODE);

		// BTN P2
		Display_LCD_Btn(100, 2, 4, LCD_COLOR_ORANGE);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * 2 / 4),
				(uint8_t*) "P2:Hum ", LEFT_MODE);

		// BTN P3
		Display_LCD_Btn(100, 1, 4, LCD_COLOR_YELLOW);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * 1 / 4),
				(uint8_t*) "P3:Pres", LEFT_MODE);

		// BTN P4
		Display_LCD_Btn(300, 3, 4, LCD_COLOR_GREEN);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * 3 / 4),
				(uint8_t*) "P4:Vit", LEFT_MODE);

		// BTN P5
		Display_LCD_Btn(300, 2, 4, LCD_COLOR_BLUE);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * 2 / 4),
				(uint8_t*) "P5:Dir", LEFT_MODE);

		// BTN P6
		Display_LCD_Btn(300, 1, 4, LCD_COLOR_MAGENTA);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() * 1 / 4),
				(uint8_t*) "P6:Pluv", LEFT_MODE);
		break;
	case 1:
		if (FirstTimeOnScreen == 1) {
			currentScreen = 1;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "TEMPERATURE :",
					CENTER_MODE);
			Display_LCD_BtnHome();
			FirstTimeOnScreen = 0;
		}
		sprintf(displayString, "[ %6.2f 'C ]", temperature_degC);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 2),
				(uint8_t*) displayString, CENTER_MODE);
		break;
	case 2:
		if (FirstTimeOnScreen == 1) {
			currentScreen = 2;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "HUMIDITE :",
					CENTER_MODE);
			Display_LCD_BtnHome();
			FirstTimeOnScreen = 0;
		}
		printf("Actu Hum en cours");
		sprintf(displayString, "[ %6.2f %% ]", humidity_perc);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 2),
				(uint8_t*) displayString, CENTER_MODE);
		break;
	case 3:
		if (FirstTimeOnScreen == 1) {
			currentScreen = 3;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "PRESSION :",
					CENTER_MODE);
			Display_LCD_BtnHome();
			FirstTimeOnScreen = 0;
		}
		sprintf(displayString, "[ %6.2f Pa ]", pressure_hPa);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 2),
				(uint8_t*) displayString, CENTER_MODE);
		break;
	case 4:
		if (FirstTimeOnScreen == 1) {
			currentScreen = 4;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "VITESSE :", CENTER_MODE);
			Display_LCD_BtnHome();
			FirstTimeOnScreen = 0;
		}
		sprintf(displayString, "[ %6.2f m/s ]", windspeed_kph);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 2),
				(uint8_t*) displayString, CENTER_MODE);
		break;
	case 5:
		if (FirstTimeOnScreen == 1) {
			currentScreen = 5;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) " DIRECTION :",
					CENTER_MODE);
			Display_LCD_BtnHome();
			FirstTimeOnScreen = 0;
		}
		sprintf(displayString, "[ %s ]", compassDirections[wind_direction]);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 2),
				(uint8_t*) displayString, CENTER_MODE);
		break;
	case 6:
		if (FirstTimeOnScreen == 1) {
			currentScreen = 6;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "PLUVIOMETRIE :",
					CENTER_MODE);
			Display_LCD_BtnHome();
			FirstTimeOnScreen = 0;
		}
		sprintf(displayString, "[ %6.2f mm/H ]", rainfall_mm);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (BSP_LCD_GetYSize() / 2),
				(uint8_t*) displayString, CENTER_MODE);
		break;
	}
}

void TS_Actualization(void) {
	if (currentScreen == 0) { //TS for Main Screen
		FirstTimeOnScreen = 1;
		if ((80 < TS_x) && (TS_x < 120) && (TS_y > 50) && (TS_y < 90)) {
			Display_LCD_Pages(1);
		} else if ((80 < TS_x) && (TS_x < 120) && (TS_y > 120)
				&& (TS_y < 160)) {
			Display_LCD_Pages(2);
		} else if ((80 < TS_x) && (TS_x < 120) && (TS_y > 190)
				&& (TS_y < 230)) {
			Display_LCD_Pages(3);
		} else if ((280 < TS_x) && (TS_x < 320) && (TS_y > 50) && (TS_y < 90)) {
			Display_LCD_Pages(4);
		} else if ((280 < TS_x) && (TS_x < 320) && (TS_y > 120)
				&& (TS_y < 160)) {
			Display_LCD_Pages(5);
		} else if ((280 < TS_x) && (TS_x < 320) && (TS_y > 190)
				&& (TS_y < 230)) {
			Display_LCD_Pages(6);
		}
	} else { //TS for Measures Screens
		if ((380 < TS_x) && (TS_x < 420) && (TS_y > 190) && (TS_y < 230)) {
			Display_LCD_Pages(0);
		} else if ((30 < TS_x) && (TS_x < 80) && (TS_y > 190) && (TS_y < 230)) {
			//Old place for TS Refresh Button
		}
	}
}
