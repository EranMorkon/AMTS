#ifndef INTERFACE_UART_H
#define INTERFACE_UART_H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART

void USART_SendString(USART_TypeDef *USARTx, char *str);
void init_all(void);
void send_welcome(void);
unsigned int wait_for_start(void);
unsigned int wait_for_test(USART_TypeDef *USARTx);
unsigned int get_test(USART_TypeDef *USARTx);

#endif
