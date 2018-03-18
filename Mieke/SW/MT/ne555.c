#include "ne555.h"

volatile uint32_t *NE555STick, NE555STickCur;
uint8_t old_state;

void init_ne555(volatile uint32_t *SysTickCnt)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
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
	while((*NE555STick - NE555STickCur) <= 1000)
	{
		state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
		if (state != old_state)
		{
			(*freq)++;
			old_state = state;
		}
	}
	*freq /= 2;
	return;
}

void deinit_ne555(void)
{
	return;
}
