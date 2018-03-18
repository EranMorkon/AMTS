#include "display.h"

#define COLORS 8

uint8_t i = 0;
char colors[COLORS][7] = {
	"RED",
	"BLUE",
	"GRAY",
	"BLACK",
	"WHITE",
	"GREEN",
	"BROWN",
	"YELLOW"
};

void init_display(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &gpio);
	
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &gpio);
	
	USART_InitTypeDef usart;
	USART_StructInit(&usart);
	usart.USART_BaudRate = 9600;
	USART_Init(USART3, &usart);
	
	USART_ClockInitTypeDef usartclock;
	USART_ClockStructInit(&usartclock);
	USART_ClockInit(USART3, &usartclock);

	USART_Cmd(USART3, ENABLE);
}

void run_display(void)
{
	char cmd[16], *cptr;
	cptr = cmd;
	sprintf(cmd, "cls %s\xFF\xFF\xFF", colors[i++]);
	if (i == COLORS) i = 0;
	while(*cptr) {
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3, *cptr++);
	}
}

void deinit_display(void)
{
	char *cmd = "rest\xFF\xFF\xFF";
	while(*cmd) {
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3, *cmd++);
	}
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_Cmd(USART3, DISABLE);
	USART_DeInit(USART3);
}
