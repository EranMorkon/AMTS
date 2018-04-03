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
