#Known vulnerabilities of HMACSocket

When using the current version of HMACSocket, you should be aware of the
following:

#Denial of Service
No Denial of Service protection has been implemented.

An attacker without access to the secret can still connect to the HMACSocket
server.
This will cause HMACSocket server to fork off a new process, which will connect
to the to-be-forwarded port.
Therefore, the attacker can flood the server with new processes and connections.
These processes and connections continue to exist as long as there is no reason
to clean them up.

HMACSocket server's forked per-connection processes will terminate and close
their connections in case
* The attacker sends data that needs authentication (but has incorrect
  authentication).
* The attacker closes its connection to the process while the process still
  expects to receive more data.

