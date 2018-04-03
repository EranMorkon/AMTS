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

#include "socket.h"

// Check if address is a net addres 0.0.0.0
#define is_net_addr(addr)  			 ((addr[0] == 0x00) &&	\
																	(addr[1] == 0x00) &&	\
																	(addr[2] == 0x00) &&	\
																	(addr[3] == 0x00))

// Check if address is broadcast address 255.255.255.255
#define is_broadcast_addr(addr)  ((addr[0] == 0xFF) &&	\
																	(addr[1] == 0xFF) &&	\
																	(addr[2] == 0xFF) &&	\
																	(addr[3] == 0xFF))

// Check if port is valid (not 0)
#define is_valid_port(port) 		 (port != 0x00)

static uint16_t local_port;

uint8_t ETH_socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag)
{
  // Check if protocol is supported
	if ((protocol == ETH_SMR_TCP) || (protocol == ETH_SMR_UDP) || (protocol == ETH_SMR_IPRAW) || (protocol == ETH_SMR_MACRAW) || (protocol == ETH_SMR_PPPoE))
  {
    // Close socket, if open
    ETH_close(s);
    // Set the mode register to the protocol, plus eventual flags
    ETH_writeSnMR(s, protocol | flag);
    if (port != 0) {
      // If port is not 0 (0 on client mode) write port to the port register
      ETH_writeSnPORT(s, port);
    } 
    else {
      // Write a random port to the port register (for client mode)
      local_port++;
      ETH_writeSnPORT(s, local_port);
    }

    // Execute the open socket command
    ETH_exec_socket_cmd(s, ETH_SCR_OPEN);
    return 1;
  }
  return 0;
}

uint8_t ETH_socket_status(SOCKET s)
{
  // Read the status register and return it
	uint8_t tmp = ETH_readSnSR(s);
  return tmp;
}

void ETH_close(SOCKET s)
{
  // Execute close socket and disable interrupts
	ETH_exec_socket_cmd(s, ETH_SCR_CLOSE);
  ETH_writeSnIR(s, 0xFF);
}

uint8_t ETH_connect(SOCKET s, uint8_t * addr, uint16_t port)
{
  // Check if port and address is valid
	if (is_net_addr(addr) || is_broadcast_addr(addr) || !is_valid_port(port)) 
    return 0;

  // Write IP to destination register
  ETH_writeSnDIPR(s, addr);
  // Write port to destination register
  ETH_writeSnDPORT(s, port);
  // Execute connect
  ETH_exec_socket_cmd(s, ETH_SCR_CONNECT);
  return 1;
}

void ETH_disconnect(SOCKET s)
{
  // Execute disconnect
	ETH_exec_socket_cmd(s, ETH_SCR_DISCON);
}

uint8_t ETH_listen(SOCKET s)
{
  // If state is not initzialized, return
  if (ETH_readSnSR(s) != ETH_SSR_INIT) {
    return 0;
  }
  // Execute the listen command
  ETH_exec_socket_cmd(s, ETH_SCR_LISTEN);
  return 1;
}

uint16_t ETH_send(SOCKET s, const uint8_t * buf, uint16_t len)
{
	uint8_t status=0;
  uint16_t ret=0;
  uint16_t freesize=0;

  // If data is bigger then Tx memory, split it
  if (len > ETH_SSIZE) 
    ret = ETH_SSIZE;
  else 
    ret = len;

  do 
  {
    // Write data to Tx memory
    freesize = ETH_get_TX_free_size(s);
    status = ETH_readSnSR(s);
    if ((status != ETH_SSR_ESTABLISHED) && (status != ETH_SSR_CLOSE_WAIT))
    {
      ret = 0; 
      break;
    }
  } 
  while (freesize < ret);

  // Start data processing and execute send command
  ETH_send_data_processing(s, (uint8_t *)buf, ret);
  ETH_exec_socket_cmd(s, ETH_SCR_SEND);

  // Check interrupt if everything is okay
  while ((ETH_readSnIR(s) & ETH_SIR_SEND_OK) != ETH_SIR_SEND_OK ) 
  {
    // If socket is closed set the state locally right
    if (ETH_readSnSR(s) == ETH_SSR_CLOSED )
    {
      ETH_close(s);
      return 0;
    }
  }
  // Mark interrupt as handled
  ETH_writeSnIR(s, ETH_SIR_SEND_OK);
  return ret;
}

int16_t ETH_recv(SOCKET s, uint8_t * buf, int16_t len)
{
  // Check remaining size
  int16_t ret = ETH_get_RX_received_size(s);
  if (ret == 0)
  {
    // Read status
    uint8_t status = ETH_readSnSR(s);
    // If status is wrong, return 0 read bytes
    if (status == ETH_SSR_LISTEN || status == ETH_SSR_CLOSED || status == ETH_SSR_CLOSE_WAIT)
    {
      ret = 0;
    }
    else
    {
      // Return -1 if there is nothing to read
      ret = -1;
    }
  }
  // If there is more to read then we have space, just read to the supplied limit
  else if (ret > len)
  {
    ret = len;
  }

  // If there is data to read and we have space, beginn processing and reading
  if (ret > 0)
  {
    ETH_recv_data_processing(s, buf, ret, 0x00);
    ETH_exec_socket_cmd(s, ETH_SCR_RECV);
  }
  return ret;
}

int16_t ETH_recv_available(SOCKET s)
{
  // Check if data is available to read
	return ETH_get_RX_received_size(s);
}

uint16_t ETH_peek(SOCKET s, uint8_t *buf)
{
  // Receive one byte
	ETH_recv_data_processing(s, buf, 1, 1);
	return 1;
}

