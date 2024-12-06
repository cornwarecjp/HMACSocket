/*
    client.c
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

#include "hmac.h"
#include "network.h"
#include "protocol.h"



void serve(int fd)
{
	uint32_t maxWriteMessageLength = 0;
	char readNonce[HMACLEN], writeNonce[HMACLEN];
	struct pollfd pollList[2];

	int serverfd = connectToPort("localhost", SERVERPORT);

	makeNonce(readNonce);
	writeInitMessage(serverfd, readNonce);
	readInitMessage(serverfd, writeNonce, &maxWriteMessageLength);
	printf("Max write message length = %d\n", maxWriteMessageLength);

	pollList[0].fd = fd;
	pollList[1].fd = serverfd;
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
			ssize_t dataLen = read(fd, buffer, maxWriteMessageLength);
			if(dataLen < 0)
				break;
			if(dataLen > 0)
				writeChunkMessage(serverfd, writeNonce, dataLen, buffer);
		}

		if(pollList[1].revents & POLLIN)
		{
			uint32_t dataLength;
			readChunkMessage(serverfd, readNonce, &dataLength, buffer);
			write(fd, buffer, dataLength);
		}

	}

	close(serverfd);
	close(fd);
}


int main(int argc, char **argv)
{
	/*
	unsigned char *key = "key";
	unsigned char *data = "The quick brown fox jumps over the lazy dog";
	unsigned char result[HMACLEN];
	unsigned int resultlen = 0;
	
	HMAC_SHA256(
		key, strlen(key),
		data, strlen(data),
		result, &resultlen
		);

	for(unsigned int i=0; i < resultlen; i++)
		printf("%02x", result[i]);

	printf("\n");
	*/

	listenOnPort(CLIENTPORT, serve);

	return 0;
}

