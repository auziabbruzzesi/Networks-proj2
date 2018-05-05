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

testclient: testudpclient.c;
	${CC} testudpclient.c -o testclient
testserver: testudpserver.c;
	${CC} testudpserver.c -o testserver
exec_server: udpserver;
	./udpserver
exec_client: udpclient;
	./udpclient
clean:
	rm udpclient udpserver out.txt
