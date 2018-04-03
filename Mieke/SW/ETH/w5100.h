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

#ifndef __W5100_H
#define __W5100_H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_spi.h"              // Keil::Device:StdPeriph Drivers:SPI

#define MAX_SOCK_NUM 4
typedef uint8_t SOCKET;

enum {
	ETH_MR_RST		= 0x80,
	ETH_MR_PB			= 0x10,
	ETH_MR_PPPoE	=	0x08,
	ETH_MR_AI			= 0x02,
	ETH_MR_IND		= 0x01
};

enum {
	ETH_IR_CONFLICT		= 0x80,
	ETH_IR_UNREACH		= 0x40,
	ETH_IR_PPPoE			= 0x20,
	ETH_IR_S3_INT			= 0x08,
	ETH_IR_S2_INT			= 0x04,
	ETH_IR_S1_INT			= 0x02,
	ETH_IR_S0_INT			= 0x01
};

enum {
	ETH_IMR_IR7		= 0x80,
	ETH_IMR_IR6		= 0x40,
	ETH_IMR_IR5		= 0x20,
	ETH_IMR_IR3		= 0x08,
	ETH_IMR_IR2		= 0x04,
	ETH_IMR_IR1		= 0x02,
	ETH_IMR_IR0		= 0x01
};

enum {
	ETH_SMR_MULTI		= 0x80,
	ETH_SMR_ND			= 0x20,
	ETH_SMR_CLOSED	= 0x00,
	ETH_SMR_TCP			= 0x01,
	ETH_SMR_UDP			= 0x02,
	ETH_SMR_IPRAW		= 0x03,
	ETH_SMR_MACRAW	= 0x04,
	ETH_SMR_PPPoE		= 0x05
};

enum {
	ETH_SCR_OPEN			= 0x01,
	ETH_SCR_LISTEN		= 0x02,
	ETH_SCR_CONNECT		= 0x04,
	ETH_SCR_DISCON		= 0x08,
	ETH_SCR_CLOSE			= 0x10,
	ETH_SCR_SEND			= 0x20,
	ETH_SCR_SEND_MAC	= 0x21,
	ETH_SCR_SEND_KEEP	= 0x22,
	ETH_SCR_RECV			= 0x40
};

enum {
	ETH_SIR_SEND_OK		= 0x10,
	ETH_SIR_TIMEOUT		= 0x08,
	ETH_SIR_RECV			= 0x04,
	ETH_SIR_DISCON		= 0x02,
	ETH_SIR_CON				= 0x01
};

enum {
	ETH_SSR_CLOSED				= 0x00,
	ETH_SSR_INIT					= 0x12, // Actually 13, Cortex doesn't seem to register the LSB
	ETH_SSR_LISTEN				= 0x14,
	ETH_SSR_SYNSENT				= 0x15,
	ETH_SSR_SYNRECV				= 0x16,
	ETH_SSR_ESTABLISHED		= 0x17,
	ETH_SSR_FIN_WAIT			= 0x18,
	ETH_SSR_CLOSING				= 0x1A,
	ETH_SSR_TIME_WAIT			= 0x1B,
	ETH_SSR_CLOSE_WAIT		= 0x1C,
	ETH_SSR_LAST_ACK			= 0x1D,
	ETH_SSR_UDP						= 0x22,
	ETH_SSR_IPRAW					= 0x32,
	ETH_SSR_MACRAW				= 0x42,
	ETH_SSR_PPPoE					= 0x5F
};

enum {
	ETH_SPROTO_IP   = 0,
  ETH_SPROTO_ICMP = 1,
  ETH_SPROTO_IGMP = 2,
  ETH_SPROTO_GGP  = 3,
  ETH_SPROTO_TCP  = 6,
  ETH_SPROTO_PUP  = 12,
  ETH_SPROTO_UDP  = 17,
  ETH_SPROTO_IDP  = 22,
  ETH_SPROTO_ND   = 77,
  ETH_SPROTO_RAW  = 255
};

void ETH_init(void);
void ETH_read_data(SOCKET s, volatile uint16_t src, volatile uint8_t * dst, uint16_t len);
void ETH_send_data_processing(SOCKET s, const uint8_t *data, uint16_t len);
void ETH_send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len);
void ETH_recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek);

inline void ETH_set_gateway_IP(uint8_t *addr);
inline void ETH_get_gateway_IP(uint8_t *addr);

inline void ETH_set_subnet_mask(uint8_t *addr);
inline void ETH_get_subnet_mask(uint8_t *addr);

inline void ETH_set_mac(uint8_t * addr);
inline void ETH_get_mac(uint8_t * addr);

