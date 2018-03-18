#include "display.h"

disp_state_t state = DISP_STATE_NONE;
uint8_t running = 0;

void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) {
		state = (disp_state_t)((USART_ReceiveData(USART3) & 0x00FF) - '0');
		if (state > 4) {
			state = DISP_STATE_NONE;
		}
	}
	if (state == DISP_STATE_START) {
		running = 1;
	} else if (state == DISP_STATE_PAUSE) {
		running = 0;
	}
}

void disp_init(void)
{
	usart3_init();
	
	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = USART3_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&nvic);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	disp_disable();
}

disp_state_t disp_get_last_state(void)
{
	disp_state_t tmp = state;
	state = DISP_STATE_NONE;
	return tmp;
}

void disp_send_gyro_data(uint8_t X, uint8_t Y, uint8_t Z)
{
	char __str[128] = {0};
	char *str = __str;
	sprintf(str, "add 1,0,%d\xFF\xFF\xFF" "add 1,1,%d\xFF\xFF\xFF" "add 1,2,%d\xFF\xFF\xFF", X, Y, Z);
	USART_SendString(USART3, str);
}

void disp_disable(void)
{
	USART_SendString(USART3, "dim=0\xFF\xFF\xFF");
}

void disp_enable(void)
{
	USART_SendString(USART3, "dim=100\xFF\xFF\xFF");
}
