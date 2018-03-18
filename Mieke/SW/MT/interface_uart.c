#include "interface_uart.h"

void USART_SendString(USART_TypeDef *USARTx, char *str)
{
	while (*str) {
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		USART_SendData(USARTx, *str++);
	}
}

void init_all(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &gpio);
	
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &gpio);
	
	USART_InitTypeDef usart;
	USART_StructInit(&usart);
	usart.USART_BaudRate = 115200;
	USART_Init(USART1, &usart);
	USART_Init(USART2, &usart);
	USART_Init(USART3, &usart);
	
	USART_ClockInitTypeDef usartclock;
	USART_ClockStructInit(&usartclock);
	USART_ClockInit(USART1, &usartclock);
	USART_ClockInit(USART2, &usartclock);
	USART_ClockInit(USART3, &usartclock);
	
	USART_Cmd(USART1, ENABLE);
	USART_Cmd(USART2, ENABLE);
	USART_Cmd(USART3, ENABLE);
	
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	SysTick_Config(clocks.HCLK_Frequency/1000 - 1); // SysTick T=1ms
}

void send_welcome(void)
{
	USART_SendString(USART1, "\x1B[2J\x1B[0;0HManufacturing test software, press space...\r\n");
	USART_SendString(USART2, "\x1B[2J\x1B[0;0HManufacturing test software, press space...\r\n");
	USART_SendString(USART3, "\x1B[2J\x1B[0;0HManufacturing test software, press space...\r\n");
}

unsigned int wait_for_start(void)
{
	uint8_t data;
	for(;;) {
		if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET) {
			data = USART_ReceiveData(USART1);
			if (data == ' ') {
				USART_Cmd(USART2, DISABLE);
				USART_Cmd(USART3, DISABLE);
				USART_DeInit(USART2);
				USART_DeInit(USART3);
				return 1;
			} else {
				USART_Cmd(USART1, DISABLE);
				USART_DeInit(USART1);
			}
		}
		if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET) {
			data = USART_ReceiveData(USART2);
			if (data == ' ') {
				USART_Cmd(USART1, DISABLE);
				USART_Cmd(USART3, DISABLE);
				USART_DeInit(USART1);
				USART_DeInit(USART3);
				return 2;
			} else {
				USART_Cmd(USART2, DISABLE);
				USART_DeInit(USART2);
			}
		}
		if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == SET) {
			data = USART_ReceiveData(USART3);
			if (data == ' ') {
				USART_Cmd(USART1, DISABLE);
				USART_Cmd(USART2, DISABLE);
				USART_DeInit(USART1);
				USART_DeInit(USART2);
				return 3;
			} else {
				USART_Cmd(USART3, DISABLE);
				USART_DeInit(USART3);
			}
		}
	}
}

unsigned int wait_for_test(USART_TypeDef *USARTx)
{
	int data;
	for(;;)
	{
		if (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == SET) {
			data = (int)USART_ReceiveData(USARTx);
			if ((data > 0x09 && data <= 0x30) || data > 0x39) return 1; 
			return (data >= 0x30) ? data - 0x30 : data;
		}
	}
}

unsigned int get_test(USART_TypeDef *USARTx)
{
	int data;
	if (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == SET) {
		data = (int)USART_ReceiveData(USARTx);
		if ((data > 0x09 && data <= 0x30) || data > 0x39) return 1; 
		return (data >= 0x30) ? data - 0x30 : data;
	}
	return 0;
}
