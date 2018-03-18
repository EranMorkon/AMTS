#ifndef RGB_H
#define RGB_H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C

#define RGB_ADDR (uint8_t)0x70					// 0b01110000
																				//   ----			Vendor address part
																				//       ---  User address part
																				//          - Keep free for R/W bit (set by I2C_Send7bitAddress())

void init_rgb(void);
void run_rgb(void);
void deinit_rgb(void);

#endif
