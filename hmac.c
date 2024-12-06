/*
    hmac.c
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
#include <string.h>

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#include "hmac.h"
#include "settings.h"

void HMAC_SHA256(
	const void *key, unsigned int keylen,
	const unsigned char *data, unsigned int datalen,
	unsigned char *result
	)
{
	unsigned int len;
	HMAC(EVP_sha256(), key, keylen, data, datalen, result, &len);
}

void getFirstMessageNonce(const void *key, unsigned int keylen,
	const unsigned char *initNonce,
	unsigned char *firstMessageNonce
	)
{
	unsigned int len;

	EVP_MD_CTX *c = EVP_MD_CTX_new();
	EVP_DigestInit_ex2(c, EVP_sha256(), NULL);
	EVP_DigestUpdate(c, initNonce, HMACLEN);
	EVP_DigestUpdate(c, key, keylen);
	EVP_DigestFinal_ex(c, firstMessageNonce, &len);
	EVP_MD_CTX_free(c);
}

void getMessageHMAC(const void *key, unsigned int keylen,
	const unsigned char *data, unsigned int datalen,
	const unsigned char *nonce,
	unsigned char *result
	)
{
	char buffer[MAX_MESSAGE_SIZE + HMACLEN];
	unsigned int len;

	memcpy(buffer, data, datalen);
	memcpy(buffer+datalen, nonce, HMACLEN);

	HMAC(EVP_sha256(), key, keylen, buffer, datalen+HMACLEN, result, &len);
}

