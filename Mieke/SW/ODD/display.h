#ifndef DISPLAY_H
#define DISPLAY_H

#include "stm32f10x.h"                  // Device header

#include "io.h"
#include "stdio.h"

typedef enum uint8_t {
	DISP_STATE_NONE = 0x00,
	DISP_STATE_PAUSE,
	DISP_STATE_START,
	DISP_STATE_SAVE,
	DISP_STATE_RECALL,
	DISP_STATE_ERROR = 0xFF - '0'
} disp_state_t;

void disp_init(void);
disp_state_t disp_get_last_state(void);
void disp_send_gyro_data(uint8_t X, uint8_t Y, uint8_t Z);
void disp_disable(void);
void disp_enable(void);

#endif
