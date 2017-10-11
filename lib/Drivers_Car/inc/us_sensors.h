/**
 * @file    us_sensors.h
 * @author  L.Senaneuch
 * @brief   Headers of functions to manage time
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _US_SENSORS_H
#define _US_SENSORS_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include "stm32f10x.h"
 
#define Rising_Edge 0x00 
#define Falling_Edge 0x02


#define Circular                  ((uint32_t)0x00000020)		// Circular Mode DMA
#define Calibration               ((uint32_t)0x00000000) // Normal Mode DMA

#define REAR_LEFT						0
#define REAR_CENTER					1
#define REAR_RIGHT					2
#define FRONT_LEFT					3
#define FRONT_CENTER				4
#define FRONT_RIGHT					5

#define DISTANCE_COEF       0.0263


extern uint8_t END_CALIBRATION;

 void US_QuickInit(void);
 void US_StartAcq(void);
 void US_StopAcq(void);
 
 float US_CalcDistance(void);
 


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#endif // _US_SENSORS_H
