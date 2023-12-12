#ifndef INC_USER_DEF_SCREEN_H_
#define INC_USER_DEF_SCREEN_H_

/* SCREEN includes -----------------------------------------------------------*/
#include "main.h"

/* SCREEN functions declaration ----------------------------------------------*/
void Display_LCD_Init(void);
void Display_LCD_ON(void);
void Display_LCD_OFF(void);
void Display_LCD_Btn(int, int, int, uint32_t);
void Display_LCD_BtnSave(void);
void Display_LCD_BtnHome(void);
void Display_LCD_BtnPara(void);
void Display_LCD_SavingLog(void);
void Display_LCD_Pages(int);
void Display_LCD_ModelTD(uint16_t, uint16_t);
void Display_LCD_ActuPara(uint8_t);
void Incr_Value(uint8_t*, uint8_t, uint8_t);

void TS_Actualization(void);

#endif /* INC_USER_DEF_SCREEN_H_ */
