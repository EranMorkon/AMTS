#ifndef ESP_H
#define ESP_H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
																				
void init_esp(void);
void run_esp(void);
void deinit_esp(void);

#endif
