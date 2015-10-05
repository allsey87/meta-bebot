#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "tcp_image_socket.h"

#define MAGIC 0x17923349ab10ea9aL

int CTCPImageSocket::Open(const char* host, int port) {
	fprintf(stderr, "Connecting to '%s' on port %d\n", host, port);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock  < 0) {
		fprintf(stderr, "Error opening socket\n");
		perror("socket: ");
		if (sock >= 0)
			close(sock);
	}

	struct hostent *server = gethostbyname(host);
	if (server == NULL) {
		fprintf(stderr, "Error getting host by name\n");
		perror("gethostbyname: ");
		if (sock >= 0)
			close(sock);
	}

	struct sockaddr_in serv_addr;
	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(port);
	bcopy((char *)server->h_addr,
	      (char *)&serv_addr.sin_addr.s_addr,
	      server->h_length);

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "Error connecting to socket\n");
		perror("connect: ");
		if (sock >= 0)
			close(sock);
	}

	fprintf(stderr, "Connected\n");
}

CTCPImageSocket::~CTCPImageSocket() {
	if (sock >= 0)
		close(sock);
}

int CTCPImageSocket::Write(uint8_t* pun_data, unsigned int un_width, unsigned int un_height) {
	int64_t magic = MAGIC;

	int64_t utime = GetTime();

	const char *format = "GRAY8";
	int32_t formatlen = strlen(format);

	int32_t unImageLength = un_width * un_height * sizeof(uint8_t);

	int32_t buflen = 8 + 8 + 4 + 4 + 4 + formatlen + 4 + unImageLength;
	uint8_t *buf = (uint8_t*)calloc(buflen, sizeof(uint8_t));
	uint8_t *ptr = buf;

	Write64(ptr, magic);                    ptr += 8;
	Write64(ptr, utime);                    ptr += 8;
	Write32(ptr, un_width);                 ptr += 4;
	Write32(ptr, un_height);                ptr += 4;
	Write32(ptr, formatlen);                ptr += 4;
	memcpy(ptr, format, formatlen);         ptr += formatlen;
	Write32(ptr, unImageLength);            ptr += 4;
	memcpy(ptr, pun_data, unImageLength);   ptr += unImageLength;

	int bytes = send(sock, buf, buflen, 0);

	free(buf);

	if (bytes != buflen) {
		fprintf(stderr, "Tried to send %d bytes, sent %d\n", buflen, bytes);
		perror("send: ");
		return -1;
	}
	return 0;
}


int64_t CTCPImageSocket::GetTime() {
	struct timeval tv;
	gettimeofday (&tv, NULL);
	return (int64_t) tv.tv_sec * 1000000 + tv.tv_usec;
}

void CTCPImageSocket::Write32(uint8_t *buf, int32_t v) {
	buf[0] = (v >> 24) & 0xFF;
	buf[1] = (v >> 16) & 0xFF;
	buf[2] = (v >>  8) & 0xFF;
	buf[3] = (v      ) & 0xFF;
}

void CTCPImageSocket::Write64(uint8_t *buf, int64_t v) {
	uint32_t h = (uint32_t) (v >> 32);
	uint32_t l = (uint32_t) (v);

	Write32(buf+0, h);
	Write32(buf+4, l);
}

