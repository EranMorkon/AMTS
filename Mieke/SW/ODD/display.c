/*
	Demo program for the 2017/18 open door day at HTL Hollabrunn
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

disp_state_t state = DISP_STATE_NONE;
uint8_t running = 0;

void USART3_IRQHandler(void)
{
	// HAndle only USART3 RXNE interrupt
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) {
		// Get display state
		state = (disp_state_t)((USART_ReceiveData(USART3) & 0x00FF) - '0');
		if (state > 4) {
			state = DISP_STATE_NONE;
		}
	}
	// Set running according to pressed button
	if (state == DISP_STATE_START) {
		running = 1;
	} else if (state == DISP_STATE_PAUSE) {
		running = 0;
	}
}

void disp_init(void)
{
	// Init USART3
	usart3_init();
	
	// Init USART3 interrupt
	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = USART3_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&nvic);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	// Disable display till startup is complete
	disp_disable();
}

disp_state_t disp_get_last_state(void)
{
	// Returns the last state (which we got form the interrupt)
	disp_state_t tmp = state;
	state = DISP_STATE_NONE;
	return tmp;
}

void disp_send_gyro_data(uint8_t X, uint8_t Y, uint8_t Z)
{
	char __str[128] = {0};
	char *str = __str;
	// Print gyro data to the display
	sprintf(str, "add 1,0,%d\xFF\xFF\xFF" "add 1,1,%d\xFF\xFF\xFF" "add 1,2,%d\xFF\xFF\xFF", X, Y, Z);
	USART_SendString(USART3, str);
}

void disp_disable(void)
{
	// Disable (dim to 0%) display
	USART_SendString(USART3, "dim=0\xFF\xFF\xFF");
}

void disp_enable(void)
{
	// Enable display again (dim to 100%)
	USART_SendString(USART3, "dim=100\xFF\xFF\xFF");
}
