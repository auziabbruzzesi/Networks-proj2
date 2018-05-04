CC = /usr/bin/gcc
all: git udpclient udpserver
git:
	git pull
udpclient: udpclient.c;
	${CC} udpclient.c -o udpclient
udpserver: udpserver.c;
	${CC} udpserver.c -o udpserver
exec_server: udpserver;
	./udpserver
exec_client: udpclient;
	./udpclient
clean:
	rm udpclient udpserver out.txt
