#include "eeprom.h"

extern volatile uint32_t SysTickCnt;

#define EEPROM_ADDR (uint8_t)0xA0				// 0b10100000
																				//   ----			Vendor address part
																				//       ---  User address part
																				//          - Keep free for R/W bit (set by I2C_Send7bitAddress())

uint32_t last_write_tick = 0;

void eeprom_init(void)
{
	i2c1_init();
}

void eeprom_read(uint16_t address, uint8_t *data, uint16_t length)
{
	uint16_t cur_pos;
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, (address & 0xFF00) >> 8);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C1, (address & 0x00FF));
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTART(I2C1, ENABLE);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	for(cur_pos = 0; cur_pos < length; cur_pos++) {
		if(cur_pos == length - 1) {
			I2C_AcknowledgeConfig(I2C1, DISABLE);
		}
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
		data[cur_pos] = I2C_ReceiveData(I2C1);
	}
	
	I2C_GenerateSTOP(I2C1, ENABLE);
}

void eeprom_write(uint16_t address, uint8_t *data, uint16_t length)
{
	uint8_t cur_page = 0;
	uint16_t cur_pos = 0;
	address = address & 0xFFA0;
	
	for(cur_page = 0; cur_page <= ((length-1)/64); cur_page++) {
		while((SysTickCnt - last_write_tick) <= 5);
		I2C_GenerateSTART(I2C1, ENABLE);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
		I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
		I2C_SendData(I2C1, (address & 0xFF00) >> 8);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		I2C_SendData(I2C1, (address & 0x00FF));
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		
		for(; (cur_pos < length) && (cur_pos%64 <= 63); cur_pos++) {
			I2C_SendData(I2C1, data[cur_pos]);
			while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		}
		
		I2C_GenerateSTOP(I2C1, ENABLE);
		address += 0x0040;
		last_write_tick = SysTickCnt;
	}
}