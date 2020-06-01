#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<errno.h>
#include<sys/wait.h>
#include<pthread.h>
#include<signal.h>
#include "integrate.h"

void signal_handler(int n){
  char ans;

  if (n == SIGINT) {
        printf("exit Y/N\n");
        scanf("%c", &ans);
        if (ans == 'y' || ans == 'Y') {
            printf("The client exited gracefully\n");

            exit(0);
        }
    }
}
int sd;
pthread_t receiver_thread;
pthread_t send_thread;
//main function:
int main(int argc, char *argv[]){
  if (argc < 2) {
        printf("Usage: %s <server port>", argv[0]);
        exit(EXIT_FAILURE);
    }
	struct sockaddr_in server, client;
	int addrlen = sizeof(server);
	char buffer[1024] = {0};
  pthread_t thread;
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

  server.sin_family = AF_INET;
  int port_no = atoi(argv[1]);
  printf("%d\n",port_no);
  server.sin_port = htons(port_no); // port at which server will listen for connections
  server.sin_addr.s_addr = INADDR_ANY;
  //fd = socket(AF_INET, SOCK_STREAM, 0);
  int trueV = 1;
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &trueV, sizeof(int));
  if (bind(sd, (struct sockaddr *)&server,
								sizeof(server))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
  //bind(fd, (struct sockaddr *) &serv, sizeof(serv));
  if (listen(sd, 1) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}


    signal(SIGINT,signal_handler);
    while((conn =accept(sd, (struct sockaddr *)&server,
					(socklen_t*)&addrlen))>-1){

            printf("entered loop\n");
    // int pid=fork();
    char rply[1024];
    read(sd,rply,sizeof(rply));
    struct Connection connection = {
        .sd = conn,
        .clientName = rply
    };
    printf("Connection Established\n");
    // Start receiver thread to receive call from others
    CALL(pthread_create(&receiver_thread, NULL, receive_voice_messages,
                        (void *)&connection),
         "Receiver thread");

    // Start receiver thread to receive call from others
    CALL(pthread_create(&send_thread, NULL, send_voice_messages,
                        (void *)&connection),
         "Send thread");

    pthread_join(receiver_thread, NULL);
    pthread_join(send_thread, NULL);
      //exit(0);

    }

  return 0;
}