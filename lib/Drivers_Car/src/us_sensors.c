/**
 * @file    us_sensors.c
 * @author  L.Senaneuch
 * @brief   Functions to handle ultrasonic sensors 
 */
 
/* Includes ------------------------------------------------------------------*/
#include "us_sensors.h"
#include "common_constants.h"

/*Data which will be set GPIO to trig US_Sensors*/
uint16_t GPIO_BSRR[2] = {(1<<1)|(1<<3)|(1<<5), (1<<2)|(1<<4)|(1<<7)};
uint16_t GPIO_BRR[2] = {(1<<2)|(1<<4)|(1<<7), (1<<1)|(1<<3)|(1<<5)};

/*Data */
/*
uint16_t RISING_EDGE[6];
uint16_t FALLING_EDGE[6];
*/
struct sensor sensors[6];


uint16_t SENSORS_LEFT_DATA[6];
uint16_t SENSORS_RIGHT_DATA[6];
uint16_t SENSORS_CENTER_DATA[6];


uint8_t EoCalibrationL = 0;
uint8_t EoCalibrationR = 0;
uint8_t EoCalibrationC = 0;

/* Local Function*/

void US_ClockEnable(void);
void DMA_Configuration_Trig(void);
void US_NVIC_Configure(void);
void DMA_Configuration_Capture(void);
void US_GPIO_Configure(void);
void US_TIM4_Trig_Init(void);
void US_TIM3_Echo_Init(uint8_t edge);
void US_StartAcq(void);
void US_StopAcq(void);


/* Local Variable*/
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_ICInitTypeDef  TIM_ICInitStructure;
/* Generate signal to enable us_sensor. Every 100ms, 3 sensors are going to be enabled.
 * First signal is going to be set for the front left, front right, and rear center sensors. |PC1, PC5, PC3|
 * Then rear left, rear right and front center senors are going to be triggered. |PC4, PC2,PC7|
 */


void US_ClockEnable(void) {
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void US_GPIO_Configure(void){
			/* GPIO Signal Triger*/
			GPIO_InitTypeDef GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOC, &GPIO_InitStructure);
	
			/*GPIO Echo*/
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_9;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOC, &GPIO_InitStructure);
			
			/*Remap TIM3*/
			GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
}
void US_NVIC_Configure(void){
	
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
	NVIC_Init(&NVIC_InitStructure);

}

void DMA_Configuration_Trig(void){
  DMA_InitTypeDef DMA_InitStructure;


  /* DMA1 Channel7 Config */
  DMA_DeInit(DMA1_Channel7);

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(GPIOC->BSRR);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)GPIO_BSRR;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 2;
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

  DMA_Init(DMA1_Channel5, &DMA_InitStructure);

  /* DMA1 Channel5 enable */

  DMA_Cmd(DMA1_Channel5, ENABLE);
	
	

}

void DMA_Configuration_Capture(void){
  DMA_InitTypeDef DMA_InitStructure;	
		
  /* DMA1 Channel2 Config TIM3 CH3 */
  DMA_DeInit(DMA1_Channel2);
	
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(TIM3->CNT);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SENSORS_LEFT_DATA;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 6;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

  DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
  /* DMA1 Channel2 enable */
  DMA_Cmd(DMA1_Channel2, ENABLE);
	
	
	
	  /* DMA1 Channel3 Config */
  DMA_DeInit(DMA1_Channel3);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SENSORS_CENTER_DATA;
	
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
  /* DMA1 Channel3 enable */
  DMA_Cmd(DMA1_Channel3, ENABLE);
	
	
	
	/* DMA1 Channel6 Config */
  DMA_DeInit(DMA1_Channel6);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SENSORS_RIGHT_DATA;
  
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);
  /* DMA1 Channel6 enable */
  DMA_Cmd(DMA1_Channel6, ENABLE);
	

}




void US_TIM4_Trig_Init(void){
	
			TIM_OCInitTypeDef  TIM_OCInitStructure;
			
			US_ClockEnable();
			US_GPIO_Configure();
			DMA_Configuration_Trig();
				
			TIM_TimeBaseStructure.TIM_Prescaler = 109;
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
			

}

