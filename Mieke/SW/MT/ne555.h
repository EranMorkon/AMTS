#ifndef NE555_H
#define NE555_H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO

void init_ne555(volatile uint32_t *SysTickCnt);
void run_ne555(float *freq);
void deinit_ne555(void);

#endif
