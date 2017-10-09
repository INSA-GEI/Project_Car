/**
 * @file    us_sensors.c
 * @author  L.Senaneuch
 * @brief   Functions to handle ultrasonic sensors 
 */
 
/* Includes ------------------------------------------------------------------*/
#include "us_sensors.h"

uint16_t GPIO_BSRR[6] = {0x0002, 0x0004, 0x0008, 0x0010,0x0020,0x0040};
uint16_t GPIO_BRR[6] = {0x0040,0x0002, 0x0004, 0x0008, 0x0010,0x0020};

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;


void US_ClockEnable(void) {
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

}

void DMA_Configuration(void)

{
  DMA_InitTypeDef DMA_InitStructure;


  /* DMA1 Channel5 Config */
  DMA_DeInit(DMA1_Channel7);

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(GPIOC->BSRR);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)GPIO_BSRR;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 6;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

  DMA_Init(DMA1_Channel7, &DMA_InitStructure);

  /* DMA1 Channel5 enable */

  DMA_Cmd(DMA1_Channel7, ENABLE);
	
	
	  /* DMA1 Channel5 Config */
  DMA_DeInit(DMA1_Channel5);

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(GPIOC->BRR);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)GPIO_BRR;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 6;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

  DMA_Init(DMA1_Channel5, &DMA_InitStructure);

  /* DMA1 Channel5 enable */

  DMA_Cmd(DMA1_Channel5, ENABLE);
	
	

}


void US_Trigger_Init(void){
	
			TIM_OCInitTypeDef  TIM_OCInitStructure;
			GPIO_InitTypeDef GPIO_InitStructure;
			NVIC_InitTypeDef NVIC_InitStructure;

			US_ClockEnable();
								
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOC, &GPIO_InitStructure);
	
			DMA_Configuration();
				
			TIM_TimeBaseStructure.TIM_Prescaler = 110;
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
			TIM_TimeBaseStructure.TIM_Period = 65535;
			TIM_TimeBaseStructure.TIM_ClockDivision = 0;
			TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
			
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;
			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
			TIM_OCInitStructure.TIM_Pulse = 33;
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
			TIM_OC3Init(TIM4,&TIM_OCInitStructure);
			TIM_DMACmd(TIM4, TIM_DMA_CC3|TIM_DMA_Update, ENABLE);
			TIM_Cmd(TIM4, ENABLE);

}
