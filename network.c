/*
    network.c
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
#include <strings.h>

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "network.h"

#define CONNECTIONQUEUELEN 10



void listenOnPort(int port, void (*callback)(int, void *), void *callbackData)
{
	int sockfd, newsockfd, clilen;
	struct sockaddr_in serv_addr, cli_addr;
	const int enable = 1;
	pid_t pid;
		
	bzero((char *) &serv_addr, sizeof(serv_addr));

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("Couldn't open socket\n");
		exit(1);
	}

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		perror("setsockopt(SO_REUSEADDR) failed\n");
		exit(1);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("ERROR on binding\n");
		exit(1);
	}

	listen(sockfd, CONNECTIONQUEUELEN);

	while(1)
	{
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
		pid = fork();
		if(pid == 0)
		{
			close(sockfd);
			callback(newsockfd, callbackData);
			exit(0);
		}
		else
		{
			close(newsockfd);
		}
	}
}


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


void writeAll(int fd, const void *buffer, size_t count)
{
	ssize_t sent = 0;
	while(count)
	{
		sent = write(fd, buffer, count);

		if(sent < 0)
			exit(0);

		/*
		for(unsigned int i=0; i<received; i++)
			printf("%02x", ((unsigned char *)buffer)[i]);
		printf("\n");
		*/

		buffer += sent;
		count -= sent;
	}
}

