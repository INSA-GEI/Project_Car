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
 
 
 void US_Trigger_Init(void); // private
 void US_QuickInit(void);
 void US_StartAcq(void);
 void US_StopAcq(void);
 


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#endif // _US_SENSORS_H
