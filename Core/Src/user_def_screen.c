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
char *temperature = "12345";
char *Humidité = "12345";
char *Pression = "1234567";
char *vitesse = "12345";
uint8_t direction = 6;
char *Pluvio = "12345";

extern TS_StateTypeDef TS_State;
extern uint16_t x, y;


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

void Display_LCD_Button(int color) {
	if (color == 1) {

		if (refresh == 0) {
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "TEMPERATURE :",
					CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "HOME", RIGHT_MODE);
		}

		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) "[         'C ]", CENTER_MODE);

		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) &temperature, CENTER_MODE);

		refresh = 1;

	} else if (color == 2) {
		if (refresh == 0) {
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "HUMIDITE :",
					CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "HOME", RIGHT_MODE);
		}
		// Affichage de la mesure

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) "[         % ]", CENTER_MODE);

		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) &Humidité, CENTER_MODE);

		refresh = 1;

	} else if (color == 3) {
		if (refresh == 0) {
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "PRESSION :",
					CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "HOME", RIGHT_MODE);
		}

		//Affichage de la mesure

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) "[            Pa ]", CENTER_MODE);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) &Pression, CENTER_MODE);

		refresh = 1;
	} else if (color == 4) {
		if (refresh == 0) {
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "VITESSE :", CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "HOME", RIGHT_MODE);
		}

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) "[         m/s ]", CENTER_MODE);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) &vitesse, CENTER_MODE);
		refresh = 1;

	} else if (color == 5) {
		if (refresh == 0) {
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) " DIRECTION :",
					CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "HOME", RIGHT_MODE);
		}

		// Affichage de la mesure

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

		if (direction == 0) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ N ]", CENTER_MODE);
		} else if (direction == 1) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ NE ]", CENTER_MODE);
		} else if (direction == 2) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ E ]", CENTER_MODE);
		} else if (direction == 3) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ SE ]", CENTER_MODE);
		} else if (direction == 4) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ S ]", CENTER_MODE);
		} else if (direction == 5) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ SW ]", CENTER_MODE);
		} else if (direction == 6) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ W ]", CENTER_MODE);
		} else if (direction == 7) {
			BSP_LCD_DisplayStringAt(10,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
					(uint8_t*) "[ NW ]", CENTER_MODE);
		}
		refresh = 1;

	} else if (color == 6) {
		if (refresh == 0) {
			// Titre de la mesure

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "PLUVIOMETRIE :",
					CENTER_MODE);

			// BTN REFRESH

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
			BSP_LCD_FillCircle(30,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(60,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "REFRESH", LEFT_MODE);

			// BTN HOME

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 20);
			BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
			BSP_LCD_FillCircle(400,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 6), 18);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(100,
					BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
					(uint8_t*) "HOME", RIGHT_MODE);
		}
		// Affichage de la mesure

		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) "[         mm/H ]", CENTER_MODE);
		BSP_LCD_DisplayStringAt(10,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 2),
				(uint8_t*) &Pluvio, CENTER_MODE);
		refresh = 1;

	} else if (color == 7) {
		refresh = 1;
		if (mesure == 1) {
			//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(1);
		} else if (mesure == 2) {
			//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(2);
		} else if (mesure == 3) {
			//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(3);
		} else if (mesure == 4) {
			//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(4);
		} else if (mesure == 5) {
			//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(5);
		} else if (mesure == 6) {
			//			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
			Display_LCD_Button(6);
		}
	} else {

		// Titre

		BSP_LCD_Clear(LCD_COLOR_BLACK);
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(0, 10, (uint8_t*) "Menu Principal :",
				CENTER_MODE);
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

		// BTN P1

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize() - (3 * BSP_LCD_GetYSize() / 4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize() - (3 * BSP_LCD_GetYSize() / 4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130,
				BSP_LCD_GetYSize() - (3 * BSP_LCD_GetYSize() / 4),
				(uint8_t*) "P1:Temp", LEFT_MODE);

		// BTN P2

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize() - (2 * BSP_LCD_GetYSize() / 4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize() - (2 * BSP_LCD_GetYSize() / 4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130,
				BSP_LCD_GetYSize() - (2 * BSP_LCD_GetYSize() / 4),
				(uint8_t*) "P2:Hum ", LEFT_MODE);

		// BTN P3

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
		BSP_LCD_FillCircle(100,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(130,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
				(uint8_t*) "P3:Pres", LEFT_MODE);

		// BTN P4

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize() - (3 * BSP_LCD_GetYSize() / 4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize() - (3 * BSP_LCD_GetYSize() / 4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330,
				BSP_LCD_GetYSize() - (3 * BSP_LCD_GetYSize() / 4),
				(uint8_t*) "P4:Vit", LEFT_MODE);

		// BTN P5

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize() - (2 * BSP_LCD_GetYSize() / 4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize() - (2 * BSP_LCD_GetYSize() / 4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330,
				BSP_LCD_GetYSize() - (2 * BSP_LCD_GetYSize() / 4),
				(uint8_t*) "P5:Dir", LEFT_MODE);

		// BTN P6

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4), 20);
		BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
		BSP_LCD_FillCircle(300,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4), 18);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(330,
				BSP_LCD_GetYSize() - (1 * BSP_LCD_GetYSize() / 4),
				(uint8_t*) "P6:Pluv", LEFT_MODE);

		refresh = 0;
	}
}

void SCREEN_Actualization(void){
	BSP_TS_GetState(&TS_State);
	if (((80 < x) && (x < 120) && (y > 50) && (y < 90)
			&& (test_mes == 0))) {
		//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
		Display_LCD_Button(1);
		test_mes = 1;
		mesure = 1;
	} else if (((80 < x) && (x < 120) && (y > 120) && (y < 160)
			&& (test_mes == 0))) {
		//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
		Display_LCD_Button(2);
		test_mes = 1;
		mesure = 2;
	} else if (((80 < x) && (x < 120) && (y > 190) && (y < 230)
			&& (test_mes == 0))) {
		//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
		Display_LCD_Button(3);
		test_mes = 1;
		mesure = 3;
	} else if (((280 < x) && (x < 320) && (y > 50) && (y < 90)
			&& (test_mes == 0))) {
		//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
		Display_LCD_Button(4);
		test_mes = 1;
		mesure = 4;
	} else if (((280 < x) && (x < 320) && (y > 120) && (y < 160)
			&& (test_mes == 0))) {
		//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
		Display_LCD_Button(5);
		test_mes = 1;
		mesure = 5;
	} else if (((280 < x) && (x < 320) && (y > 190) && (y < 230)
			&& (test_mes == 0))) {
		//				  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
		Display_LCD_Button(6);
		test_mes = 1;
		mesure = 6;
	} else {
		if ((test_mes == 1) && (380 < x) && (x < 420) && (y > 190)
				&& (y < 230)) {
			//					  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 0);
			Display_LCD_Button(0);
			test_mes = 0;
			mesure = 0;
		} else if ((test_mes == 1) && (30 < x) && (x < 80) && (y > 190)
				&& (y < 230)) {
			//					  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 0);
			Display_LCD_Button(7);

			refresh = refresh + 1;
			if (mesure == 1) {
				//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				Display_LCD_Button(1);
			} else if (mesure == 2) {
				//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				Display_LCD_Button(2);
			} else if (mesure == 3) {
				//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				Display_LCD_Button(3);
			} else if (mesure == 4) {
				//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				Display_LCD_Button(4);
			} else if (mesure == 5) {
				//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				Display_LCD_Button(5);
			} else if (mesure == 6) {
				//							HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
				Display_LCD_Button(6);
			}

			test_mes = 1;

		}
	}
}
