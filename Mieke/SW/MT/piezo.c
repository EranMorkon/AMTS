#include "piezo.h"

volatile uint32_t *PiezoSTick, PiezoSTickCur, Freq;

void init_piezo(volatile uint32_t *SysTickCnt)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
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
	while((*PiezoSTick - PiezoSTickCur) <= 1000)
	{
		Freq = *PiezoSTick;
		while((*PiezoSTick - Freq) <= 1);
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
