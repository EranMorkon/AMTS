#include "bma.h"

#define BMA_ADDR (uint8_t)0x70					// 0b01110000
												//   ----			Vendor address part
												//       ---  User address part
												//          - Keep free for R/W bit (set by I2C_Send7bitAddress())

void bma_init(void)
{
	i2c1_init();
}

void bma_get_acc(uint8_t *X, uint8_t *Y, uint8_t *Z)
{
	int16_t acc[3];
	// Send first register address to read
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, BMA_ADDR, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, 0x02);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	// Start Rx transmission
	I2C_GenerateSTART(I2C1, ENABLE);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, BMA_ADDR, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	// Read X LSB
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	acc[0] = (I2C_ReceiveData(I2C1) & 0xC0) >> 6;
	// Read X MSB
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	acc[0] = (I2C_ReceiveData(I2C1) & 0xFF) << 2 | (acc[0] & 0x0003);
	if(acc[0] & 0x0200) acc[0] |= 0xFC00;
	
	// Read Y LSB
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	acc[1] = (I2C_ReceiveData(I2C1) & 0xC0) >> 6;
	// Read Y MSB
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	acc[1] = (I2C_ReceiveData(I2C1) & 0xFF) << 2 | (acc[1] & 0x0003);
	if(acc[1] & 0x0200) acc[1] |= 0xFC00;
	
	// Read Z LSB
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	acc[2] = (I2C_ReceiveData(I2C1) & 0xC0) >> 6;
	// Read Z MSB
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	acc[2] = (I2C_ReceiveData(I2C1) & 0xFF) << 2 | (acc[2] & 0x0003);
	if(acc[2] & 0x0200) acc[2] |= 0xFC00;
	
	// Stop condition
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	// Calculate display values, with a maximum amplitude of 2.0g, as described in the datasheet
	*X = (acc[0]/4) + 128;
	*Y = (acc[1]/4) + 128;
	*Z = (acc[2]/4) + 128;
}
