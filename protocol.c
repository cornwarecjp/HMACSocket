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
#include <poll.h>

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

void writeChunkMessage(int fd, unsigned char *nonce, uint32_t dataLen, const unsigned char *data)
{
	struct __attribute__((__packed__))
	{
		uint32_t dataLength;
		char HMAC[HMACLEN];
		char data[MAX_MESSAGE_SIZE];
	} message;
	message.dataLength = htonl(dataLen);
	//TODO: HMAC writing
	memcpy(message.data, data, dataLen);

	write(fd, &message, 4 + HMACLEN + dataLen);
}

void readChunkMessage(int fd, unsigned char *nonce, uint32_t *dataLen, unsigned char *buffer)
{
	uint32_t dataLength;
	char HMAC[HMACLEN];

	readAll(fd, &dataLength, sizeof(dataLength));
	dataLength = ntohl(dataLength);
	*dataLen = dataLength;
	if(dataLength == 0)
	{
		//TODO: read error message instead
	}
	if(dataLength > MAX_MESSAGE_SIZE)
	{
		perror("Received too big data size\n");
		exit(1);
	}

	readAll(fd, HMAC, sizeof(HMAC));

	readAll(fd, buffer, dataLength);

	//TODO: HMAC checking
}

void forwardData(int regularfd, int HMACfd)
{
	uint32_t maxWriteMessageLength = 0;
	char readNonce[HMACLEN], writeNonce[HMACLEN];
	struct pollfd pollList[2];

	makeNonce(readNonce);
	writeInitMessage(HMACfd, readNonce);
	readInitMessage(HMACfd, writeNonce, &maxWriteMessageLength);
	printf("Max write message length = %d\n", maxWriteMessageLength);

	pollList[0].fd = regularfd;
	pollList[1].fd = HMACfd;
	pollList[0].events = POLLIN;
	pollList[1].events = POLLIN;

	while(1)
	{
		char buffer[MAX_MESSAGE_SIZE];

		if(poll(pollList, 2, -1) < 0)
		{
			perror("poll error\n");
			exit(1);
		}

		for(unsigned int i=0; i<2; i++)
			if(pollList[i].revents & (POLLHUP | POLLERR | POLLNVAL))
				break;

		if(pollList[0].revents & POLLIN)
		{
			ssize_t dataLen = read(regularfd, buffer, maxWriteMessageLength);
			if(dataLen < 0)
				break;
			if(dataLen > 0)
				writeChunkMessage(HMACfd, writeNonce, dataLen, buffer);
		}

		if(pollList[1].revents & POLLIN)
		{
			uint32_t dataLength;
			readChunkMessage(HMACfd, readNonce, &dataLength, buffer);
			write(regularfd, buffer, dataLength);
		}

	}
}

