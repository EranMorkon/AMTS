/*
	Ethernet shield test program
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

#include "stm32f10x.h"
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART

#include "w5100.h"
#include "socket.h"

#include "string.h"

#define BUFFLEN 256

// Socket (TCP) with ID 1
SOCKET tcp = 1;
uint8_t buffer[BUFFLEN] = {0};
uint8_t len = 0;

void USART_SendString(USART_TypeDef *USARTx, uint8_t *str)
{
	// Send a string character by character over UART
	while(*str) {
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		USART_SendData(USARTx, *str++);
	}
}

int main() {
	// Enable USART 1 and GPIOA clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// Create gpio strct and set default values
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
	
	// Set up USART1
	USART_InitTypeDef usart;
	USART_StructInit(&usart);
	USART_Init(USART1, &usart);
	
	// Set up USART1 clocks
	USART_ClockInitTypeDef usartclock;
	USART_ClockStructInit(&usartclock);
	USART_ClockInit(USART1, &usartclock);
	
	// Enable USART1
	USART_Cmd(USART1, ENABLE);
	
	// Initzialize Ethernet module
	ETH_init();
	
	// Set gateway IP
	uint8_t gid[4] = {192, 168, 10, 1};

	// Set subnet mask
	uint8_t sma[4] = {255, 255, 255, 0};

	// Set MAC address (unused testing address space)
	uint8_t mac[6] = {0x1E, 0x8E, 0xA8, 0x88, 0x1C, 0xAA};

	// Set source IP (IP of the Ethernet module)
	uint8_t sip[4] = {192, 168, 10, 2};
	
	// Transmit all these values to the module
	ETH_set_gateway_IP(gid);
	ETH_set_subnet_mask(sma);
	ETH_set_mac(mac);
	ETH_set_IP(sip);
	
	// Try to create a TCP socket on port 80 as long as it does not work
	do {
		ETH_socket(tcp, ETH_SMR_TCP, 80, 0);
	} while (ETH_socket_status(tcp) != ETH_SSR_INIT);

	// Wait for the socket to finish creating
	while(!ETH_listen(tcp));
	
	// Endless loop
	do {
		// Wait for data to be available
		while(ETH_recv_available(tcp) == 0);
		// Receive data
		buffer[ETH_recv(tcp, buffer, BUFFLEN)] = 0;
		// Send data to USART
		USART_SendString(USART1, buffer);
		// Send string over tcp
		ETH_send(tcp, "Hallo, Welt!", 12);
	} while (1);
}
