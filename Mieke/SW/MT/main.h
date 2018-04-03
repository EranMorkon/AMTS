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
