#include "rgb.h"
#include "main.h"

// Define registers of the RGB LED driver
typedef enum {
	RGB_SHUTDOWN = 0x00,
	RGB_MAXCUR,
	RGB_RED,
	RGB_GREEN,
	RGB_BLUE,
	RGB_UPLEND,
	RGB_DOWNLEND,
	RGB_DIMSTEP
} RGB_T;


void rgb_send_command(uint8_t data)
{
	// Generate start condition and transmit address
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, RGB_ADDR, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	// Send data
	I2C_SendData(I2C1, data);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	// Generate stop condition
	I2C_GenerateSTOP(I2C1, ENABLE);

	return;
}

void init_rgb(void)
{
	// Enable GPIOB anf I2C1 clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	// Create gpio struct and fill it with default values
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	// SET PB6 to alternate function push pull (SCL)
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &gpio);
	
	// Set PB7 to alternate function open drain (SDA)
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	gpio.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &gpio);
	
	// Init I2C1 to 400 kHz
	I2C_InitTypeDef i2c;
	I2C_StructInit(&i2c);
	i2c.I2C_ClockSpeed = 400000;
	I2C_Init(I2C1, &i2c);
	
	// Enable I2C1 driver
	I2C_Cmd(I2C1, ENABLE);
	
	// Set the max current the driver should allow
	// LED actually allows 40mA, but driver only handles around 30
	rgb_send_command(0x3F);
	return;
}

void run_rgb(void)
{
	// Set random colors for 100 ms each
	rgb_send_command(0x5F);
	wait(100);
	rgb_send_command(0x7F);
	wait(100);
	rgb_send_command(0x9F);
	wait(100);
	rgb_send_command(0x50);
	wait(100);
	rgb_send_command(0x70);
	wait(100);
	rgb_send_command(0x90);
	wait(100);
	rgb_send_command(0x40);
	wait(100);
	rgb_send_command(0x60);
	wait(100);
	rgb_send_command(0x80);
	wait(100);
	return;
}

void deinit_rgb(void)
{
	// Disable I2C1
	I2C_Cmd(I2C1, DISABLE);
	I2C_DeInit(I2C1);
	return;
}
