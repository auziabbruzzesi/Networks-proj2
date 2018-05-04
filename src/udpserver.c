/* udp_server.c */
/* Programmed by Adarsh Sethi */
/* February 21, 2018 */

#include <ctype.h>          /* for toupper */
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset */
#include <sys/socket.h>     /* for socket, sendto, and recvfrom */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define STRING_SIZE 1024

/* SERV_UDP_PORT is the port number on which the server listens for
   incoming messages from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

#define SERV_UDP_PORT 45678
//struct for segment
typedef struct{
  ////HEADER//////
  short seq_num;
  short count;
  ///DATA///////
  char data[STRING_SIZE];
}Segment;

//struct for ACK
typedef struct{
  ////ACK//////
  short ack_num;
}ACK;


int main(void) {


  int sock_server;  /* Socket on which server listens to clients */

  struct sockaddr_in server_addr;  /* Internet address structure that
                                      stores server address */
  unsigned short server_port;  /* Port number used by server (local port) */

  struct sockaddr_in client_addr;  /* Internet address structure that
                                      stores client address */
  unsigned int client_addr_len;  /* Length of client address structure */

  char filename[STRING_SIZE];  /* receive message */
  char modifiedfilename[STRING_SIZE]; /* send message */
  unsigned int msg_len;  /* length of message */
  int bytes_sent, bytes_recd; /* number of bytes sent or received */
  unsigned int i;  /* temporary loop variable */

  /* open a socket */

  if ((sock_server = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    perror("Server: can't open datagram socket\n");
    exit(1);
  }

  /* initialize server address information */

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                      any host interface, if more than one
                                      are present */
  server_port = SERV_UDP_PORT; /* Server will listen on this port */
  server_addr.sin_port = htons(server_port);

  /* bind the socket to the local server port */

  if (bind(sock_server, (struct sockaddr *) &server_addr,
                                  sizeof (server_addr)) < 0) {
    perror("Server: can't bind to local address\n");
    close(sock_server);
    exit(1);
  }

  /* wait for incoming messages in an indefinite loop */

  printf("Waiting for incoming messages on port %hu\n\n", 
                          server_port);

  client_addr_len = sizeof (client_addr);


/* open file */
  bytes_recd = recvfrom(sock_server, &filename, STRING_SIZE, 0,
                  (struct sockaddr *) &client_addr, &client_addr_len);
  printf("Received filename is: %s\n",filename);


  
  msg_len = bytes_recd;
  FILE* file;                //initialize the file
  file = fopen(filename,"r");
  char * line = (char*)malloc(80*sizeof(char));
  size_t buffer = STRING_SIZE;
  
  
  if(file){
  
  /*Enter the FSM*/
    short seq_num = 0;           
    Segment s;
    ACK a;

  /*Enter outer loop*/
    while(getline(&line,&buffer,file)>=0){
      s.count = htons((short)strlen(line));
      strcpy(s.data,line);
      s.seq_num = seq_num;
      printf("---------------------------\n");
      printf("string: %s\n",line);
      printf("sequence number of outgoing packet %d\n",s.seq_num);
      printf("count %d\n",s.count);
      
      /*SEND SEGMENT*/
      bytes_sent = sendto(sock_server,&s ,sizeof(s),0,
        (struct sockaddr*) &client_addr, client_addr_len);

      /*Enter inner loop: wait for Ack*/

      while(1){     
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        setsockopt(sock_server, SOL_SOCKET,SO_RCVTIMEO, (const void*)&timeout,sizeof(timeout));
        bytes_recd = recvfrom(sock_server,&a,sizeof(a),0,(struct sockaddr *)0,(int*)0);
        
        if(bytes_recd <= 0) {         //timeout
          printf("timeout.. retransmitting packet\n");
          bytes_sent = sendto(sock_server,&s,1024,0,(struct sockaddr*) &client_addr, client_addr_len);
          continue;
        }
        printf("ack recvd\n");
        printf("expected ack number %d\n",seq_num);
        printf("actual incoming ack number %d\n",ntohs(a.ack_num));
        if(ntohs(a.ack_num) != seq_num){
          continue;                     //null action
        }else{
          printf("changing seq_num\n");
          seq_num = 1 - seq_num;
          break;
        }
      }
    }
      Segment EOT;
      EOT.count = 0;
      EOT.seq_num = seq_num;
      bytes_sent = sendto(sock_server,&EOT,1024,0,(struct sockaddr*) &client_addr, client_addr_len);
  }   
	
}	
