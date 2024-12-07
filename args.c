/*
    args.c
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

#include <argp.h>

#include "args.h"



int parse_opt(int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;
	switch (key)
	{
	case 'k':
		arguments->keyFile = arg;
		break;
	case 'l':
		{
			char *pos = strchr(arg, ':');
			if(pos)
			{
				*pos = '\0';
				arguments->listenHost = arg;
				arguments->listenPort = atoi(pos+1);
			}
			else
			{
				arguments->listenPort = atoi(arg);
			}
		}
		break;
	case 'c':
		{
			char *pos = strchr(arg, ':');
			if(pos)
			{
				*pos = '\0';
				arguments->connectHost = arg;
				arguments->connectPort = atoi(pos+1);
			}
			else
			{
				perror("Connect port must have the form host:port");
				exit(1);
			}
		}
		break;
	case ARGP_KEY_END:
		if(arguments->keyFile == NULL)
		{
			perror("No key file was specified");
			exit(1);
		}
		if(arguments->listenPort < 0)
		{
			perror("No listen port was specified");
			exit(1);
		}
		if(arguments->connectPort < 0)
		{
			perror("No connect port was specified");
			exit(1);
		}
	}
	return 0;
}


struct arguments pargseArgs(int argc, char **argv)
{
	struct arguments ret;

	struct argp_option options[] =
	{
		{ "keyfile", 'k', "filename", 0, "Key file"},
		{ "listen" , 'l', "[host:]port", 0, "Port to listen on"},
		{ "connect", 'c', "host:port", 0, "Port to connect to"},
		{ 0 }
	};
	struct argp argp = { options, parse_opt };

	ret.keyFile = NULL;
	ret.listenHost = NULL;
	ret.listenPort = -1;
	ret.connectHost = NULL;
	ret.connectPort = -1;
	argp_parse (&argp, argc, argv, 0, 0, &ret);

	if(ret.keyFile)
		printf("Key file: %s\n", ret.keyFile);

	if(ret.listenHost)
		printf("Listen host: %s\n", ret.listenHost);
	printf("Listen port: %d\n", ret.listenPort);

	if(ret.connectHost)
		printf("Connect host: %s\n", ret.connectHost);
	printf("Connect port: %d\n", ret.connectPort);

	return ret;
}

