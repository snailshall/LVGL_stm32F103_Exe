#ifndef __TIMER_H__
#define __TIMER_H__

#include "stm32f10x.h"

void TIM3_Config(uint16_t arr, uint16_t psc);
void TIM3_IRQHandler(void);

#endif // __TIMER_H__
