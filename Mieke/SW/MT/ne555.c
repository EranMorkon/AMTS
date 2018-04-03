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

#include "ne555.h"

volatile uint32_t *NE555STick, NE555STickCur;
uint8_t old_state;

void init_ne555(volatile uint32_t *SysTickCnt)
{
	// Enable GPIOb clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// Create gpio struct and fill it with default values
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	// Set PB0 to input floating
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &gpio);
	
	NE555STick = SysTickCnt;
	return;
}

void run_ne555(float *freq)
{
	uint8_t state;
	*freq = 0.0f;
	NE555STickCur = *NE555STick;
	// Run for one second
	while((*NE555STick - NE555STickCur) <= 1000)
	{
		state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
		// If the state changes low -> high or high -> low increment freq and save state
		if (state != old_state)
		{
			(*freq)++;
			old_state = state;
		}
	}
	// Divide freq by two to get the frequency
	*freq /= 2;
	return;
}

void deinit_ne555(void)
{
	return;
}
