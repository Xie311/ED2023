#ifndef BSP_DELAY_H
#define BSP_DELAY_H

#include "stm32f4xx.h"
#include "tim.h"

#define TIM_Select &htim2

void delay_us(uint16_t us);

void delay_ms(uint16_t ms);

#endif // BSP_DELAY_H
