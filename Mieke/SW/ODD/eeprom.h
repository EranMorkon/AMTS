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

#ifndef EEPROM_H
#define EEPROM_H

#include "stm32f10x.h"                  // Device header

#include "io.h"
#include "systick.h"

void eeprom_init(void);
void eeprom_read(uint16_t address, uint8_t *data, uint16_t length);
void eeprom_write(uint16_t address, uint8_t *data, uint16_t length);

#endif
