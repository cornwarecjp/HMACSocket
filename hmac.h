/*
    hmac.h
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

#ifndef HMAC_H
#define HMAC_H

//The length of HMAC values, in bytes (32 bytes for SHA256).
#define HMACLEN 32

/*
Calculate the nonce to be used for the first Chunk or Error message.
In the documentation, this is H(N|K).

key, keylen:       The secret key (K in the documentation).
initNonce:         The nonce value in the Init message (N in the documentation).
                   Must be HMACLEN bytes.
firstMessageNonce: The location where the output gets written.
                   Must be HMACLEN bytes.
*/
void getFirstMessageNonce(const void *key, unsigned int keylen,
	const unsigned char *initNonce,
	unsigned char *firstMessageNonce
	);

/*
Calculate the HMAC for a Chunk or Error message.

key, keylen:       The secret key (K in the documentation).
data, datalen:     The data (D in the documentation).
                   Must be at most MAX_MESSAGE_SIZE (settings.h) bytes.
nonce:             The nonce for this message (CN(i) in the documentation).
                   Must be HMACLEN bytes.
result:            The location where the output gets written.
                   Must be HMACLEN bytes.
*/
void getMessageHMAC(const void *key, unsigned int keylen,
	const unsigned char *data, unsigned int datalen,
	const unsigned char *nonce,
	unsigned char *result
	);

#endif

