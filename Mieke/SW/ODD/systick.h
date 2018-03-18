#ifndef SYSTICK_H
#define SYSTICK_H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC

extern volatile uint32_t SysTickCnt;

void systick_init(void);
void Wait(uint32_t ms);

#endif
