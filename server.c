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
	char buffer[256];
	int  n;

	uint32_t maxWriteMessageLength = 0;
	char writeNonce[HMACLEN];
	readInitMessage(fd, writeNonce, &maxWriteMessageLength);
	printf("Max write message length = %d\n", maxWriteMessageLength);

	while(1)
	{
		bzero(buffer,256);
		n = read(fd, buffer, 255);

		if(n < 0)
		{
			perror("ERROR in reading from socket");
			exit(1);
		}

		printf("client said: %s \n", buffer);

		n = write(fd, buffer, strlen(buffer));

		if(n < 0)
		{
			perror("ERROR in writing to socket");
			exit(1);
		}

		if(!memcmp(buffer, "quit", 4))
			break;
	}
	
	close(fd);
}


int main(int argc, char **argv)
{
	listenOnPort(SERVERPORT, serve);
	return 0;
}

