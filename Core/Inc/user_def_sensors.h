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
void SENSORS_Start_WDir_Conversion(void);
void SENSORS_Start_hts221_Conversion(void);
void SENSORS_Start_lps22hh_Conversion(void);
float linear_interpolation(lin_t*, int16_t);

//SENSOR hts221
void SENSOR_hts221_Init(void);
int32_t hts221_write(void*, uint8_t, const uint8_t*,uint16_t);
int32_t hts221_read(void*, uint8_t, uint8_t*,uint16_t);
void SENSOR_hts221_Read_Data(void);

//SENSOR lps22hh
void SENSOR_lps22hh_Init(void);
int32_t lps22hh_write(void*, uint8_t, const uint8_t*,uint16_t);
int32_t lps22hh_read(void*, uint8_t, uint8_t*,uint16_t);
void SENSOR_lps22hh_Read_Data(void);

//SENSOR wind speed
void SENSOR_WindSpeed_Read_Data(void);
float calculateWindSpeed(uint16_t);

//SENSOR wind direction
void SENSOR_WDir_Read_Data(void);
uint8_t determineDirection(uint16_t);

//SENSOR rainfall
void SENSOR_Rain_Read_Data(void);
float calculateRainfall(uint32_t);

#endif /* INC_USER_DEF_SENSORS_H_ */