inline void ETH_set_IP(uint8_t * addr);
inline void ETH_get_IP(uint8_t * addr);

inline void ETH_set_retransmission_time(uint16_t timeout);
inline void ETH_set_retransmission_count(uint8_t retry);

inline void ETH_exec_socket_cmd(SOCKET s, uint8_t cmd);
  
uint16_t ETH_get_TX_free_size(SOCKET s);
uint16_t ETH_get_RX_received_size(SOCKET s);

uint8_t ETH_write_8(uint16_t addr, uint8_t data);
uint16_t ETH_write_n(uint16_t addr, const uint8_t *buf, uint16_t len);

uint8_t ETH_read_8(uint16_t addr);
uint16_t ETH_read_n(uint16_t addr, uint8_t *buf, uint16_t len);

#define __GP_REGISTER8(name, address)										\
  static inline void ETH_write##name(uint8_t _data) {		\
    ETH_write_8(address, _data);												\
  }																											\
  static inline uint8_t ETH_read##name() {							\
    return ETH_read_8(address);													\
  }
#define __GP_REGISTER16(name, address)									\
  static void ETH_write##name(uint16_t _data) {					\
    ETH_write_8(address,   _data >> 8);									\
    ETH_write_8(address+1, _data & 0xFF);								\
  }																											\
  static uint16_t ETH_read##name() {										\
    uint16_t res = ETH_read_8(address);									\
    res = (res << 8) + ETH_read_8(address + 1);					\
    return res;																					\
  }
#define __GP_REGISTER_N(name, address, size)						\
  static uint16_t ETH_write##name(uint8_t *_buff) {			\
    return ETH_write_n(address, _buff, size);						\
  }																											\
  static uint16_t ETH_read##name(uint8_t *_buff) {			\
    return ETH_read_n(address, _buff, size);						\
  }

__GP_REGISTER8 (MR,     0x0000);    // Mode
__GP_REGISTER_N(GAR,    0x0001, 4); // Gateway IP address
__GP_REGISTER_N(SUBR,   0x0005, 4); // Subnet mask address
__GP_REGISTER_N(SHAR,   0x0009, 6); // Source MAC address
__GP_REGISTER_N(SIPR,   0x000F, 4); // Source IP address
__GP_REGISTER8 (IR,     0x0015);    // Interrupt
__GP_REGISTER8 (IMR,    0x0016);    // Interrupt Mask
__GP_REGISTER16(RTR,    0x0017);    // Timeout address
__GP_REGISTER8 (RCR,    0x0019);    // Retry count
__GP_REGISTER8 (RMSR,   0x001A);    // Receive memory size
__GP_REGISTER8 (TMSR,   0x001B);    // Transmit memory size
__GP_REGISTER8 (PATR,   0x001C);    // Authentication type address in PPPoE mode
__GP_REGISTER8 (PTIMER, 0x0028);    // PPP LCP Request Timer
__GP_REGISTER8 (PMAGIC, 0x0029);    // PPP LCP Magic Number
__GP_REGISTER_N(UIPR,   0x002A, 4); // Unreachable IP address in UDP mode
__GP_REGISTER16(UPORT,  0x002E);    // Unreachable Port address in UDP mode

#undef __GP_REGISTER8
#undef __GP_REGISTER16
#undef __GP_REGISTER_N

static inline uint8_t ETH_sock_read_8(SOCKET _s, uint16_t _addr);
static inline uint8_t ETH_sock_write_8(SOCKET _s, uint16_t _addr, uint8_t _data);
static inline uint16_t ETH_sock_read_n(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t len);
static inline uint16_t ETH_sock_write_n(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t len);

static const uint16_t ETH_CH_BASE = 0x0400;
static const uint16_t ETH_CH_SIZE = 0x0100;

#define __SOCKET_REGISTER8(name, address)														\
  static inline void ETH_write##name(SOCKET _s, uint8_t _data) {		\
    ETH_sock_write_8(_s, address, _data);														\
  }																																	\
  static inline uint8_t ETH_read##name(SOCKET _s) {									\
    return ETH_sock_read_8(_s, address);														\
  }
#define __SOCKET_REGISTER16(name, address)													\
  static void ETH_write##name(SOCKET _s, uint16_t _data) {					\
    ETH_sock_write_8(_s, address,   _data >> 8);										\
    ETH_sock_write_8(_s, address+1, _data & 0xFF);									\
  }																																	\
  static uint16_t ETH_read##name(SOCKET _s) {												\
    uint16_t res = ETH_sock_read_8(_s, address);										\
    uint16_t res2 = ETH_sock_read_8(_s,address + 1);								\
    res = res << 8;																									\
    res2 = res2 & 0xFF;																							\
    res = res | res2;																								\
    return res;																											\
  }
