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

#define HMACLEN 32

void getFirstMessageNonce(const void *key, unsigned int keylen,
	const unsigned char *initNonce,
	unsigned char *firstMessageNonce
	);

void getMessageHMAC(const void *key, unsigned int keylen,
	const unsigned char *data, unsigned int datalen,
	const unsigned char *nonce,
	unsigned char *result
	);

#endif

