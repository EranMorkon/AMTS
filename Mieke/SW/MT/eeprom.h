#ifndef EEPROM_H
#define EEPROM_H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C

#define EEPROM_ADDR (uint8_t)0xA0				// 0b10100000
												//   ----			Vendor address part
												//       ---  User address part
												//          - Keep free for R/W bit (set by I2C_Send7bitAddress())

void init_eeprom(volatile uint32_t *SysTickCnt);
void run_eeprom(uint8_t *success);
void deinit_eeprom(void);

#endif
