#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "uart_socket.h"

/***********************************************************/
/***********************************************************/

int CUARTSocket::Open(const char* pch_port, unsigned int un_baud) {
	m_nPort = ::open(pch_port, O_RDWR | O_NOCTTY | O_NDELAY);
	if (m_nPort == -1) {
		return errno;
	}
	/* O_NONBLOCK might be needed for write */
	fcntl(m_nPort, F_SETFL, 0);
	/* store current settings in structure */
	struct termios sPortSettings;
	tcgetattr(m_nPort, &sPortSettings);
	/* set speed */
	speed_t eBaud;
	switch(un_baud) {
	case(9600):
		eBaud = B9600;
		break;
	case(19200):
		eBaud = B19200;
		break;
	case(38400):
		eBaud = B38400;
		break;
	case(115200):
		eBaud = B115200;
		break;
	default:
		return -1;
	}
	/* set baud rates */
	cfsetispeed(&sPortSettings, eBaud);
	cfsetospeed(&sPortSettings, eBaud);
	/* set no parity, stop bits, 8 data bits */
	sPortSettings.c_cflag &= ~PARENB;
	sPortSettings.c_cflag &= ~CSTOPB;
	sPortSettings.c_cflag &= ~CSIZE;
	sPortSettings.c_cflag |= CS8;
	/* apply the settings to the port */
	tcsetattr(m_nPort, TCSANOW, &sPortSettings);
}

/***********************************************************/
/***********************************************************/

int CUARTSocket::Close() {
	int nRet = ::close(m_nPort);
	m_nPort = -1;
	return nRet;
}

/***********************************************************/
/***********************************************************/

ssize_t CUARTSocket::Read(void* p_buffer, size_t un_length) const {
	return ::read(m_nPort, p_buffer, un_length);
}

/***********************************************************/
/***********************************************************/

ssize_t CUARTSocket::Write(const void* p_buffer, size_t un_length) const {
	return ::write(m_nPort, p_buffer, un_length);
}
