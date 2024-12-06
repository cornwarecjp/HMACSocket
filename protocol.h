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

#include <stdint.h>

#define MAX_MESSAGE_SIZE 1048576 //1 MiB

void makeNonce(unsigned char *nonce);

void writeInitMessage(int fd, const unsigned char *nonce);
void readInitMessage(int fd, unsigned char *nonce, uint32_t *maxMessageLength);

void writeChunkMessage(int fd,
	unsigned char *nonce,
	uint32_t dataLen, const unsigned char *buffer,
	const unsigned char *key, unsigned int keyLen);
void readChunkMessage(int fd,
	unsigned char *nonce,
	uint32_t *dataLen, unsigned char *buffer,
	const unsigned char *key, unsigned int keyLen);

void forwardData(int regularfd, int HMACfd, const unsigned char *key, unsigned int keyLen);

#endif

