/*
    args.h
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

#ifndef ARGS_H
#define ARGS_H

struct arguments {
	//The secret key (this is the key itself, not the filename):
    char *key;
    unsigned int keyLength;

	//The listen host (or NULL if not specified) and port:    
    char *listenHost;
    int listenPort;

	//The connect host and port:
    char *connectHost;
    int connectPort;
};

/*
Parse commandline arguments and return the values.
*/
struct arguments pargseArgs(int argc, char **argv);

#endif

