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

#ifndef MAIN_H
#define MAIN_H

// Define program version
#define VERSION "0.1.0"

// Define interfaces which can be used
enum iface_t {
	interface_none = 0,
	interface_usart1,
	interface_usart2,
	interface_usart3
};

// Define available tests
enum test_t {
	test_not_init = 0,
	test_none = 1,
	test_bma,
	test_ne555,
	test_led,
	test_esp,
	test_eeprom,
	test_rgb,
	test_piezo,
	test_display
};

void SysTick_Handler(void);
void USART2_IRQHandler(void);
void wait(uint32_t ms);

#endif
