/*
    protocol.h
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

#ifndef PROTOCOL_H
#define PROTOCOL_H

/*
Forward data between a regular TCP socket and a socket where communication
happens with the HMACSocket protocol.
This function blocks until either connection gets closed.

regularfd:       The file descriptor for the regular connection.
HMACfd:          The file descriptor for the HMACSocket connection.
key, keyLen:     The secret key (K in the documentation).
*/
void forwardData(int regularfd, int HMACfd, const unsigned char *key, unsigned int keyLen);

#endif

