/*
    server.c
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
#include <strings.h>

#include <unistd.h>

#include "hmac.h"
#include "network.h"
#include "protocol.h"



void serve(int fd)
{
	uint32_t maxWriteMessageLength = 0;
	char readNonce[HMACLEN], writeNonce[HMACLEN];
	char buffer[MAX_MESSAGE_SIZE];
	uint32_t dataLength;

	makeNonce(readNonce);
	writeInitMessage(fd, readNonce);
	readInitMessage(fd, writeNonce, &maxWriteMessageLength);
	printf("Max write message length = %d\n", maxWriteMessageLength);

	while(1)
	{
		readChunkMessage(fd, readNonce, &dataLength, buffer);
		buffer[dataLength] = 0;
		printf("Received: %s\n", buffer);

		writeChunkMessage(fd, writeNonce, dataLength, buffer);
	}
	
	close(fd);
}


int main(int argc, char **argv)
{
	listenOnPort(SERVERPORT, serve);
	return 0;
}

