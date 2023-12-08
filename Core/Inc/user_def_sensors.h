#ifndef INC_USER_DEF_SENSORS_H_
#define INC_USER_DEF_SENSORS_H_

/* SENSORS includes ----------------------------------------------------------*/
#include "main.h"


/* SENSORS structures --------------------------------------------------------*/
typedef struct {
	float x0;
	float y0;
	float x1;
	float y1;
} lin_t;


/* SENSORS functions declaration ---------------------------------------------*/
//SENSORS GEN
void tx_com(uint8_t*, uint16_t);
void platform_delay(uint32_t);

//SENSOR hts221
void SENSOR_hts221_Init(void);
int32_t hts221_write(void*, uint8_t, const uint8_t*,uint16_t);
int32_t hts221_read(void*, uint8_t, uint8_t*,uint16_t);
float linear_interpolation(lin_t*, int16_t);
void SENSOR_hts221_Start_Conversion(void);
void SENSOR_hts221_Read_Data(void);
void SENSOR_hts221_Add_Data(void);

//SENSOR lps22hh
void SENSOR_lps22hh_Init(void);
int32_t lps22hh_write(void*, uint8_t, const uint8_t*,uint16_t);
int32_t lps22hh_read(void*, uint8_t, uint8_t*,uint16_t);
void SENSOR_lps22hh_Start_Conversion(void);
void SENSOR_lps22hh_Read_Data(void);
void SENSOR_lps22hh_Add_Data(void);

//SENSOR Wind Speed
float calculateWindSpeed(uint16_t);
void SENSOR_WindSpeed_Read_Data(void);
void SENSOR_WindSpeed_Add_Data(void);

//SENSOR Wind Direction
uint8_t determineDirection(uint16_t);
void SENSOR_WDir_Start_Conversion(void);
void SENSOR_WDir_Read_Data(void);
void SENSOR_WDir_Add_Data(void);

//SENSOR Rainfall
void SENSOR_Rain_Read_Data(uint32_t *RainfallCounter);
void SENSOR_Rain_Add_Data(void);

#endif /* INC_USER_DEF_SENSORS_H_ */
