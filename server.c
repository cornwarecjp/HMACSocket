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
#include <sys/socket.h>
#include <netinet/in.h>

#define CONNECTIONQUEUELEN 10
#define PORTNUMBER 5001



void serve(int fd)
{
	char buffer[256];
	int  n;

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


void listenOnPort(int port, void (*callback)(int))
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
			callback(newsockfd);
			exit(0);
		}
		else
		{
			close(newsockfd);
		}
	}
}


int main(int argc, char **argv)
{
	listenOnPort(PORTNUMBER, serve);
	return 0;
}

