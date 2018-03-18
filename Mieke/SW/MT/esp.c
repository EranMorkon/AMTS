#include "esp.h"

void init_esp(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &gpio);
	
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &gpio);
	
	USART_InitTypeDef usart;
	USART_StructInit(&usart);
	usart.USART_BaudRate = 115200;
	USART_Init(USART2, &usart);
	
	USART_ClockInitTypeDef usartclock;
	USART_ClockStructInit(&usartclock);
	USART_ClockInit(USART2, &usartclock);
	
	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = USART2_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&nvic);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART2, ENABLE);
}

void esp_wait()
{
	int i, j;
	for(j=0;j<500;j++) {
		for(i=0;i<65536;i++);
	}
}

void send_str(char *str) {
	while(*str) {
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, *str++);
	}
}

void run_esp(void)
{
	send_str("AT+RESTORE\r\n");
	esp_wait();
	send_str("AT+CWMODE?\r\n");
	esp_wait();
	send_str("AT+CWMODE=2\r\n");
	esp_wait();
	send_str("AT+RST\r\n");
	esp_wait();
	send_str("AT+CIPMUX=1\r\n");
	esp_wait();
	send_str("AT+CIPSERVER=1,2526\r\n");
	esp_wait();
	send_str("AT+CIPAP_CUR?\r\n");
}

void deinit_esp(void)
{
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART2, DISABLE);
	USART_DeInit(USART2);
}
