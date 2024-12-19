.PHONY: all clean test

all: hmacsocketclient hmacsocketserver

clean:
	-rm hmacsocketclient hmacsocketserver
	-rm *.o

hmacsocketclient: client.o hmac.o network.o protocol.o args.o
	gcc -o hmacsocketclient client.o hmac.o network.o protocol.o args.o -lcrypto

hmacsocketserver: server.o hmac.o network.o protocol.o args.o
	gcc -o hmacsocketserver server.o hmac.o network.o protocol.o args.o -lcrypto

test: hmacsocketclient hmacsocketserver
	./test.py

