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

#ifndef __SOCKET_H
#define __SOCKET_H

#include "w5100.h"

uint8_t ETH_socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag);
uint8_t ETH_socket_status(SOCKET s);
void ETH_close(SOCKET s);
uint8_t ETH_connect(SOCKET s, uint8_t * addr, uint16_t port);
void ETH_disconnect(SOCKET s);
uint8_t ETH_listen(SOCKET s);
uint16_t ETH_send(SOCKET s, const uint8_t * buf, uint16_t len);
int16_t ETH_recv(SOCKET s, uint8_t * buf, int16_t len);
int16_t ETH_recv_available(SOCKET s);
uint16_t ETH_peek(SOCKET s, uint8_t *buf);
uint16_t ETH_sendto(SOCKET s, const uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port);
uint16_t ETH_recvfrom(SOCKET s, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t *port);
void ETH_flush(SOCKET s);

uint16_t ETH_igmpsend(SOCKET s, const uint8_t * buf, uint16_t len);

int ETH_start_UDP(SOCKET s, uint8_t* addr, uint16_t port);
uint16_t ETH_buffer_data(SOCKET s, uint16_t offset, const uint8_t* buf, uint16_t len);
int ETH_send_UDP(SOCKET s);

#endif
