#include "ledswitch.h"

void init_ledswitch(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOC, &gpio);
	
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_Init(GPIOA, &gpio);
	return;
}

void run_ledswitch(uint8_t *switches)
{
	*switches = (GPIO_ReadInputData(GPIOA) & 0x000F) | ((GPIO_ReadInputData(GPIOA) & 0x01E0) >> 1);
	GPIO_Write(GPIOC, ((*switches & 0xE0) << 2) | ((*switches & 0x1F) << 1));
	return;
}

void deinit_ledswitch(void)
{
	return;
}
