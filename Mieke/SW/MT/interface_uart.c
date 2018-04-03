#include "interface_uart.h"

void USART_SendString(USART_TypeDef *USARTx, char *str)
{
	// Sends a string, character for character, over the specified UART
	while (*str) {
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		USART_SendData(USARTx, *str++);
	}
}

void init_all(void)
{
	// Enable all GPIO and USART clocks needed for USART1, 2 and 3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	// Create gpio struct and fill it with defaults
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	// Set PA9 to alternate function push pull (Tx)
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &gpio);
	
	// Same for PA2
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &gpio);
	
	// Same for PB10
	gpio.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &gpio);
	
	// Set PA10 to input floating (Rx)
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &gpio);
	
	// Same for PA3
	gpio.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &gpio);
	
	// Same for PB11
	gpio.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &gpio);
	
	// Create usart struct and set baud rate to 115 200, init all three USARTs with this baud rate
	USART_InitTypeDef usart;
	USART_StructInit(&usart);
	usart.USART_BaudRate = 115200;
	USART_Init(USART1, &usart);
	USART_Init(USART2, &usart);
	USART_Init(USART3, &usart);
	
	// Enable USART clocks
	USART_ClockInitTypeDef usartclock;
	USART_ClockStructInit(&usartclock);
	USART_ClockInit(USART1, &usartclock);
	USART_ClockInit(USART2, &usartclock);
	USART_ClockInit(USART3, &usartclock);
	
	// Enable the USARTs
	USART_Cmd(USART1, ENABLE);
	USART_Cmd(USART2, ENABLE);
	USART_Cmd(USART3, ENABLE);
	
	// Enable the SysTick with T = 1 ms
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	SysTick_Config(clocks.HCLK_Frequency/1000 - 1); // SysTick T=1ms
}

void send_welcome(void)
{
	// Send a welcome message to all three USARTs
	USART_SendString(USART1, "\x1B[2J\x1B[0;0HManufacturing test software, press space...\r\n");
	USART_SendString(USART2, "\x1B[2J\x1B[0;0HManufacturing test software, press space...\r\n");
	USART_SendString(USART3, "\x1B[2J\x1B[0;0HManufacturing test software, press space...\r\n");
}

unsigned int wait_for_start(void)
{
	uint8_t data;
	// Runs until space has been pressed on one UART
	for(;;) {
		// Checks if USART1 receive register is not empty
		if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET) {
			data = USART_ReceiveData(USART1);
			// Checks if space has been pressed
			if (data == ' ') {
				// Disable the other UARTs and return 1 as this is the used UART
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
		// Checks if USART2 receive register is not empty
		if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET) {
			data = USART_ReceiveData(USART2);
			// Checks if space has been pressed
			if (data == ' ') {
				// Disable the other UARTs and return 2 as this is the used UART
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
		// Checks if USART3 receive register is not empty
		if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == SET) {
			data = USART_ReceiveData(USART3);
			// Checks if space has been pressed
			if (data == ' ') {
				// Disable the other UARTs and return 3 as this is the used UART
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
	// Blocks until something is pressed
	for(;;)
	{
		// Checks if receive register is not empty
		if (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == SET) {
			data = (int)USART_ReceiveData(USARTx);
			// If data is out of range return 1 (no test)
			if ((data > 0x09 && data <= 0x30) || data > 0x39) return 1;
			// Else return the correct test id
			return (data >= 0x30) ? data - 0x30 : data;
		}
	}
}

unsigned int get_test(USART_TypeDef *USARTx)
{
	int data;
	// Checks if receive register is not empty
	if (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == SET) {
		data = (int)USART_ReceiveData(USARTx);
		// If data is out of range return 1 (no test)
		if ((data > 0x09 && data <= 0x30) || data > 0x39) return 1;
		// Else return the correct test id 
		return (data >= 0x30) ? data - 0x30 : data;
	}
	// If nothing has been sent to the UART, return 0 (test not changed)
	return 0;
}
