/*
    network.h
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

#ifndef NETWORK_H
#define NETWORK_H

/*
Start listening on the given TCP port.
This function blocks forever.
Whenever an incoming connection is established, a forked process is created,
which executes the given callback function.

hostname:     The hostname of the interface to bind to, or NULL to bind to all.
port:         The TCP port to bind to.
callback:     The function to call in a forked process when a connection is
              established.
              It receives these arguments:
              - The file descriptor of the TCP connection socket.
              - The callback data.
callbackData: Gets passed to the callback function.
*/
void listenOnPort(char *hostname, int port, void (*callback)(int, void *), void *callbackData);

/*
Connect to the given TCP port.

hostname: The hostname where the port is located.
port:     The TCP port to connect to.

Returns:  The file descriptor of the TCP connection socket.
*/
int connectToPort(char *hostname, int port);

/*
Reads all of the requested data.
Blocks as long as the requested amount of data is not available.

fd:     The file descriptor.
buffer: The buffer in which the data gets written.
        Must be count bytes.
count:  The number of bytes to read.
*/
void readAll(int fd, void *buffer, size_t count);

/*
Writes all of the given data.
Blocks as long as the requested amount of data cannot all be written.

fd:     The file descriptor.
buffer: The buffer from which the data gets read.
        Must be count bytes.
count:  The number of bytes to write.
*/
void writeAll(int fd, const void *buffer, size_t count);

#endif

