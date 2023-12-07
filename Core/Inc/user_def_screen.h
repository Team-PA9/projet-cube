#ifndef INC_USER_DEF_SCREEN_H_
#define INC_USER_DEF_SCREEN_H_

/* SCREEN includes -----------------------------------------------------------*/
#include "main.h"

/* SCREEN functions declaration ----------------------------------------------*/
void Display_LCD_Init(void);
void Display_LCD_ON(void);
void Display_LCD_OFF(void);
void Display_LCD_BtnRefresh(void);
void Display_LCD_BtnHome(void);
void Display_LCD_Pages(int);
void SCREEN_Actualization(void);

#endif /* INC_USER_DEF_SCREEN_H_ */
