#ifndef TCP_IMAGE_SOCKET_H
#define TCP_IMAGE_SOCKET_H

#include <stdint.h>

class CTCPImageSocket {
public:


	int Open(const char* host, int port);

	~CTCPImageSocket();

	int Write(uint8_t* pun_data, unsigned int un_width, unsigned int un_height);

private:
	int sock;

	int64_t GetTime();

	void Write32(uint8_t *buf, int32_t v);

	void Write64(uint8_t *buf, int64_t v);
};

#endif
