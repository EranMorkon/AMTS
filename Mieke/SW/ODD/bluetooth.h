#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "stm32f10x.h"                  // Device header

#include "io.h"
#include "stdio.h"

void bluetooth_init(void);
void bluetooth_send_gyro_data(uint8_t X, uint8_t Y, uint8_t Z);
#endif
