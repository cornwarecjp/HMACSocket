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
#include <strings.h>

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "hmac.h"

#define PORTNUMBER 5001



int connectToPort(char *hostname, int port)
{
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("Couldn't open socket\n");
		exit(1);
	}

	server = gethostbyname(hostname);
	if(server == NULL)
	{
		perror("ERROR, no such host\n");
		exit(0);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("ERROR while connecting");
		exit(1);
	}
	
	return sockfd;
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

	int fd = connectToPort("localhost", PORTNUMBER);
	write(fd, "Bla", 3);
	write(fd, "quit", 4);

	return 0;
}

