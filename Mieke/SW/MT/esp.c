#include "esp.h"

void init_esp(void)
{
	// Enable GPIOa and USART2 clocks
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// Create gpio struct and fill it with default values
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	// Set PB2 to alternate function push pull (Tx)
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &gpio);
	
	// Set PB3 to input floating (Rx)
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &gpio);
	
	// Create usart struct and init USART2 to 115 200 baud
	USART_InitTypeDef usart;
	USART_StructInit(&usart);
	usart.USART_BaudRate = 115200;
	USART_Init(USART2, &usart);
	
	// Init USART2 clock
	USART_ClockInitTypeDef usartclock;
	USART_ClockStructInit(&usartclock);
	USART_ClockInit(USART2, &usartclock);
	
	// Init NVIC for USART2 RXNE to see stuff sent to the TCP server. The ISR is in main.c
	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = USART2_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&nvic);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	// Enable USART2
	USART_Cmd(USART2, ENABLE);
}

void esp_wait()
{
	// Wait some time so the ESP can execute the commands sent
	int i, j;
	for(j=0;j<500;j++) {
		for(i=0;i<65536;i++);
	}
}

void send_str(char *str) {
	// Sends a string over UART
	while(*str) {
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, *str++);
	}
}

void run_esp(void)
{
	// Restore the ESP to defaults, then wait
	send_str("AT+RESTORE\r\n");
	esp_wait();
	// Print out the current mode
	send_str("AT+CWMODE?\r\n");
	esp_wait();
	// Set mode to access point
	send_str("AT+CWMODE=2\r\n");
	esp_wait();
	// Reset
	send_str("AT+RST\r\n");
	esp_wait();
	// Enable access point
	send_str("AT+CIPMUX=1\r\n");
	esp_wait();
	// Start TCP server on port 2526
	send_str("AT+CIPSERVER=1,2526\r\n");
	esp_wait();
	// Print out the current IP address
	send_str("AT+CIPAP_CUR?\r\n");
}

void deinit_esp(void)
{
	// Disable the interrupt, then disable USART2
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART2, DISABLE);
	USART_DeInit(USART2);
}
