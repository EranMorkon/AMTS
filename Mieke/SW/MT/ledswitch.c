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

#include "ledswitch.h"

void init_ledswitch(void)
{
	// Init GPIOA and GPIOC clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	// Create gpio struct and fill it with defaults
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	// Set the LED ports to push pull
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOC, &gpio);
	
	// Set the switch ports to input pull up
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_Init(GPIOA, &gpio);
	return;
}

void run_ledswitch(uint8_t *switches)
{
	// Read switches and shift it to represent a 8 bit number
	*switches = (GPIO_ReadInputData(GPIOA) & 0x000F) | ((GPIO_ReadInputData(GPIOA) & 0x01E0) >> 1);
	// Write that 8 bit number correctly shifted again to the LEDs
	GPIO_Write(GPIOC, ((*switches & 0xE0) << 2) | ((*switches & 0x1F) << 1));
	return;
}

void deinit_ledswitch(void)
{
	return;
}
