#ifndef UART_SOCKET_H
#define UART_SOCKET_H

#include <stdint.h>

class CUARTSocket {
	/* file descriptor for the serial port */
	int m_nPort; 
public:

	CUARTSocket() : m_nPort(-1) {}

	~CUARTSocket() {
		if(m_nPort != -1) {
			Close();
		}
	}

	/*  open a serial port connection */
	int Open(const char* pch_port, unsigned int un_baud = 115200);

	/* close the connection */
	int Close();

	/* read into buffer */ 
	ssize_t Read(void* p_buffer, size_t un_length) const;

	/* write from buffer */
	ssize_t Write(const void* p_buffer, size_t un_length) const;

};

#endif
