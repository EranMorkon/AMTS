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

#include "piezo.h"

volatile uint32_t *PiezoSTick, PiezoSTickCur, Freq;

void init_piezo(volatile uint32_t *SysTickCnt)
{
	// Enable GPIOB clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// Create gpio struct and fill it with default values
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	// Set PB0 to output push pull
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &gpio);
	
	PiezoSTick = SysTickCnt;
	return;
}

void run_piezo()
{
	uint8_t pstate = 0x0;
	PiezoSTickCur = *PiezoSTick;
	// Run for one second
	while((*PiezoSTick - PiezoSTickCur) <= 1000)
	{
		Freq = *PiezoSTick;
		// Wait for 1 millisecond
		while((*PiezoSTick - Freq) <= 1);
		// Toggle output
		if (pstate) {
			pstate = ~pstate;
			GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET);
		}	else {
			pstate = ~pstate;
			GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
		}
	}
	return;
}

void deinit_piezo(void)
{
	return;
}
