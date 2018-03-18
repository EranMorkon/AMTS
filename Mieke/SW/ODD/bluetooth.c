#include "bluetooth.h"

void bluetooth_init(void)
{
	usart1_init();
}

void bluetooth_send_gyro_data(uint8_t X, uint8_t Y, uint8_t Z)
{
	char __str[128] = {0};
	char *str = __str;
	sprintf(str, "%d,%d,%d\r\n", X, Y, Z);
	USART_SendString(USART1, str);
}
