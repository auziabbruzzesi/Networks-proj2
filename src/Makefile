CC = /usr/bin/gcc
CFLAGS = -lm
all: udpclient udpserver
test: testclient testserver
git:
	git pull
udpclient: udpclient.c;
	${CC} udpclient.c -o udpclient
udpserver: udpserver.c;
	${CC}  udpserver.c -o udpserver $(CFLAGS)
clean:
	rm udpclient udpserver out.txt