void US_TIM3_Echo_Init(uint8_t edge){
			TIM_TimeBaseStructure.TIM_Prescaler = 109;
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
			TIM_TimeBaseStructure.TIM_Period = 65535;
			TIM_TimeBaseStructure.TIM_ClockDivision = 0;
			TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
			TIM_ICInitStructure.TIM_ICPolarity = edge;
			TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
			TIM_ICInitStructure.TIM_ICPrescaler = 0;
			TIM_ICInitStructure.TIM_ICFilter = 0x0;
			TIM_ICInit(TIM3, &TIM_ICInitStructure);

			TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
			TIM_ICInitStructure.TIM_ICPolarity = edge;
			TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
			TIM_ICInitStructure.TIM_ICPrescaler = 0;
			TIM_ICInitStructure.TIM_ICFilter = 0x0;
			TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
			
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
			TIM_ICInitStructure.TIM_ICPolarity = edge;
			TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
			TIM_ICInitStructure.TIM_ICPrescaler = 0;
			TIM_ICInitStructure.TIM_ICFilter = 0x0;
			TIM_ICInit(TIM3, &TIM_ICInitStructure);
			
			TIM_DMACmd(TIM3, TIM_DMA_CC3|TIM_DMA_CC4|TIM_DMA_CC1, ENABLE);	
}

float US_CalcDistance(uint8_t sensor_position){
	 return (sensors[sensor_position].COUNTER_DIFF * DISTANCE_COEF);
	 
}



void US_QuickInit(void){
	  EoCalibrationL = 0;
	  EoCalibrationR = 0;
	  EoCalibrationC = 0;
	  US_ClockEnable();
		US_GPIO_Configure();
		US_NVIC_Configure();
	  DMA_Configuration_Trig();
		DMA_Configuration_Capture();
		US_TIM4_Trig_Init();
		US_TIM3_Echo_Init(Falling_Edge);
}


/* If StartAcq is the first one after Init. Sensors will jump into a calibration mode to get rising edge counter value in RISING_EDGE array.
 * Then you can read distance with function US_CalcDistance(); 
 */
void US_StartAcq(void){
		 TIM_Cmd(TIM3, ENABLE);	
		 TIM_Cmd(TIM4, ENABLE);
}


void US_StopAcq(void){
		 TIM_Cmd(TIM3, DISABLE);
		 TIM_Cmd(TIM4, DISABLE);
}




/*Handler for right sensors*/
void DMA1_Channel6_IRQHandler(void){  
	DMA_InitTypeDef DMA_InitStructure; 
	if(EoCalibrationR == 0){
		DMA_Cmd(DMA1_Channel6, DISABLE);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(TIM3->CNT);
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SENSORS_RIGHT_DATA;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = 2;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		
		DMA_Init(DMA1_Channel6, &DMA_InitStructure);
		DMA_Cmd(DMA1_Channel6, ENABLE);
		
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
		TIM_ICInitStructure.TIM_ICPolarity = Rising_Edge;
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
		TIM_ICInitStructure.TIM_ICPrescaler = 0;
		TIM_ICInitStructure.TIM_ICFilter = 0x0;
		TIM_ICInit(TIM3, &TIM_ICInitStructure);
	 
		sensors[FRONT_RIGHT].RISING_EDGE = SENSORS_RIGHT_DATA[4];
		sensors[REAR_RIGHT].RISING_EDGE = SENSORS_RIGHT_DATA[5];
			
	  EoCalibrationR =1;
		
	}
	else{
		sensors[FRONT_RIGHT].FALLING_EDGE = SENSORS_RIGHT_DATA[0];
		sensors[REAR_RIGHT].FALLING_EDGE = SENSORS_RIGHT_DATA[1];
	
		sensors[FRONT_RIGHT].COUNTER_DIFF = sensors[FRONT_RIGHT].FALLING_EDGE - sensors[FRONT_RIGHT].RISING_EDGE;
		sensors[REAR_RIGHT].COUNTER_DIFF = sensors[REAR_RIGHT].FALLING_EDGE - sensors[REAR_RIGHT].RISING_EDGE;
	}
	
	DMA_ClearITPendingBit(DMA1_IT_GL6);
}

