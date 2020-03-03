#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#define BUFSIZE 10000


void main(int argc, char const *argv[]) 
{ 
	// PulseAudio details
	static const pa_sample_spec ss = {
		.format = PA_SAMPLE_S16LE,
		.rate = 20000,
		.channels = 1
	};
	pa_simple *s = NULL;
	int ret = 1;
	int error;

	/* Create the recording stream */
	if (!(s = pa_simple_new(NULL, NULL, PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
		fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
	}
		
	
	// socket create
	struct sockaddr_in address; 
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1; // create socket
	memset(&serv_addr, '0', sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(6000); //  port
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) return -1; // connecting
	
	// Last read time
	int last_read = 10000;

	// Run continuously
	int ctr = 0;
	while(1){
		//printf("hi");
		uint8_t buf[BUFSIZE];
		
		
		send(sock , buf, sizeof(buf) , 0 ); 
		printf("Sent %d\n",ctr++); 
		
	}
} 

