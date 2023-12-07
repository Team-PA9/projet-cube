/* SCREEN includes -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include "user_def_screen.h"

/* SCREEN variables ----------------------------------------------------------*/
uint8_t test_mes = 0;
uint8_t refresh = 0;
uint8_t mesure = 0;

extern float humidity_perc;
extern float temperature_degC;
extern float pressure_hPa;
extern float windspeed_kph;
extern float rainfall_mm;
extern uint8_t wind_direction;
extern char *compassDirections[];
uint8_t direction = 6;
int currentScreen = 0;

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

void Display_LCD_BtnRefresh(void) {
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillCircle(30, BSP_LCD_GetYSize() - (BSP_LCD_GetYSize()/6), 20);
	BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
	BSP_LCD_FillCircle(30, BSP_LCD_GetYSize() - (BSP_LCD_GetYSize()/6), 18);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(60,	BSP_LCD_GetYSize() - (BSP_LCD_GetYSize()/4),
			(uint8_t*) "REFRESH", LEFT_MODE);
}

void Display_LCD_BtnHome(void) {
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillCircle(400, BSP_LCD_GetYSize() - (BSP_LCD_GetYSize()/6), 20);
	BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
	BSP_LCD_FillCircle(400,	BSP_LCD_GetYSize() - (BSP_LCD_GetYSize()/6), 18);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(100, BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()/4),
			(uint8_t*) "HOME", RIGHT_MODE);
}

void Display_LCD_Pages(int color) {
	if (color == 1) {
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		char displayString[20];
		sprintf(displayString, "[ %6.2f 'C ]", temperature_degC);
		BSP_LCD_DisplayStringAt(10,	BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()/2),
				(uint8_t*) displayString, CENTER_MODE);
		if (refresh == 0) {
			currentScreen = 1;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "TEMPERATURE :",
					CENTER_MODE);
			Display_LCD_BtnRefresh();
			Display_LCD_BtnHome();
			refresh = 1;
		}
	}

	else if (color == 2) {
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		char displayString[20];
		sprintf(displayString, "[ %6.2f %% ]", humidity_perc);
		BSP_LCD_DisplayStringAt(10, BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()/2),
				(uint8_t*) displayString, CENTER_MODE);
		if (refresh == 0) {
			currentScreen = 2;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "HUMIDITE :",
					CENTER_MODE);
			Display_LCD_BtnRefresh();
			Display_LCD_BtnHome();
			refresh = 1;
		}
	}

	else if (color == 3) {
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		char displayString[20];
		sprintf(displayString, "[ %6.2f Pa ]", pressure_hPa);
		BSP_LCD_DisplayStringAt(10,	BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()/2),
				(uint8_t*) displayString, CENTER_MODE);
		if (refresh == 0) {
			currentScreen = 3;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "PRESSION :",
					CENTER_MODE);
			Display_LCD_BtnRefresh();
			Display_LCD_BtnHome();
			refresh = 1;
		}
	}

	else if (color == 4) {
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		char displayString[20];
		sprintf(displayString, "[ %6.2f m/s ]", windspeed_kph);
		BSP_LCD_DisplayStringAt(10,	BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()/2),
				(uint8_t*) displayString, CENTER_MODE);
		if (refresh == 0) {
			currentScreen = 4;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "VITESSE :",
					CENTER_MODE);
			Display_LCD_BtnRefresh();
			Display_LCD_BtnHome();
			refresh = 1;
		}
	}

	else if (color == 5) {
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		char displayString[20];
		sprintf(displayString, "[ %s ]", compassDirections[wind_direction]);
		BSP_LCD_DisplayStringAt(10,	BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()/2),
				(uint8_t*) displayString, CENTER_MODE);
		if (refresh == 0) {
			currentScreen = 5;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) " DIRECTION :",
					CENTER_MODE);
			Display_LCD_BtnRefresh();
			Display_LCD_BtnHome();
			refresh = 1;
		}
	}

	else if (color == 6) {
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		char displayString[20];
		sprintf(displayString, "[ %6.2f mm/H ]", rainfall_mm);
		BSP_LCD_DisplayStringAt(10,	BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()/2),
				(uint8_t*) displayString, CENTER_MODE);
		if (refresh == 0) {
			currentScreen = 6;
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "PLUVIOMETRIE :",
					CENTER_MODE);
			Display_LCD_BtnRefresh();
			Display_LCD_BtnHome();
			refresh = 1;
		}
	}

	else if (color == 7) {
		refresh = 1;
		if (mesure == 1) {
			Display_LCD_Pages(1);
		} else if (mesure == 2) {
			Display_LCD_Pages(2);
		} else if (mesure == 3) {
			Display_LCD_Pages(3);
		} else if (mesure == 4) {
			Display_LCD_Pages(4);
		} else if (mesure == 5) {
			Display_LCD_Pages(5);
		} else if (mesure == 6) {
			Display_LCD_Pages(6);
		}
	}

	else {
		currentScreen = 0;
		BSP_LCD_Clear(LCD_COLOR_BLACK);
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "Menu Principal :",
				CENTER_MODE);

		// BTN P1
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*3/4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*3/4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*3/4),
				(uint8_t*) "P1:Temp", LEFT_MODE);

		// BTN P2
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*2/4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*2/4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*2/4),
				(uint8_t*) "P2:Hum ", LEFT_MODE);

		// BTN P3

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*1/4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*1/4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*1/4),
				(uint8_t*) "P3:Pres", LEFT_MODE);

		// BTN P4

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*3/4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*3/4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*3/4),
				(uint8_t*) "P4:Vit", LEFT_MODE);

		// BTN P5

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*2/4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*2/4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*2/4),
				(uint8_t*) "P5:Dir", LEFT_MODE);

		// BTN P6

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*1/4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*1/4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330,
				BSP_LCD_GetYSize()-(BSP_LCD_GetYSize()*1/4),
				(uint8_t*) "P6:Pluv", LEFT_MODE);

		refresh = 0;
	}
}

