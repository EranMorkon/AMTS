/*
	Manufacturing tests for the new cortex minimal system
    Copyright (C) 2018  Andreas Mieke

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef BMA_H
#define BMA_H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C

#define BMA_ADDR (uint8_t)0x70					// 0b01110000
												//   ----			Vendor address part
												//       ---  User address part
												//          - Keep free for R/W bit (set by I2C_Send7bitAddress())
																				
void init_bma(void);
void run_bma(float acc_f[3]);
void deinit_bma(void);

#endif
