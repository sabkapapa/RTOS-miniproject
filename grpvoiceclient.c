#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "integrate.h"

struct sockaddr_in serv;  // socket variable
int sd;                   // socket descripter
int conn;                 // connection descripter
struct VoiceMessage message;
pthread_t receiver_thread;
pthread_t send_thread;
// send function

// signal handler for ctrl + C
void signal_handler(int n) {
    char ans;
    exit(0);

    if (n == SIGINT) {
        printf("exit Y/N\n");
        scanf("%c", &ans);
        if (ans == 'y' || ans == 'Y') {
            printf("The client exited gracefully\n");

            exit(0);
        }
    }
}
// main function.
int main(int argc, char const *argv[]) {
    signal(SIGINT, signal_handler);  // to catch SIGINT and exit gracefully.

    // take from input argv
    const char *clientname = argv[3];  // name of the client
    printf("%s\n", clientname);
    const char *clientgroup = argv[4];  // group which the clien wants to add to.
    printf("%s\n", clientgroup);
    struct JoinRequest request;
    strcpy(request.name, clientname);
    strcpy(request.groupName, clientgroup);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    printf("socket created\n");
    // sd = socket(AF_INET, SOCK_STREAM, 0);
    serv.sin_family = AF_INET;
    int port_no = atoi(argv[2]);
    printf("%d\n", port_no);
    serv.sin_port = htons(port_no);
    printf("%s\n", argv[1]);
    if (inet_pton(AF_INET, argv[1], &serv.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    // inet_pton(AF_INET, argv[2], &serv.sin_addr); //binding client to local
    // host connect(sd, (struct sockaddr *)&serv, sizeof(serv));
    if (connect(sd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    send_data(sd, &request, sizeof(request));
    struct JoinResponse response;
    read_data(sd, &response, sizeof(response));

    int connectionId = response.id;
    printf("connectionId = %d\n", connectionId);

    int group_Id = response.groupId;
    printf("%d %d\n", connectionId, group_Id);
    struct Connection connection = {.sd = sd, .clientName = argv[3]};
    CALL(pthread_create(&receiver_thread, NULL, receive_voice_messages,
                        (void *)&connection),
         "Receiver thread");

    // Start receiver thread to receive call from others
    CALL(pthread_create(&send_thread, NULL, send_voice_messages,
                        (void *)&connection),
         "Send thread");
    pthread_join(send_thread, NULL);
    pthread_join(receiver_thread, NULL);
    message.id = connectionId;
    message.groupId = group_Id;



    return 0;
}