#!/usr/bin/env python3

import hashlib
import hmac
import os
import socket
import subprocess
import unittest
import struct
import time



HMACPORT = 5001
REGULARPORT = 5002
KEY = b'FooBar'
KEYFILE = 'testKey.dat'

HASHLEN = 32



sha256 = lambda data: hashlib.sha256(data).digest()
hmac_sha256 = lambda key, data: hmac.new(key, data, hashlib.sha256).digest()


def writeAll(fd, data):
	while data:
		sent = fd.send(data)
		data = data[sent:]


def readAll(fd, size):
	ret = b''
	while size:
		newData = fd.recv(size)
		if not newData:
			raise Exception('Connection has closed')
		ret += newData
		size -= len(newData)
	return ret


incrementNonce = lambda nonce: (int.from_bytes(nonce, byteorder='big') + 1).to_bytes(length=HASHLEN, byteorder='big')



class HMACProtocol:
	def __init__(self, fd, key):
		self.fd = fd
		self.key = key
		self.readNonce = None
		self.writeNonce = None
		self.maxReadLen = None
		self.maxWriteLen = None


	def writeInit(self, maxDataLen, nonce):
		writeAll(self.fd,
			struct.pack('!HI', len(nonce), maxDataLen) + nonce
			)
		self.readNonce = sha256(nonce + self.key)
		self.maxReadLen = maxDataLen


	def readInit(self):
		data = readAll(self.fd, 6)
		hashLen, maxDataLen = struct.unpack('!HI', data)
		if hashLen != HASHLEN:
			raise Exception('Received incorrect hash length')
		nonce = readAll(self.fd, hashLen)
		self.writeNonce = sha256(nonce + self.key)
		self.maxWriteLen = maxDataLen


	def readChunk(self):
		data = readAll(self.fd, 4)
		dataLen = struct.unpack('!I', data)[0]
		if dataLen == 0:
			raise Exception('Received Error chunk')
		if dataLen > self.maxReadLen:
			raise Exception('More data than allowed')
		HMAC = readAll(self.fd, HASHLEN)
		data = readAll(self.fd, dataLen)
		expectedHMAC = hmac_sha256(self.key, data + self.readNonce)
		if not hmac.compare_digest(HMAC, expectedHMAC):
			raise Exception('Received incorrect HMAC')
		self.readNonce = incrementNonce(self.readNonce)
		return data


	def writeChunk(self, data):
		HMAC = hmac_sha256(self.key, data + self.writeNonce)
		writeAll(self.fd, struct.pack('!I', len(data)) + HMAC + data)
		self.writeNonce = incrementNonce(self.writeNonce)



class TestServer(unittest.TestCase):
	def setUp(self):
		self.listenSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.listenSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		self.listenSocket.bind(('localhost', REGULARPORT))
		self.listenSocket.listen(5)

		with open(KEYFILE, 'wb') as f:
			f.write(KEY)

		self.process = subprocess.Popen([
			'./hmacsocketserver',
			'-k', KEYFILE,
			'-l', 'localhost:' + str(HMACPORT),
			'-c', 'localhost:' + str(REGULARPORT),
			])
		time.sleep(0.1)


	def tearDown(self):
		#First terminate child processes:
		os.system('pkill -TERM -P ' + str(self.process.pid))

		#Then the process itself:
		self.process.terminate()
		self.process.wait()
		
		self.listenSocket.close()


	def setupSocketPair(self, maxDataLen, nonce):
		HMACSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		HMACSocket.connect(('localhost', HMACPORT))
		protocol = HMACProtocol(HMACSocket, KEY)
		protocol.writeInit(maxDataLen, nonce)
		protocol.readInit()
		regularSocket, address = self.listenSocket.accept()
		return HMACSocket, regularSocket, protocol


	def test_sendsCorrectData(self):
		try:
			HMACSocket, regularSocket, protocol = self.setupSocketPair(
				maxDataLen=100, nonce=b'A'*HASHLEN)

			sentData = b'Foobar'
			for i in range(10):
				writeAll(regularSocket, sentData)

				receivedData = b''
				while len(receivedData) < len(sentData):
					receivedData += protocol.readChunk()
				self.assertEqual(receivedData, sentData)

		finally:
			HMACSocket.close()
			regularSocket.close()


	def test_acceptsCorrectHMACAndData(self):
		try:
			HMACSocket, regularSocket, protocol = self.setupSocketPair(
				maxDataLen=100, nonce=b'A'*HASHLEN)

			sentData = b'Foobar'
			for i in range(10):
				protocol.writeChunk(sentData)

				receivedData = readAll(regularSocket, len(sentData))
				self.assertEqual(receivedData, sentData)

		finally:
			HMACSocket.close()
			regularSocket.close()



if __name__ == '__main__':
	unittest.main(verbosity=3)

