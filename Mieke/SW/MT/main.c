#include <stdio.h>
#include "interface_uart.h"

#include "main.h"

#include "bma.h"
#include "ne555.h"
#include "ledswitch.h"
#include "eeprom.h"
#include "esp.h"
#include "rgb.h"
#include "piezo.h"
#include "display.h"

volatile uint32_t SysTickCnt = 0;
USART_TypeDef *used_usart;

void SysTick_Handler()
{
	SysTickCnt++;
}

void USART2_IRQHandler()
{
	USART_SendData(used_usart, USART_ReceiveData(USART2));
}

void wait(uint32_t ms)
{
	uint32_t SysTickCntHold = SysTickCnt;  
  while((SysTickCnt - SysTickCntHold) <= ms);
}

int main()
{
	char buffer[1024];
	enum test_t current_test = test_not_init, next_test = test_not_init;
	enum iface_t control_interface = interface_none;

	for (;;) {
		switch (control_interface) {
			case interface_none:
				init_all();
				send_welcome();
				control_interface = (enum iface_t)wait_for_start();
				switch (control_interface) {
					case interface_usart1:
						used_usart = USART1;
						break;
					case interface_usart2:
						used_usart = USART2;
						break;
					case interface_usart3:
						used_usart = USART3;
						break;
					default: 
						control_interface = interface_none;
						break;
				}
				break;
			case interface_usart1:
			case interface_usart2:
			case interface_usart3:
				switch(current_test) {
					case test_not_init:
						current_test = test_none;
						break;
					case test_none:
						USART_SendString(used_usart, "\x1B[2J\x1B[0;0HManufacturing test software, Version " VERSION "\r\n\r\n\
\tTo run tests, enter one of the following numbers:\r\n\t\t[2]\tBMA\r\n\t\t[3]\tNE555/LFU/IR\r\n\
\t\t[4]\tLEDs and Switches\r\n\t\t[5]\tESP\r\n\t\t[6]\tEEPROM\r\n\t\t[7]\tRGB LED\r\n\t\t[8]\tPiezo\r\n\t\t[9]\tDisplay\r\n\r\nWaiting for your selection... ");
						current_test = (enum test_t)wait_for_test(used_usart);
						break;
					case test_bma:
						USART_SendString(used_usart, "\x1B[2J\x1B[0;0HBMA Test\r\n\r\nThis tests the correct function\
 of the used BMA gyroscope as well as I2C port 1. Below you should see the current acceleration values printed\
 for the X, Y and Z axis. Where the Z axis should show something around 1g, as this is the gravitational\
 acceleration on the Earth, of course this value is lower if you run this program on the moon!\r\n\r\n\
To return to the main menu press a button.\r\n\r\n\r\n");
						init_bma();
						float accs[3];
						for(;;) {
							run_bma(accs);
							sprintf(buffer, "\x1B[K\rX: % 02.5f, Y: % 02.5f, Z: % 02.5f", accs[0], accs[1], accs[2]);
							USART_SendString(used_usart, buffer);
							next_test = (enum test_t)get_test(used_usart);
							if (next_test != test_not_init) {
								current_test = next_test;
								deinit_bma();
								break;
							}
							wait(50);
						}
						break;
					case test_ne555:
						USART_SendString(used_usart, "\x1B[2J\x1B[0;0HNE555/LFU/IR Test\r\n\r\nThis tests the correct function\
 of the NE555/LFU/IR on the board. The currently selected frequency should be printed below!\r\n\r\n\
To return to the main menu press a button.\r\n\r\n\r\n");
						init_ne555(&SysTickCnt);
						float freq;
						for(;;) {
							run_ne555(&freq);
							sprintf(buffer, "\x1B[K\r% 05.1fHz", freq);
							USART_SendString(used_usart, buffer);
							next_test = (enum test_t)get_test(used_usart);
							if (next_test != test_not_init) {
								current_test = next_test;
								deinit_ne555();
								break;
							}
							wait(50);
						}
						break;
					case test_led:
						USART_SendString(used_usart, "\x1B[2J\x1B[0;0HLED/Switch Test\r\n\r\nThis tests the correct function\
 of the switches and the leds on the board. The currently selected leds are shown hexadecimal below!\r\n\r\n\
To return to the main menu press a button.\r\n\r\n\r\n");
						init_ledswitch();
						uint8_t switches;
						for(;;) {
							run_ledswitch(&switches);
							sprintf(buffer, "\x1B[K\rSelected: 0x%02x", switches);
							USART_SendString(used_usart, buffer);
							next_test = (enum test_t)get_test(used_usart);
							if (next_test != test_not_init) {
								current_test = next_test;
								deinit_ledswitch();
								break;
							}
							wait(50);
						}
						break;
					case test_esp:
						USART_SendString(used_usart, "\x1B[2J\x1B[0;0HESP Test\r\n\r\nThis tests the correct function\
 of the ESP module on the board. The ESP should open a WLAN AP!\r\n\r\n\
To return to the main menu press a button.\r\n\r\n\r\n");
						init_esp();
						sprintf(buffer, "\x1B[K\rRunning test...\r\n\r\n");
						USART_SendString(used_usart, buffer);
						run_esp();
						for(;;) {
							next_test = (enum test_t)get_test(used_usart);
							if (next_test != test_not_init) {
								current_test = next_test;
								deinit_esp();
								break;
							}
							wait(50);
						}
						break;
					case test_eeprom:
						USART_SendString(used_usart, "\x1B[2J\x1B[0;0HEEPROM Test\r\n\r\nThis tests the correct function\
 of the EEPROM on the board. The test will run and show an OK or NOK below!\r\n\r\n\
To return to the main menu press a button.\r\n\r\n\r\n");
						init_eeprom(&SysTickCnt);
						uint8_t success;
						sprintf(buffer, "\x1B[K\rRunning test...");
						USART_SendString(used_usart, buffer);
						run_eeprom(&success);
						if(success == 1) sprintf(buffer, "OK");
						else sprintf(buffer, "NOK");
						USART_SendString(used_usart, buffer);
						for(;;) {
							next_test = (enum test_t)get_test(used_usart);
							if (next_test != test_not_init) {
								current_test = next_test;
								deinit_eeprom();
								break;
							}
							wait(50);
						}
						break;
					case test_rgb:
						USART_SendString(used_usart, "\x1B[2J\x1B[0;0HRGB LED Test\r\n\r\nThis tests the correct function\
 of the RGB LED and it's I2C driver on the board!\r\n\r\n\
To return to the main menu press a button.\r\n\r\n\r\n");
						init_rgb();
						for(;;) {
							run_rgb();
							next_test = (enum test_t)get_test(used_usart);
							if (next_test != test_not_init) {
								current_test = next_test;
								deinit_rgb();
								break;
							}
							wait(50);
						}
						break;
					case test_piezo:
						USART_SendString(used_usart, "\x1B[2J\x1B[0;0HPiezo Test\r\n\r\nThis tests the correct function\
 of the Piezo on the board. You should hear the frequency printed below!\r\n\r\n\
To return to the main menu press a button.\r\n\r\n\r\n");
						init_piezo(&SysTickCnt);
						for(;;) {
							run_piezo();
							sprintf(buffer, "\x1B[K\r500Hz");
							USART_SendString(used_usart, buffer);
							next_test = (enum test_t)get_test(used_usart);
							if (next_test != test_not_init) {
								current_test = next_test;
								deinit_piezo();
								break;
							}
							wait(50);
						}
						break;
					case test_display:
						USART_SendString(used_usart, "\x1B[2J\x1B[0;0HDisplay Test\r\n\r\nThis tests the correct function\
 of the Display on the board. You should see text printed on the display!\r\n\r\n\
To return to the main menu press a button.\r\n\r\n\r\n");
						init_display();
						for(;;) {
							run_display();
							sprintf(buffer, "OK");
							USART_SendString(used_usart, buffer);
							next_test = (enum test_t)get_test(used_usart);
							if (next_test != test_not_init) {
								current_test = next_test;
								deinit_display();
								break;
							}
							wait(50);
						}
						break;
					default:
						current_test = test_none;
						break;
				}
				break;
			default:
				break;
		}
	}
}
