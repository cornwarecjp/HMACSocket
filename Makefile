.PHONY: all clean

all: hmacsocketclient hmacsocketserver

clean:
	-rm hmacsocketclient hmacsocketserver
	-rm *.o

hmacsocketclient: client.o hmac.o network.o
	gcc -o hmacsocketclient client.o hmac.o network.o -lcrypto

hmacsocketserver: server.o hmac.o network.o
	gcc -o hmacsocketserver server.o hmac.o network.o -lcrypto

