#ifndef EEPROM_H
#define EEPROM_H

#include "stm32f10x.h"                  // Device header

#include "io.h"
#include "systick.h"

void eeprom_init(void);
void eeprom_read(uint16_t address, uint8_t *data, uint16_t length);
void eeprom_write(uint16_t address, uint8_t *data, uint16_t length);

#endif
