#include <stdint.h>
#include <stm32f10x.h>

#include "manager.h"
#include "can.h"
#include "direction.h"
#include "us_sensors.h"

uint16_t test;
int main(void) {
	
	US_QuickInit();
	US_StartAcq();
  while (1){
		test=(uint16_t)US_CalcDistance();
	}
}