#define __SOCKET_REGISTER_N(name, address, size)										\
  static uint16_t ETH_write##name(SOCKET _s, uint8_t *_buff) {			\
    return ETH_sock_write_n(_s, address, _buff, size);							\
  }																																	\
  static uint16_t ETH_read##name(SOCKET _s, uint8_t *_buff) {				\
    return ETH_sock_read_n(_s, address, _buff, size);								\
  }

__SOCKET_REGISTER8(SnMR,        0x0000)        // Mode
__SOCKET_REGISTER8(SnCR,        0x0001)        // Command
__SOCKET_REGISTER8(SnIR,        0x0002)        // Interrupt
__SOCKET_REGISTER8(SnSR,        0x0003)        // Status
__SOCKET_REGISTER16(SnPORT,     0x0004)        // Source Port
__SOCKET_REGISTER_N(SnDHAR,     0x0006, 6)     // Destination Hardw Addr
__SOCKET_REGISTER_N(SnDIPR,     0x000C, 4)     // Destination IP Addr
__SOCKET_REGISTER16(SnDPORT,    0x0010)        // Destination Port
__SOCKET_REGISTER16(SnMSSR,     0x0012)        // Max Segment Size
__SOCKET_REGISTER8(SnPROTO,     0x0014)        // Protocol in IP RAW Mode
__SOCKET_REGISTER8(SnTOS,       0x0015)        // IP TOS
__SOCKET_REGISTER8(SnTTL,       0x0016)        // IP TTL
__SOCKET_REGISTER16(SnTX_FSR,   0x0020)        // TX Free Size
__SOCKET_REGISTER16(SnTX_RD,    0x0022)        // TX Read Pointer
__SOCKET_REGISTER16(SnTX_WR,    0x0024)        // TX Write Pointer
__SOCKET_REGISTER16(SnRX_RSR,   0x0026)        // RX Free Size
__SOCKET_REGISTER16(SnRX_RD,    0x0028)        // RX Read Pointer
__SOCKET_REGISTER16(SnRX_WR,    0x002A)        // RX Write Pointer (supported?)
  
#undef __SOCKET_REGISTER8
#undef __SOCKET_REGISTER16
#undef __SOCKET_REGISTER_N

static const int ETH_SOCKETS = 4;
static const uint16_t ETH_SMASK = 0x07FF; // Tx buffer MASK
static const uint16_t ETH_RMASK = 0x07FF; // Rx buffer MASK
static const uint16_t ETH_SSIZE = 2048; // Max Tx buffer size
static const uint16_t ETH_RSIZE = 2048; // Max Rx buffer size

extern uint16_t ETH_SBASE[ETH_SOCKETS]; // Tx buffer base address
extern uint16_t ETH_RBASE[ETH_SOCKETS]; // Rx buffer base address

uint8_t ETH_sock_read_8(SOCKET s, uint16_t addr)
{
	return ETH_read_8(ETH_CH_BASE + s * ETH_CH_SIZE + addr);
}

uint8_t ETH_sock_write_8(SOCKET s, uint16_t addr, uint8_t data)
{
	return ETH_write_8(ETH_CH_BASE + s * ETH_CH_SIZE + addr, data);
}

uint16_t ETH_sock_read_n(SOCKET s, uint16_t addr, uint8_t *buf, uint16_t len)
{
	return ETH_read_n(ETH_CH_BASE + s * ETH_CH_SIZE + addr, buf, len);
}

uint16_t ETH_sock_write_n(SOCKET s, uint16_t addr, uint8_t *buf, uint16_t len)
{
	return ETH_write_n(ETH_CH_BASE + s * ETH_CH_SIZE + addr, buf, len);
}

void ETH_set_gateway_IP(uint8_t *addr) { ETH_writeGAR(addr); }
void ETH_get_gateway_IP(uint8_t *addr) { ETH_readGAR(addr); }

void ETH_set_subnet_mask(uint8_t *addr) { ETH_writeSUBR(addr); }
void ETH_get_subnet_mask(uint8_t *addr) { ETH_readSUBR(addr); }

void ETH_set_mac(uint8_t * addr) { ETH_writeSHAR(addr); }
void ETH_get_mac(uint8_t * addr) { ETH_readSHAR(addr); }

void ETH_set_IP(uint8_t * addr) { ETH_writeSIPR(addr); }
void ETH_get_IP(uint8_t * addr) { ETH_readSIPR(addr); }

void ETH_set_retransmission_time(uint16_t timeout) { ETH_writeRTR(timeout); }
void ETH_set_retransmission_count(uint8_t retry) { ETH_writeRCR(retry); }

#endif
