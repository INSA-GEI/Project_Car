/**
 * @file    us_sensors.c
 * @author  L.Senaneuch
 * @brief   Functions to handle ultrasonic sensors 
 */
 
/* Includes ------------------------------------------------------------------*/
#include "us_sensors.h"

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;


void US_ClockEnable(void) {
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

}


void US_Trigger_Init(void){
	
			TIM_OCInitTypeDef  TIM_OCInitStructure;
			GPIO_InitTypeDef GPIO_InitStructure;

			US_ClockEnable();
						
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOC, &GPIO_InitStructure);
				
			TIM_TimeBaseStructure.TIM_Prescaler = 110;
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
			TIM_TimeBaseStructure.TIM_Period = 65535;
			TIM_TimeBaseStructure.TIM_ClockDivision = 0;
			TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
			
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;
			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
			TIM_OCInitStructure.TIM_Pulse = 13;
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
			TIM_OC3Init(TIM4,&TIM_OCInitStructure);
			TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Disable);
			
			TIM_ITConfig(TIM4, TIM_IT_Update | TIM_IT_CC3, ENABLE);

}
