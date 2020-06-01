#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "integrate.h"

int sd;
pthread_t receiver_thread;
pthread_t send_thread;
void killClient();

// Interrupt handler
void handle_my(int sig);

// Server thread function
void *connection_handler(void *nsd);

//main function.
int main(int argc, char const *argv[]) {

  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}


  //fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server, client;
  int IP = inet_addr(argv[1]);
  server.sin_family = AF_INET;
  int port_no = atoi(argv[2]);
  printf("%d\n",port_no );
  server.sin_port = htons(port_no);
  printf("%s\n",argv[2] );
  if(inet_pton(AF_INET, argv[1], &server.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}
  //inet_pton(AF_INET, argv[2], &serv.sin_addr); //binding client to local host
  //connect(fd, (struct sockaddr *)&serv, sizeof(serv));
  CALL(connect(sd, (struct sockaddr *)&server, sizeof(server)),
       "Connecting to Server");

  struct Connection connection= {
      .sd = sd,
      .clientName = argv[3]
  };
  send(sd,connection.clientName, strlen(connection.clientName),0);
  // Start receiver thread to receive call from others
  CALL(pthread_create(&receiver_thread, NULL, receive_voice_messages,
                      (void *)&connection),
       "Receiver thread");

  // // Start receiver thread to receive call from others
  CALL(pthread_create(&send_thread, NULL, send_voice_messages, (void *)&connection),
       "Send thread");

  // pthread_join(receiver_thread, NULL);
  pthread_join(send_thread, NULL);

  return 0;
}

// Function to close resource handles and gracefully shutdown
void killClient() {
    char response[1];

    printf("Are you sure you want to close the client ? (Y/N) \n");
    scanf("%s", response);

    if (response[0] == 'Y' || response[0] == 'y') {
        pthread_kill(receiver_thread, SIGINT);
        pthread_kill(send_thread, SIGINT);
        close(sd);

        exit(EXIT_SUCCESS);
    }
}

// Interrupt handler
void handle_my(int sig) {
    switch (sig) {
        case SIGINT:
            killClient();
            break;
    }
}