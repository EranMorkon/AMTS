#include "bma.h"

void init_bma(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	// SCL
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &gpio);
	
	// SDA
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	gpio.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &gpio);
	
	I2C_InitTypeDef i2c;
	I2C_StructInit(&i2c);
	i2c.I2C_ClockSpeed = 400000;
	I2C_Init(I2C1, &i2c);
	
	I2C_Cmd(I2C1, ENABLE);
}

void run_bma(float acc_f[3])
{
	int16_t acc[3];
	// Select first register address to read
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, BMA_ADDR, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, 0x02);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	I2C_GenerateSTART(I2C1, ENABLE);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, BMA_ADDR, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	// X LSB
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	acc[0] = (I2C_ReceiveData(I2C1) & 0xC0) >> 6;
	// X MSB
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	acc[0] = (I2C_ReceiveData(I2C1) & 0xFF) << 2 | (acc[0] & 0x0003);
	if(acc[0] & 0x0200) acc[0] |= 0xFC00;
	
	// Y LSB
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	acc[1] = (I2C_ReceiveData(I2C1) & 0xC0) >> 6;
	// Y MSB
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	acc[1] = (I2C_ReceiveData(I2C1) & 0xFF) << 2 | (acc[1] & 0x0003);
	if(acc[1] & 0x0200) acc[1] |= 0xFC00;
	
	// Z LSB
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	acc[2] = (I2C_ReceiveData(I2C1) & 0xC0) >> 6;
	// Z MSB
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	acc[2] = (I2C_ReceiveData(I2C1) & 0xFF) << 2 | (acc[2] & 0x0003);
	if(acc[2] & 0x0200) acc[2] |= 0xFC00;
	
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	acc_f[0] = 2.0f * ((float)acc[0]/512);
	acc_f[1] = 2.0f * ((float)acc[1]/512);
	acc_f[2] = 2.0f * ((float)acc[2]/512);
}

void deinit_bma(void)
{
	I2C_Cmd(I2C1, DISABLE);
	I2C_DeInit(I2C1);
}
