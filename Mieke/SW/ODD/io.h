#ifndef IO_H
#define IO_H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C

void i2c1_init(void);

void usart1_init(void);
void usart3_init(void);
void USART_SendString(USART_TypeDef *USARTx, char *str);

#endif
