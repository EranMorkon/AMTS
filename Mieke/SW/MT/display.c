/*
	Manufacturing tests for the new cortex minimal system
    Copyright (C) 2018  Andreas Mieke

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
	// Init GPIOB and USART3 clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	// Create gpio struct and fill it with default values
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	// Set PB10 to alternate function push pull
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &gpio);
	
	// Set PB11 to input floating
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &gpio);
	
	// Create usart struct and set USART3 to 9600 baud
	USART_InitTypeDef usart;
	USART_StructInit(&usart);
	usart.USART_BaudRate = 9600;
	USART_Init(USART3, &usart);
	
	// Init USART clock
	USART_ClockInitTypeDef usartclock;
	USART_ClockStructInit(&usartclock);
	USART_ClockInit(USART3, &usartclock);

	// Enable USART3
	USART_Cmd(USART3, ENABLE);
}

void run_display(void)
{
	// String to store command
	char cmd[16], *cptr;
	cptr = cmd;
	// Fill string with every color from colors[]
	sprintf(cmd, "cls %s\xFF\xFF\xFF", colors[i++]);
	// If the last color is reached, begin again
	if (i == COLORS) i = 0;
	// Send character for character to USART3 (the display)
	while(*cptr) {
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3, *cptr++);
	}
}

void deinit_display(void)
{
	// Reset command
	char *cmd = "rest\xFF\xFF\xFF";
	// Reset the display after the test finishes
	while(*cmd) {
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3, *cmd++);
	}
	// Wait till the last character is sent
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	// Then disable USART3 again
	USART_Cmd(USART3, DISABLE);
	USART_DeInit(USART3);
}
