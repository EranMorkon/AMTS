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

#include "bluetooth.h"

void bluetooth_init(void)
{
	// Init USART1
	usart1_init();
}

void bluetooth_send_gyro_data(uint8_t X, uint8_t Y, uint8_t Z)
{
	char __str[128] = {0};
	char *str = __str;
	// Print data comma-sperated to String
	sprintf(str, "%d,%d,%d\r\n", X, Y, Z);
	// Send string over USARt to the bluetooth module
	USART_SendString(USART1, str);
}
