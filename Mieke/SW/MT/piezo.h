#ifndef PIEZO_H
#define PIEZO_H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO

void init_piezo(volatile uint32_t *SysTickCnt);
void run_piezo(void);
void deinit_piezo(void);

#endif