void SCREEN_Actualization(void) {
	BSP_TS_GetState(&TS_State);
	if (((80 < TS_x)&&(TS_x < 120) && (TS_y > 50)&&(TS_y < 90)
			&& (test_mes == 0))) {
		Display_LCD_Pages(1);
		test_mes = 1;
		mesure = 1;
	}
	else if (((80 < TS_x)&&(TS_x < 120) && (TS_y > 120)&&(TS_y < 160)
			&& (test_mes == 0))) {
		Display_LCD_Pages(2);
		test_mes = 1;
		mesure = 2;
	}
	else if (((80 < TS_x) && (TS_x < 120) && (TS_y > 190) && (TS_y < 230)
			&& (test_mes == 0))) {
		Display_LCD_Pages(3);
		test_mes = 1;
		mesure = 3;
	}
	else if (((280 < TS_x) && (TS_x < 320) && (TS_y > 50) && (TS_y < 90)
			&& (test_mes == 0))) {
		Display_LCD_Pages(4);
		test_mes = 1;
		mesure = 4;
	}
	else if (((280 < TS_x) && (TS_x < 320) && (TS_y > 120) && (TS_y < 160)
			&& (test_mes == 0))) {
		Display_LCD_Pages(5);
		test_mes = 1;
		mesure = 5;
	}
	else if (((280 < TS_x) && (TS_x < 320) && (TS_y > 190) && (TS_y < 230)
			&& (test_mes == 0))) {
		Display_LCD_Pages(6);
		test_mes = 1;
		mesure = 6;
	}
	else {
		if ((test_mes == 1) && (380 < TS_x) && (TS_x < 420) && (TS_y > 190)
				&& (TS_y < 230)) {
			Display_LCD_Pages(0);
			test_mes = 0;
			mesure = 0;
		} else if ((test_mes == 1) && (30 < TS_x) && (TS_x < 80) && (TS_y > 190)
				&& (TS_y < 230)) {
			Display_LCD_Pages(7);

			refresh = refresh + 1;
			if (mesure == 1) {
				Display_LCD_Pages(1);
			} else if (mesure == 2) {
				Display_LCD_Pages(2);
			} else if (mesure == 3) {
				Display_LCD_Pages(3);
			} else if (mesure == 4) {
				Display_LCD_Pages(4);
			} else if (mesure == 5) {
				Display_LCD_Pages(5);
			} else if (mesure == 6) {
				Display_LCD_Pages(6);
			}

			test_mes = 1;

		}
	}
}
