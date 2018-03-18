#ifndef DISPLAY_H
#define DISPLAY_H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART

#include <stdio.h>
																				
void init_display(void);
void run_display(void);
void deinit_display(void);

#endif
