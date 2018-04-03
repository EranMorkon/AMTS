/*
		Ethernet shield test program
    Copyright (C) 2018  Andreas Mieke
    Copyright (C) 2010  Arduino LLC

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "w5100.h"

uint16_t ETH_SBASE[ETH_SOCKETS]; // Tx buffer base address
uint16_t ETH_RBASE[ETH_SOCKETS]; // Rx buffer base address

#define TX_RX_MAX_BUF_SIZE 2048
#define TX_BUF 0x1100
#define RX_BUF (TX_BUF + TX_RX_MAX_BUF_SIZE)

#define ETH_TXBUF_BASE 0x4000
#define ETH_RXBUF_BASE 0x6000

void SPI_SetSS(void)
{
	GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
}

void SPI_ResetSS(void)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) != RESET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
}

uint8_t SPI_WriteRead(uint8_t write)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI1->DR = write;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI1->DR;
}

void ETH_init(void)
{
	// Port init stuff
	// PA5 -> SCK
	// PA6 -> MISO
	// PA7 -> MOSI
	// PC8 -> /SS
	
	// Set RCC Registers for GPIOA/C
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	// Init gpio structure to default values
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	// Set PA5 to alternate function PushPull
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA, &gpio);
	
	// Set PA7 to alternate function push pull
	gpio.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &gpio);
	
	// Set PC8 to push pull
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOC, &gpio);
	GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
	
	// Set PA6 to input floating
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &gpio);
	
	// Init SPI engine
	// Init struct to default values
	SPI_InitTypeDef spi;
	SPI_StructInit(&spi);
	
	// We are master
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_NSS = SPI_NSS_Soft;
	//spi.SPI_CPHA = SPI_CPHA_2Edge;
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	
	// Write the registers
	SPI_Init(SPI1, &spi);
	
	// Enable SPI1
	SPI_Cmd(SPI1, ENABLE);
	
	for(uint16_t i = 0; i < 65535; i++);
	
	ETH_writeMR(ETH_MR_RST);
	ETH_writeRMSR(0x55);
	ETH_writeTMSR(0x55);
	
	for (int i=0; i<MAX_SOCK_NUM; i++) {
    ETH_SBASE[i] = ETH_TXBUF_BASE + ETH_SSIZE * i;
    ETH_RBASE[i] = ETH_RXBUF_BASE + ETH_RSIZE * i;
  }
}

uint8_t ETH_write_8(uint16_t addr, uint8_t data)
{
	SPI_SetSS();
	SPI_WriteRead(0xF0);
	SPI_WriteRead(addr >> 8);
	SPI_WriteRead(addr & 0xFF);
	SPI_WriteRead(data);
	SPI_ResetSS();
	return 8;
}

uint16_t ETH_write_n(uint16_t addr, const uint8_t *buf, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++) {
		SPI_SetSS();
		SPI_WriteRead(0xF0);
		SPI_WriteRead(addr >> 8);
		SPI_WriteRead(addr & 0xFF);
		addr++;
		SPI_WriteRead(buf[i]);
		SPI_ResetSS();
	}
	return len;
}

uint8_t ETH_read_8(uint16_t addr)
{
	uint8_t data;
	SPI_SetSS();
	SPI_WriteRead(0x0F);
	SPI_WriteRead(addr >> 8);
	SPI_WriteRead(addr & 0xFF);
	data = SPI_WriteRead(0x00);
	SPI_ResetSS();
	return data;
}

uint16_t ETH_read_n(uint16_t addr, uint8_t *buf, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++) {
		SPI_SetSS();
		SPI_WriteRead(0x0F);
		SPI_WriteRead(addr >> 8);
		SPI_WriteRead(addr & 0xFF);
		addr++;
		buf[i] = SPI_WriteRead(0x00);
		SPI_ResetSS();
	}
	return len;
}

uint16_t ETH_get_TX_free_size(SOCKET s)
{
	uint16_t val=0, val1=0;
  do {
    val1 = ETH_readSnTX_FSR(s);
    if (val1 != 0)
      val = ETH_readSnTX_FSR(s);
  } 
  while (val != val1);
  return val;
}

uint16_t ETH_get_RX_received_size(SOCKET s)
{
	uint16_t val=0,val1=0;
  do {
    val1 = ETH_readSnRX_RSR(s);
    if (val1 != 0)
      val = ETH_readSnRX_RSR(s);
  } 
  while (val != val1);
  return val;
}

void ETH_read_data(SOCKET s, volatile uint16_t src, volatile uint8_t * dst, uint16_t len)
{
	uint16_t size;
  uint16_t src_mask;
  uint16_t src_ptr;

  src_mask = src & ETH_RMASK;
  src_ptr = ETH_RBASE[s] + src_mask;

  if( (src_mask + len) > ETH_RSIZE ) 
  {
    size = ETH_RSIZE - src_mask;
    ETH_read_n(src_ptr, (uint8_t *)dst, size);
    dst += size;
    ETH_read_n(ETH_RBASE[s], (uint8_t *) dst, len - size);
  } 
  else
    ETH_read_n(src_ptr, (uint8_t *) dst, len);
}

void ETH_send_data_processing(SOCKET s, const uint8_t *data, uint16_t len)
{
	ETH_send_data_processing_offset(s, 0, data, len);
}

void ETH_send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len)
{
	uint16_t ptr = ETH_readSnTX_WR(s);
  ptr += data_offset;
  uint16_t offset = ptr & ETH_SMASK;
  uint16_t dstAddr = offset + ETH_SBASE[s];

  if (offset + len > ETH_SSIZE)
  {
    uint16_t size = ETH_SSIZE - offset;
    ETH_write_n(dstAddr, data, size);
    ETH_write_n(ETH_SBASE[s], data + size, len - size);
  } 
  else {
    ETH_write_n(dstAddr, data, len);
  }

  ptr += len;
  ETH_writeSnTX_WR(s, ptr);
}

void ETH_recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek)
{
	uint16_t ptr;
  ptr = ETH_readSnRX_RD(s);
  ETH_read_data(s, ptr, data, len);
  if (!peek)
  {
    ptr += len;
    ETH_writeSnRX_RD(s, ptr);
  }
}

void ETH_exec_socket_cmd(SOCKET s, uint8_t cmd)
{
	ETH_writeSnCR(s, cmd);
  while(ETH_readSnCR(s)) {
		// Wait for command to be executed
	}
}
