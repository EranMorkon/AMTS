#ifndef BMA_H
#define BMA_H

#include "stm32f10x.h"                  // Device header

#include "io.h"

void bma_init(void);
void bma_get_acc(uint8_t *X, uint8_t *Y, uint8_t *Z);

#endif
