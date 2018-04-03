#include "io.h"

uint8_t i2c1_inited = 0;
uint8_t usart1_inited = 0;
uint8_t usart3_inited = 0;

void i2c1_init(void)
{
	// If I2C1 is already inited, do nothing
	if (i2c1_inited == 1) {
		return;
	}

	// Enable GPIOB and I2C1 clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	// Create gpio struct and fill default values
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	// Set PB6 to alternate function push pull (SCL)
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &gpio);
	
	// Set PB7 to alternate function open drain (SDA)
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	gpio.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &gpio);
	
	// Set I2C1 clock to 400 kHz
	I2C_InitTypeDef i2c;
	I2C_StructInit(&i2c);
	i2c.I2C_ClockSpeed = 400000;
	I2C_Init(I2C1, &i2c);
	
	// Enable I2C1
	I2C_Cmd(I2C1, ENABLE);
	i2c1_inited = 1;
}

void usart1_init(void)
{
	// If USART1 is inited, do nothing
	if (usart1_inited == 1) {
		return;
	}

	// Enable GPIOA and USART1 clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	// Create gpio struct and fill default values
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	// Set PA9 to alternate function push pull (TxD)
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &gpio);

	// Set PA10 to input floating (RxD)
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &gpio);
	
	// Set USART1 clock to 115 200 baud
	USART_InitTypeDef usart;
	USART_StructInit(&usart);
	usart.USART_BaudRate = 115200;
	USART_Init(USART1, &usart);
	
	// Init USART1 clocks
	USART_ClockInitTypeDef usartclock;
	USART_ClockStructInit(&usartclock);
	USART_ClockInit(USART1, &usartclock);
	
	// Enable USART1
	USART_Cmd(USART1, ENABLE);
	usart1_inited = 1;
}

void usart3_init(void)
{
	// If USART3 is inited, do nothing
	if (usart3_inited == 1) {
		return;
	}

	// Enable GPIOB and USART3 clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	// Create gpio struct and fill default values
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	// Set PB10 to alternate function push pull (TxD)
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &gpio);

	// Set PB11 to input floating (RxD)
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &gpio);
	
	// Set USART3 clock to 115 200 baud
	USART_InitTypeDef usart;
	USART_StructInit(&usart);
	usart.USART_BaudRate = 115200;
	USART_Init(USART3, &usart);
	
	// Init USART3 clocks
	USART_ClockInitTypeDef usartclock;
	USART_ClockStructInit(&usartclock);
	USART_ClockInit(USART3, &usartclock);
	
	// Enable USART3
	USART_Cmd(USART3, ENABLE);
	usart3_inited = 1;
}

void USART_SendString(USART_TypeDef *USARTx, char *str)
{
	// Send a string, byte by byte over the specified USART
	while (*str) {
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		USART_SendData(USARTx, *str++);
	}
}
