#include "io.h"

uint8_t i2c1_inited = 0;
uint8_t usart1_inited = 0;
uint8_t usart3_inited = 0;

void i2c1_init(void)
{
	if (i2c1_inited == 1) {
		return;
	}
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	// SCL
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &gpio);
	
	// SDA
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	gpio.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &gpio);
	
	I2C_InitTypeDef i2c;
	I2C_StructInit(&i2c);
	i2c.I2C_ClockSpeed = 400000;
	I2C_Init(I2C1, &i2c);
	
	I2C_Cmd(I2C1, ENABLE);
	i2c1_inited = 1;
}

void usart1_init(void)
{
	if (usart1_inited == 1) {
		return;
	}
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	// TxD
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &gpio);

	// RxD
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &gpio);
	
	USART_InitTypeDef usart;
	USART_StructInit(&usart);
	usart.USART_BaudRate = 115200;
	USART_Init(USART1, &usart);
	
	USART_ClockInitTypeDef usartclock;
	USART_ClockStructInit(&usartclock);
	USART_ClockInit(USART1, &usartclock);
	
	USART_Cmd(USART1, ENABLE);
	usart1_inited = 1;
}

void usart3_init(void)
{
	if (usart3_inited == 1) {
		return;
	}
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	// TxD
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &gpio);

	// RxD
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &gpio);
	
	USART_InitTypeDef usart;
	USART_StructInit(&usart);
	usart.USART_BaudRate = 115200;
	USART_Init(USART3, &usart);
	
	USART_ClockInitTypeDef usartclock;
	USART_ClockStructInit(&usartclock);
	USART_ClockInit(USART3, &usartclock);
	
	USART_Cmd(USART3, ENABLE);
	usart3_inited = 1;
}

void USART_SendString(USART_TypeDef *USARTx, char *str)
{
	while (*str) {
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		USART_SendData(USARTx, *str++);
	}
}
