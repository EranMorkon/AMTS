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
#include "bma.h"
#include "eeprom.h"
#include "bluetooth.h"

#define BUFFER_SIZE 256

int main()
{
	// Variables needed
	disp_state_t current_state = DISP_STATE_NONE;
	uint8_t X, Y, Z, running = 0;
	uint8_t bX[BUFFER_SIZE] = {128}, bY[BUFFER_SIZE] = {128}, bZ[BUFFER_SIZE] = {128}; // 128 is the 'zero' line in the display, so the array is initzialized to 128
	uint16_t buffer_pos = 0;

	// Init everything we need
	disp_init();
	systick_init();
	bma_init();
	eeprom_init();
	bluetooth_init();

	// Enable display
	disp_enable();
	
	// Main loop
	for (;;) {
		// Check if button is pressed
		current_state = disp_get_last_state();
		if(current_state == DISP_STATE_START) {
			// If start button set running to true
			running = 1;
		} else if(current_state == DISP_STATE_PAUSE) {
			// On pause set running to false
			running = 0;
		}
		// Only read and send data if display is actually running
		if(running == 1) {
			// Get accelerations
			bma_get_acc(&X, &Y, &Z);
			// Put them into the buffer (for save/recall)
			bX[buffer_pos] = X;
			bY[buffer_pos] = Y;
			bZ[buffer_pos] = Z;
			// Send data to display and bluetooth
			disp_send_gyro_data(X, Y, Z);
			bluetooth_send_gyro_data(X, Y, Z);
			// Increment buffer position
			buffer_pos++;
			if(buffer_pos == BUFFER_SIZE) {
				// If buffer size is reached, return to 0 (ringbuffer)
				buffer_pos = 0;
			}
		}
		if(current_state == DISP_STATE_SAVE) {
			// On save, send the three arrays to the EEPROM
			eeprom_write(0x0000, bX, BUFFER_SIZE);
			eeprom_write(0x0400, bY, BUFFER_SIZE);
			eeprom_write(0x0800, bZ, BUFFER_SIZE);
		}
		if(current_state == DISP_STATE_RECALL) {
			// On recall read the three arrays from EEPROM
			eeprom_read(0x0000, bX, BUFFER_SIZE);
			eeprom_read(0x0400, bY, BUFFER_SIZE);
			eeprom_read(0x0800, bZ, BUFFER_SIZE);
			// Also update display and bluetooth completely with the new data in the buffer
			for(uint16_t i = 0; i < BUFFER_SIZE; i++) {
				disp_send_gyro_data(bX[i], bY[i], bZ[i]);
				bluetooth_send_gyro_data(bX[i], bY[i], bZ[i]);
			}
		}
	}
}
