#ifndef LEDSWITCH_H
#define LEDSWITCH_H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO

void init_ledswitch(void);
void run_ledswitch(uint8_t *switches);
void deinit_ledswitch(void);

#endif
