.PHONY: all clean

all: hmacsocketclient hmacsocketserver

clean:
	-rm hmacsocketclient hmacsocketserver
	-rm *.o

hmacsocketclient: client.o hmac.o
	gcc -o hmacsocketclient client.o hmac.o -lcrypto

hmacsocketserver: server.o hmac.o
	gcc -o hmacsocketserver server.o hmac.o -lcrypto

