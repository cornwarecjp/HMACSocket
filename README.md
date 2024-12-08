# HMACSocket

A TCP socket forwarding tool that adds HMAC-based authentication.

It is intended to minimize CPU overhead, compared to e.g. SSH port forwarding,
for use cases where authentication is needed, but encryption is not.


# Compiling
First install the dependencies:

* gcc
* make
* OpenSSL' libcrypto and its development files

For instance, on Debian, this should be sufficient (though this is untested):

```
apt install build-essential libssl-dev
```

Then, to compile, just run `make`.


# Usage

First, generate a file containing the shared secret, e.g. with:

```
dd if=/dev/urandom of=secret.dat bs=32 count=1
```

This file must be present on both sides of the authenticated connection, but it
must be kept secret from all others.
An attacker who gains access to this file, can authenticate himself, which
breaks the protection of the authentication mechanism.

Next, on the side where the to-be-forwarded port is reachable, run the
HMACSocket server program:

```
hmacsocketserver --keyfile=<keyfile> --listen=[<listenHost>:]<listenPort> --connect=<connectHost>:<connectPort>
```

Here
* `<keyfile>` is the key file, e.g. the file `secret.dat` generated earlier.
* `<listenHost>` is the hostname where the HMACSocket server listens for incoming
  connections.
  This can be used to only accept incoming connections from certain network
  interfaces.
  If omitted, the HMACSocket server listens on all interfaces.
* `<listenPort>` is the TCP port where the HMACSocket server listens for incoming
  connections.
* `<connectHost>` is the hostname where the to-be-forwarded port is located.
* `<connectPort>` is the TCP port number of the to-be-forwarded port.

Next, on the side where the to-be-forwarded port must be made reachable, run the
HMACSocket client program:

```
hmacsocketclient --keyfile=<keyfile> --listen=[<listenHost>:]<listenPort> --connect=<connectHost>:<connectPort>
```

Here
* `<keyfile>` is the key file, e.g. the file `secret.dat` generated earlier.
* `<listenHost>` is the hostname where the HMACSocket client listens for incoming
  connections.
  This can be used to only accept incoming connections from certain network
  interfaces.
  If omitted, the HMACSocket client listens on all interfaces.
* `<listenPort>` is the TCP port where the HMACSocket client listens for incoming
  connections.
* `<connectHost>` is the hostname where the HMACSocket server process is located.
* `<connectPort>` is the TCP port number of the HMACSocket server's listen port.

Now, processes can connect to the HMACSocket client's listen port, in the same
way as they would normally connect to the to-be-forwarded port.


# Usage example

In this example, a to-be-forwarded service is reachable only locally on host
`serverhost`, on TCP port 5000.
We want to make it available locally on host `clienthost`, but not to any other
participant in the network.

First, we copy `secret.dat` to both hosts, in a secure way that preserves its
secrecy.

On `serverhost`, we run the server:

```
hmacsocketserver --keyfile=secret.dat --listen=5001 --connect=localhost:5000
```

On `clienthost`, we run the client:

```
hmacsocketclient --keyfile=secret.dat --listen=localhost:5002 --connect=serverhost:5001
```

Now, on `clienthost`, processes can connect to TCP port 5002; this will be
forwarded to the service on TCP port 5000 on `serverhost`.


# Security considerations
For the authentication to be effective, there must be no way for an attacker to
work around it.
Therefore:

* The to-be-forwarded port must not be reachable from any untrusted host.
  In many cases, this means it should be bound to the localhost interface only.
* The port on which the HMACSocket client listens must not be reachable from any
  untrusted host.
  In many cases, this means it should be bound to the localhost interface only.
  You should therefore (nearly?) always specify `<listenHost>` on its
  commandline, even though the program allows you to omit this.

By design, HMACSocket does not provide encryption.
This limits its use to situations where
* An attacker cannot eavesdrop
* It is OK if an attacker gets to know the transfered data
* The transfered data is already encrypted through some other mechanism

Also read [doc/vulnerabilities.md](doc/vulnerabilities.md).


# Implementation details
The protocol between `hmacsocketclient` and `hmacsocketserver` is documented in
[doc/protocol.md](doc/protocol.md).

`hmacsocketclient` will only try to connect to `hmacsocketserver` when a process
connects to its listening port.
Similarly, `hmacsocketserver` will only try to connect to the to-be-forwarded
port when `hmacsocketclient` connects to its listening port.
Therefore, on start-up of these programs, errors in the `<connectHost>` and
`<connectPort>` values will not result in errors; these errors will only occur
once you try to establish connections.


# License

GPL v3. See the file `LICENSE`.

