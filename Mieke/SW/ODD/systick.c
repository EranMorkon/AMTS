#include "systick.h"

volatile uint32_t SysTickCnt;

void SysTick_Handler()
{
	SysTickCnt++;
}

void systick_init(void)
{
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	SysTick_Config(clocks.HCLK_Frequency/1000 - 1);
	
	SysTickCnt = 0;
}

void Wait(uint32_t ms)
{
	uint32_t SysTickCntHold = SysTickCnt;  
  while((SysTickCnt - SysTickCntHold) <= ms);
}
