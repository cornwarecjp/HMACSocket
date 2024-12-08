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

/*
Make a new random nonce value.

nonce: Location where the nonce gets written.
       Must be HMACLEN (hmac.h) bytes.
*/
void makeNonce(unsigned char *nonce);

/*
Write an Init message to a socket.

fd:    The file descriptor.
nonce: The nonce value to be put into the message.
       Must be HMACLEN (hmac.h) bytes.
*/
void writeInitMessage(int fd, const unsigned char *nonce);

/*
Read an Init message from a socket.

fd:               The file descriptor.
nonce:            Location where the nonce from the message gets written.
                  Must be HMACLEN (hmac.h) bytes.
maxMessageLength: Location where the maximum message length from the message
                  gets written.
                  Note that the value written here will never exceed
                  MAX_MESSAGE_SIZE (settings.h), even if the value in the
                  message does.
*/
void readInitMessage(int fd, unsigned char *nonce, uint32_t *maxMessageLength);

/*
Write a Chunk message to a socket.

fd:              The file descriptor.
nonce:           The nonce to be used for calculating the message HMAC
                 (CN(i) in the documentation).
                 Must be HMACLEN (hmac.h) bytes.
dataLen, buffer: The data to be put into the message.
key, keyLen:     The secret key (K in the documentation).
*/
void writeChunkMessage(int fd,
	unsigned char *nonce,
	uint32_t dataLen, const unsigned char *buffer,
	const unsigned char *key, unsigned int keyLen);

/*
Read a Chunk message from a socket.

fd:              The file descriptor.
nonce:           The nonce to be used for calculating the message HMAC
                 (CN(i) in the documentation).
                 Must be HMACLEN (hmac.h) bytes.
                 Before returning, this function increments the nonce value by
                 one, so that it equals the nonce value to be used for the next
                 message.
dataLen, buffer: Location where the data from the message gets written.
                 Must be at most MAX_MESSAGE_SIZE (settings.h) bytes.
key, keyLen:     The secret key (K in the documentation).
*/
void readChunkMessage(int fd,
	unsigned char *nonce,
	uint32_t *dataLen, unsigned char *buffer,
	const unsigned char *key, unsigned int keyLen);

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

