#include <stm32f10x.h>

void wait(void);
void init_leds_switches(void);
void set_leds(char value);
char get_switches(void);
void wait_ms(int ms);

void init_leds_switches() {
    int temp;

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    temp = GPIOA->CRL;
    temp &= 0x00000000;
    temp |= 0x88888888;
    GPIOA->CRL = temp;
    GPIOA->BSRR = 0x00FF;

    temp = GPIOB->CRH;
    temp &= 0x00000000;
    temp |= 0x33333333;
    GPIOB->CRH = temp;
}

void set_leds(char value) {
    GPIOB->ODR =    (GPIOB->ODR & 0xFFFF00FF) |
                    ((value & 0x000000FF) <<8);
}

char get_switches() {
    return (GPIOA->IDR & 0x000000FF);
}

void wait_ms(int ms) {
    int i,j;

    for (i = 0; i < ms; i++) {
        for (j = 0; j < 1595; j++);
    }
}

void wait() {
    unsigned char speed;
    unsigned char i;

    speed = get_switches();
    speed = ~speed;
    speed = speed & 0x7f;

    for (i = 0; i <= speed; i++) {
        wait_ms(100);
    }
}

int main () {
    char i;
    char hs;
    char lauflicht;

    init_leds_switches();
    hs = get_switches() & 0x80;
    while (hs) {
        lauflicht = 0x01;
        i = 0;
        while (i < 8) {
            set_leds(lauflicht);
            lauflicht = lauflicht << 1;
            i++;
            wait();
        }
        hs = get_switches() & 0x80;
    }

    while (1);
}
