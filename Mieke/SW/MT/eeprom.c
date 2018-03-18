#include "eeprom.h"

volatile uint32_t *EEPROMSTick, EEPROMSTickCur;

uint8_t load_byte(uint16_t eeprom_addr)
{
	uint8_t data;
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, (eeprom_addr & 0xFF00) >> 8);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C1, (eeprom_addr & 0x00FF));
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	data = I2C_ReceiveData(I2C1);
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	return data;
}

void write_byte(uint16_t eeprom_addr, uint8_t data)
{
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, (eeprom_addr & 0xFF00) >> 8);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C1, (eeprom_addr & 0x00FF));
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_SendData(I2C1, data);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	EEPROMSTickCur = *EEPROMSTick;
	while((*EEPROMSTick - EEPROMSTickCur) <= 50); // 5ms write cycle, see datasheet param 17
	return;
}

void init_eeprom(volatile uint32_t *SysTickCnt)
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
	EEPROMSTick = SysTickCnt;
	return;
}

void run_eeprom(uint8_t *success)
{
	uint8_t set, read;
	*success = 0;
	set = 0xAA;
	write_byte(0x0000, set);
	read = load_byte(0x0000);
	if (set != read) return;
	set = 0xBA;
	write_byte(0x0010, set);
	read = load_byte(0x0010);
	if (set != read) return;
	set = 0xAD;
	write_byte(0x0001, set);
	read = load_byte(0x0001);
	if (set != read) return;
	set = 0x00;
	write_byte(0x0000, set);
	read = load_byte(0x0000);
	if (set != read) return;
	set = 0x88;
	write_byte(0x0002, set);
	read = load_byte(0x0002);
	if (set != read) return;
	set = 0x01;
	write_byte(0x0000, set);
	read = load_byte(0x0000);
	if (set != read) return;
	set = 0x55;
	write_byte(0x00005, set);
	read = load_byte(0x0005);
	if (set != read) return;
	set = 0xAA;
	write_byte(0x0005, set);
	read = load_byte(0x0005);
	if (set != read) return;
	*success = 1;
	return;
}

void deinit_eeprom(void)
{
	I2C_Cmd(I2C1, DISABLE);
	I2C_DeInit(I2C1);
	return;
}
