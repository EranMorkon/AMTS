#include "stm32f10x.h"

void init_leds_switches(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRH &= 0x00000000;
    GPIOB->CRH |= 0x00000003;
}

int main()
{
    int i;
    init_leds_switches();

    for (;;) {
        GPIOB->ODR = 0x01FF & ~(GPIOB->ODR & 0x01FF);
        for (i=0; i < 65535; i++);
        for (i=0; i < 65535; i++);
        for (i=0; i < 65535; i++);
    }
}