/*Handler for Left sensors*/
void DMA1_Channel2_IRQHandler(void){
   DMA_InitTypeDef DMA_InitStructure;
	
	 if(EoCalibrationL == 0){
		 DMA_Cmd(DMA1_Channel2, DISABLE);
		 DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(TIM3->CNT);
		 DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SENSORS_LEFT_DATA;
	   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	   DMA_InitStructure.DMA_BufferSize = 2;
	   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		 DMA_Init(DMA1_Channel2, &DMA_InitStructure);

	   DMA_Cmd(DMA1_Channel2, ENABLE);
	 
	   TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	   TIM_ICInitStructure.TIM_ICPolarity = Rising_Edge;
	   TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	   TIM_ICInitStructure.TIM_ICPrescaler = 0;
	   TIM_ICInitStructure.TIM_ICFilter = 0x0;
	   TIM_ICInit(TIM3, &TIM_ICInitStructure);
	 
	   sensors[FRONT_LEFT].RISING_EDGE = SENSORS_LEFT_DATA[4];
	   sensors[REAR_LEFT].RISING_EDGE = SENSORS_LEFT_DATA[5];
		 EoCalibrationL=1;
	 }
	 else{
		 sensors[FRONT_LEFT].FALLING_EDGE = SENSORS_LEFT_DATA[0];
	   sensors[REAR_LEFT].FALLING_EDGE = SENSORS_LEFT_DATA[1];
	 
		 sensors[FRONT_LEFT].COUNTER_DIFF = sensors[FRONT_LEFT].FALLING_EDGE - sensors[FRONT_LEFT].RISING_EDGE;
		 sensors[REAR_LEFT].COUNTER_DIFF = sensors[REAR_LEFT].FALLING_EDGE - sensors[REAR_LEFT].RISING_EDGE;
	 }
	
	 
	 DMA_ClearITPendingBit(DMA1_IT_GL2);
}


/*Handler for center sensors*/
void DMA1_Channel3_IRQHandler(void){
	 DMA_InitTypeDef DMA_InitStructure;
	 if(EoCalibrationC == 0){
		 DMA_Cmd(DMA1_Channel3, DISABLE);
		 DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(TIM3->CNT);
		 DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SENSORS_CENTER_DATA;
	   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	   DMA_InitStructure.DMA_BufferSize = 2;
	   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	   DMA_Cmd(DMA1_Channel3, ENABLE);
	   DMA_Init(DMA1_Channel3, &DMA_InitStructure);
		
  	 TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	   TIM_ICInitStructure.TIM_ICPolarity = Rising_Edge;
	   TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	   TIM_ICInitStructure.TIM_ICPrescaler = 0;
	   TIM_ICInitStructure.TIM_ICFilter = 0x0;
	   TIM_ICInit(TIM3, &TIM_ICInitStructure);
		 
	   sensors[FRONT_CENTER].RISING_EDGE = SENSORS_CENTER_DATA[4];
	   sensors[REAR_CENTER].RISING_EDGE = SENSORS_CENTER_DATA[5];
		 EoCalibrationC =1;
	 }
	 else{
		 sensors[FRONT_CENTER].FALLING_EDGE = SENSORS_CENTER_DATA[1];
	   sensors[REAR_CENTER].FALLING_EDGE = SENSORS_CENTER_DATA[0];

		 sensors[FRONT_CENTER].COUNTER_DIFF = sensors[FRONT_CENTER].FALLING_EDGE - sensors[FRONT_CENTER].RISING_EDGE;
		 sensors[REAR_CENTER].COUNTER_DIFF = sensors[REAR_CENTER].FALLING_EDGE - sensors[REAR_CENTER].RISING_EDGE;
	 }
	 
	
	 DMA_ClearITPendingBit(DMA1_IT_GL3);
}



/*
 * Function Main
 * 1 - Enable Clock
 * 2 - Configure GPIO
 * 3 - Configure DMA
 * 4 - Configure Timer
 * 5 - Launch Timer
 */
