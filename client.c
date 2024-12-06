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

#include <unistd.h>

#include "network.h"
#include "protocol.h"


#define KEY "foobar"
#define KEYLEN 6

void serve(int fd)
{
	int serverfd = connectToPort("localhost", SERVERPORT);
	forwardData(fd, serverfd, KEY, KEYLEN);
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

