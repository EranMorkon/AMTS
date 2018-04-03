/*
	Demo program for the 2017/18 open door day at HTL Hollabrunn
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

#include "systick.h"

volatile uint32_t SysTickCnt;

void SysTick_Handler()
{
	// SysTick_Handler increments SysTickCnt
	SysTickCnt++;
}

void systick_init(void)
{
	// Init the systick clocks to a T = 1 ms
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	SysTick_Config(clocks.HCLK_Frequency/1000 - 1);
	
	// Set count to 0
	SysTickCnt = 0;
}

void Wait(uint32_t ms)
{
	// Wait specified time in milliseconds
	uint32_t SysTickCntHold = SysTickCnt;  
	while((SysTickCnt - SysTickCntHold) <= ms);
}
