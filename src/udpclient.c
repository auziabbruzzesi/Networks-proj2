/* udp_client.c */
/* Programmed by Adarsh Sethi */
/* February 21, 2018 */

#include <stdio.h>      /* for standard I/O functions */
#include <stdlib.h>     /* for exit */
#include <string.h>     /* for memset, memcpy, and strlen */
#include <netdb.h>      /* for struct hostent and gethostbyname */
#include <sys/socket.h> /* for socket, sendto, and recvfrom */
#include <netinet/in.h> /* for sockaddr_in */
#include <unistd.h>     /* for close */
#include<time.h>        /* for time */
#include<math.h>
#define STRING_SIZE 1024

#define SERV_UDP_PORT 45678

float PacketLossRate = 0;
float ACKLossRate = 0;
//struct for ACK
typedef struct{
  ////ACK//////
  short seq_num;
} ACK;

// Struct for segment
typedef struct{
  ////HEADER//////
  short seq_num;
  short count;
  ///DATA///////
  char data[STRING_SIZE];
} Segment;


int SimulateLoss(){
  srand((unsigned)time(NULL));
  float x = (((long double)rand()+1)/((long double)RAND_MAX+1));
  if(x < PacketLossRate){
    return 1;
  }else{
    return 0;
  }
}

int SimulateACKLoss(){
  srand((unsigned)time(NULL));
  float x = (((long double)rand()+1)/((long double)RAND_MAX+1));
  if(x < ACKLossRate){
    return 1;
  }else{
    return 0;
  }

}
int main(int argc, char** argv){

  if(argc != 3){
    printf("Usage: udpclient <Packet Loss Rate (0<=n<=1)> <ACK Loss Rate (0<=n<=1)>\n");
    printf("defaulting to parameters 0 0\n");
  }else{

  PacketLossRate = atof(argv[1]);
  ACKLossRate = atof(argv[2]);
  }



  int sock_client; /* Socket used by client */

  struct sockaddr_in client_addr; /* Internet address structure that
                                        stores client address */
  unsigned short client_port;     /* Port number used by client (local port) */

  struct sockaddr_in server_addr;    /* Internet address structure that
                                        stores server address */
  struct hostent *server_hp;         /* Structure to store server's IP
                                        address */
  char server_hostname[STRING_SIZE]; /* Server's hostname */
  unsigned short server_port;        /* Port number used by server (remote port) */

  char filename[STRING_SIZE];         /* send message */
  char modifiedfilename[STRING_SIZE]; /* receive message */
  unsigned int msg_len;               /* length of message */
  int bytes_sent, bytes_recd;         /* number of bytes sent or received */
  int ACKLossRate = 0;
  int PacketLossRate = 0;

  /* open a socket */

  if ((sock_client = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
    perror("Client: can't open datagram socket\n");
    exit(1);
  }

  /* Note: there is no need to initialize local client address information
            unless you want to specify a specific local port.
            The local address initialization and binding is done automatically
            when the sendto function is called later, if the socket has not
            already been bound. 
            The code below illustrates how to initialize and bind to a
            specific local port, if that is desired. */

  /* initialize client address information */

  client_port = 0; /* This allows choice of any available local port */

  
  /* clear client address structure and initialize with client address */
  memset(&client_addr, 0, sizeof(client_addr));
  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* This allows choice of
                                        any host interface, if more than one 
                                        are present */
  client_addr.sin_port = htons(client_port);

  /* bind the socket to the local client port */

  if (bind(sock_client, (struct sockaddr *)&client_addr,sizeof(client_addr)) < 0){
    perror("Client: can't bind to local address\n");
    close(sock_client);
    exit(1);
  }

  /* end of local address initialization and binding */

  /* initialize server address information */

  strcpy(server_hostname, "localhost");
  if ((server_hp = gethostbyname(server_hostname)) == NULL){
    perror("Client: invalid server hostname\n");
    close(sock_client);
    exit(1);
  }
  server_port = 45678;

  /* Clear server address structure and initialize with server address */
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
         server_hp->h_length);
  server_addr.sin_port = htons(server_port);

  /* user interface */

  // printf("Please input a filename:\n");
  scanf("%s", filename);
  msg_len = strlen(filename) + 1;

  /* SEND FILE NAME */
  bytes_sent = sendto(sock_client, filename, msg_len, 0,
    (struct sockaddr *)&server_addr, sizeof(server_addr));



  Segment s; 
  ACK a;     
  int data_bytes = 0; //initialize data_bytes
  short expectedSeqNum = 0;
  short seq;
  int packetloss;
  int ackloss;

  
  FILE *file;
  file = fopen("out.txt", "wb");
  
  while (1){
    // printf("before rcv\n");
    bytes_recd = recvfrom(sock_client, &s, sizeof(s), 0, (struct sockaddr *)0, (int *)0);
    //printf("after rcv\n");
    // printf("line: %s\n",s.data);
    //printf("line: %s",line);

    //char * message = (char*)malloc((80)*sizeof(char)); //initialize the message... we know this will be at most 80 chars

    seq = ntohs(s.seq_num); // pull the sequence number out of the header
    if (ntohs(s.count) == 0){
       printf("received EOT packet\n");
      break;
    }
    char *message = (s.data);
    printf("--------------------------------------\n");
    printf("string: %s\n", message);
    printf("sequence number of incoming packet %d\n", ntohs(s.seq_num));
    printf("expected seqnum %d \n", expectedSeqNum);
    printf("count: %d \n", ntohs(s.count));

    
    packetloss = SimulateLoss();

    if(packetloss){
      continue;
    }else{
      if(ntohs(s.seq_num) != expectedSeqNum){
        a.seq_num = htons(1 - expectedSeqNum);
        
      }else{
        a.seq_num = htons(expectedSeqNum);
      }

      ackloss = SimulateACKLoss();

      if(!ackloss){
        bytes_sent = sendto(sock_client, &a, sizeof(a), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
      }else{
        continue;
      }
    }
    expectedSeqNum = 1 - expectedSeqNum;


   
  }
  close(sock_client);
}