uint16_t ETH_sendto(SOCKET s, const uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port)
{
	uint16_t ret=0;

  // If write size is bigger then memory, limit to memory
  if (len > ETH_SSIZE) ret = ETH_SSIZE;
  else ret = len;

  // Check if address and port is right
  if (is_net_addr(addr) || !is_valid_port(port) || ret == 0) {
    ret = 0;
  }
  else
  {
    // Set address and port to send to
    ETH_writeSnDIPR(s, addr);
    ETH_writeSnDPORT(s, port);

    // Process data and send it
    ETH_send_data_processing(s, (uint8_t *)buf, ret);
    ETH_exec_socket_cmd(s, ETH_SCR_SEND);

    // Wait for the data to be sent
    while ((ETH_readSnIR(s) & ETH_SIR_SEND_OK) != ETH_SIR_SEND_OK) 
    {
      // If time out reset send and timout interrupts
      if (ETH_readSnIR(s) & ETH_SIR_TIMEOUT)
      {
        ETH_writeSnIR(s, (ETH_SIR_SEND_OK | ETH_SIR_TIMEOUT));
        return 0;
      }
    }

    // Reset send ok interrupt
    ETH_writeSnIR(s, ETH_SIR_SEND_OK);
  }
  return ret;
}

uint16_t ETH_recvfrom(SOCKET s, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t *port)
{
	uint8_t head[8];
  uint16_t data_len=0;
  uint16_t ptr=0;

  // Check if there is something to receive
  if (len > 0)
  {
    // Get the receive pointer
    ptr = ETH_readSnRX_RD(s);
    // Handle receiving according to socket type
    switch (ETH_readSnMR(s) & 0x07)
    {
    case ETH_SMR_UDP :
      ETH_read_data(s, ptr, head, 0x08);
      ptr += 8;
      addr[0] = head[0];
      addr[1] = head[1];
      addr[2] = head[2];
      addr[3] = head[3];
      *port = head[4];
      *port = (*port << 8) + head[5];
      data_len = head[6];
      data_len = (data_len << 8) + head[7];

      ETH_read_data(s, ptr, buf, data_len);
      ptr += data_len;

      ETH_writeSnRX_RD(s, ptr);
      break;

    case ETH_SMR_IPRAW :
      ETH_read_data(s, ptr, head, 0x06);
      ptr += 6;

      addr[0] = head[0];
      addr[1] = head[1];
      addr[2] = head[2];
      addr[3] = head[3];
      data_len = head[4];
      data_len = (data_len << 8) + head[5];

      ETH_read_data(s, ptr, buf, data_len);
      ptr += data_len;

      ETH_writeSnRX_RD(s, ptr);
      break;

    case ETH_SMR_MACRAW:
      ETH_read_data(s, ptr, head, 2);
      ptr+=2;
      data_len = head[0];
      data_len = (data_len<<8) + head[1] - 2;

      ETH_read_data(s, ptr, buf, data_len);
      ptr += data_len;
      ETH_writeSnRX_RD(s, ptr);
      break;

    default :
      break;
    }
    ETH_exec_socket_cmd(s, ETH_SCR_RECV);
  }
  return data_len;
}

void ETH_flush(SOCKET s)
{
	// TODO
}

uint16_t ETH_igmpsend(SOCKET s, const uint8_t * buf, uint16_t len)
{
	uint16_t ret=0;

  if (len > ETH_SSIZE) 
    ret = ETH_SSIZE;
  else 
    ret = len;

  if (ret == 0)
    return 0;

  // Process and send IGMP data
  ETH_send_data_processing(s, (uint8_t *)buf, ret);
  ETH_exec_socket_cmd(s, ETH_SCR_SEND);

  // Wairt for data to be sent or timeout
  while ((ETH_readSnIR(s) & ETH_SIR_SEND_OK) != ETH_SIR_SEND_OK) 
  {
    if (ETH_readSnIR(s) & ETH_SIR_TIMEOUT)
    {
      ETH_close(s);
      return 0;
    }
  }

  // Reset interrupt
  ETH_writeSnIR(s, ETH_SIR_SEND_OK);
  return ret;
}

int ETH_start_UDP(SOCKET s, uint8_t* addr, uint16_t port)
{
  // Check if address and port are valid
	if (is_net_addr(addr) || !is_valid_port(port))
  {
    return 0;
  }
  else
  {
    // Write IP and port
    ETH_writeSnDIPR(s, addr);
    ETH_writeSnDPORT(s, port);
    return 1;
  }
}

uint16_t ETH_buffer_data(SOCKET s, uint16_t offset, const uint8_t* buf, uint16_t len)
{
	uint16_t ret = 0;
  if (len > ETH_get_TX_free_size(s))
  {
    ret = ETH_get_TX_free_size(s);
  }
  else
  {
    ret = len;
  }
  ETH_send_data_processing_offset(s, offset, buf, ret);
  return ret;
}

int ETH_send_UDP(SOCKET s)
{
  ETH_exec_socket_cmd(s, ETH_SCR_SEND);

  while ((ETH_readSnIR(s) & ETH_SIR_SEND_OK) != ETH_SIR_SEND_OK) 
  {
    if (ETH_readSnIR(s) & ETH_SIR_TIMEOUT)
    {
      ETH_writeSnIR(s, (ETH_SIR_SEND_OK | ETH_SIR_TIMEOUT));
      return 0;
    }
  }

  ETH_writeSnIR(s, ETH_SIR_SEND_OK);
  return 1;
}
