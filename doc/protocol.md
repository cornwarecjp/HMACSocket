#hmacsocket protocol

Prior to connecting, client and server must be configured to use the same
secret key and hash function.
This protocol does not specify which hash function must be used.

When a client connects to a server, each sends the other an Init message.
After receiving the Init message, each can transmit data in the form of
Chunk messages and error messages.

All integers are big endian.


#Init message

This has the following structure:

* 2 bytes: **LH**: The length of the output of the hash function being used (in bytes).
* 4 bytes: **ML**: The maximum chunk data length, in bytes
* LH bytes: **N**: The nonce to be used in chunk messages.

The transmitting side
* MUST set LH to the length of the output of the hash function it uses.
* MUST set ML to a value greater than 0.
* MUST NOT set ML to a value greater than the chunk data length it can receive.
* MUST NOT set N to a value that has a significant probability of resulting in
  chunk or error message CN(i) values that have been used before with the same
  secret key.
* SHOULD set N to a cryptographically secure random number.

The receiving side
* SHOULD terminate the connection if LH does not correspond with the length of
  the output of the hash function it uses.


#Chunk message

This has the following structure:

* 4 bytes: **LD**: The length of the data, in bytes
* LH bytes: **H**: The HMAC
* LD bytes: **D**: The data

The correct HMAC for the chunk is defined as HMAC(K, D | CN(i)), where

* HMAC is the HMAC function based on the hash function being used.
* K is the secret key being used.
* CN(i) = H(N,K) + i is the nonce for this chunk:
  - N is the nonce value, as in the init message received by the transmitter of
    the chunk message.
  - H is the hash function being used.
  - + denotes big endian, unsigned integer addition.
* i is the index of the chunk message, such that the first transmitted chunk
  message has i = 0, the second has i = 1, and so on.
* | denotes concatenation.

The transmitting side
* MUST NOT transmit chunk messages with the length of D equal to 0.
* MUST NOT transmit chunk messages with the length of D than ML, as in the
  init message received by the transmitter of the chunk message.
* MUST set LD to the length of D.
* MUST set H to the correct HMAC for the chunk.

The receiving side
* SHOULD stop with an error if LD is greater than ML, as in the init message
  transmitted by the receiver of the chunk message.
* MUST stop with an error if LD is greater than the chunk data length it can
  receive.

The receiving side, in case H is set to the correct HMAC for the chunk:
* SHOULD accept the data.

The receiving side, in case H is not set to the correct HMAC for the chunk:
* MUST reject the data.
* SHOULD stop with an error.


#Error message

Error messages can be distinguished from Chunk messages as they have an LD value
of zero.

This has the following structure:

* 4 bytes: **LD**: Zero
* LH bytes: **H**: The HMAC
* 1 byte: **EC**: The error code
* 1 byte: **LE** The length of the error message, in bytes
* LE bytes: **EM**: The error message

The correct HMAC for the error message is defined in the same way as for chunk
messages.
Error messages also increment i in the same way as chunk messages, so that,
after two chunk messages and three error messages, i has increased by five.

The transmitting side
* MUST set LD to zero.
* MUST set H to the correct HMAC for the error message.
* MUST set LE to the length of EM.
* MUST NOT set EM to a value with a length greater than 256 bytes.
* MUST set EM to a human-readable UTF-8 encoded string.
* MUST NOT include a NUL terminator in EM.
* SHOULD include a description of the reason for the error in EM.

The receiving side, in case H is not set to the correct HMAC for the chunk:
* SHOULD ignore the error contents.
* SHOULD stop without sending an error.

The receiving side
* SHOULD NOT process EM if it is not properly UTF-8 encoded.

## Error codes

0x10: Data length too long
0x20: HMAC failure

# Notes

* Each side uses the N value from the Init message it *received* for the Chunk
  and Error messages it *transmits*, and uses the N value from the Init message
  it *transmitted* for the Chunk and Error messages it *receives*.
* The i value increments independently for messages received and for messages
  transmitted.
  So, receiving a message does not increment the i value to be used when
  transmitting a message, and transmitting a message does not increment the i
  value to be used when receiving a message.

