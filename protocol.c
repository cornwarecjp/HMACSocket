/*
    protocol.c
    Copyright (C) 2024 by the HMACSocket developers

    This file is part of HMACSocket.

    HMACSocket is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    HMACSocket is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with HMACSocket. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>

#include "protocol.h"
#include "hmac.h"


#define RANDOMSOURCE "/dev/urandom"


void readAll(int fd, void *buffer, size_t count)
{
	ssize_t received = 0;
	while(count)
	{
		received = read(fd, buffer, count);

		if(received < 0)
			exit(0);

		/*
		for(unsigned int i=0; i<received; i++)
			printf("%02x", ((unsigned char *)buffer)[i]);
		printf("\n");
		*/

		buffer += received;
		count -= received;
	}
}


void makeNonce(unsigned char *nonce)
{
	FILE *fd = fopen(RANDOMSOURCE, "r");
	if(fread(nonce, HMACLEN, 1, fd) != 1)
	{
		perror("Failed to read random data\n");
		exit(1);
	}
	fclose(fd);
}


void writeInitMessage(int fd, const unsigned char *nonce)
{
	struct __attribute__((__packed__))
	{
		uint16_t hashLength;
		uint32_t maxMessageLength;
		char nonce[HMACLEN];
	} message;
	message.hashLength = htons(HMACLEN);
	message.maxMessageLength = htonl(MAX_MESSAGE_SIZE);
	memcpy(message.nonce, nonce, HMACLEN);

	write(fd, &message, sizeof(message));
}

void readInitMessage(int fd, unsigned char *nonce, uint32_t *maxMessageLength)
{
	uint16_t hashLength_bigEndian;
	uint32_t maxMessageLength_bigEndian;
	readAll(fd, &hashLength_bigEndian, sizeof(hashLength_bigEndian));
	if(ntohs(hashLength_bigEndian) != HMACLEN)
	{
		perror("Hash length mismatch\n");
		exit(1);
	}

	readAll(fd, &maxMessageLength_bigEndian, sizeof(maxMessageLength_bigEndian));
	*maxMessageLength = ntohl(maxMessageLength_bigEndian);
	if(*maxMessageLength > MAX_MESSAGE_SIZE)
		*maxMessageLength = MAX_MESSAGE_SIZE;

	readAll(fd, nonce, HMACLEN);
}
