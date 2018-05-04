CC = /usr/bin/gcc
all: git udpclient udpserver
test: udpclient udpserver
git:
	git pull
udpclient: udpclient.c;
	${CC} udpclient.c -o udpclient
udpserver: udpserver.c;
	${CC} udpserver.c -o udpserver

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
