#include "display.h"
#include "bma.h"
#include "eeprom.h"
#include "bluetooth.h"

#define BUFFER_SIZE 256

int main()
{
	disp_state_t current_state = DISP_STATE_NONE;
	uint8_t X, Y, Z, running = 0;
	uint8_t bX[BUFFER_SIZE] = {128}, bY[BUFFER_SIZE] = {128}, bZ[BUFFER_SIZE] = {128};
	uint16_t buffer_pos = 0;
	disp_init();
	systick_init();
	bma_init();
	eeprom_init();
	bluetooth_init();
	disp_enable();
	
	// Main loop
	for (;;) {
		current_state = disp_get_last_state();
		if(current_state == DISP_STATE_START) {
			running = 1;
		} else if(current_state == DISP_STATE_PAUSE) {
			running = 0;
		}
		if(running == 1) {
			bma_get_acc(&X, &Y, &Z);
			bX[buffer_pos] = X;
			bY[buffer_pos] = Y;
			bZ[buffer_pos] = Z;
			disp_send_gyro_data(X, Y, Z);
			bluetooth_send_gyro_data(X, Y, Z);
			buffer_pos++;
			if(buffer_pos == BUFFER_SIZE) {
				buffer_pos = 0;
			}
		}
		if(current_state == DISP_STATE_SAVE) {
			eeprom_write(0x0000, bX, BUFFER_SIZE);
			eeprom_write(0x0400, bY, BUFFER_SIZE);
			eeprom_write(0x0800, bZ, BUFFER_SIZE);
		}
		if(current_state == DISP_STATE_RECALL) {
			eeprom_read(0x0000, bX, BUFFER_SIZE);
			eeprom_read(0x0400, bY, BUFFER_SIZE);
			eeprom_read(0x0800, bZ, BUFFER_SIZE);
			for(uint16_t i = 0; i < BUFFER_SIZE; i++) {
				disp_send_gyro_data(bX[i], bY[i], bZ[i]);
				bluetooth_send_gyro_data(bX[i], bY[i], bZ[i]);
			}
		}
	}
}
