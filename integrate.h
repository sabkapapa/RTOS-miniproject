#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/gccmacro.h>
extern int DEBUG;
#define BUFSIZE 1024


//for text chat
struct sockaddr_in serv; // socket variable
int fd; // file descripter
int conn; // connection descripter
char message[100] = "";
void* send_func(void* n){
  int sd = *(int*)n;
  char msg[1024] = {0};
  fgets(msg,1024,stdin);
  send(sd,msg, strlen(msg),0);
  return 0;
}

void* recv_func(void* n){
  int rsd = *(int*)n;
  char rply[1024];
  read(rsd,rply,sizeof(rply));
  printf("%s\n",rply );
  return 0;
}
struct Connection{
  int sd;
  char* clientName;
};
#define CALL(n, msg)                                              \
    if ((n) < 0) {                                                \
        fprintf(stderr, "%s (%s:%d)\n", msg, __FILE__, __LINE__); \
        exit(EXIT_FAILURE);                                       \
    }
    static ssize_t loop_write(int fd, const void*data, size_t size) {
        ssize_t ret = 0;

        while (size > 0) {
            ssize_t r;

            if ((r = write(fd, data, size)) < 0)
                return r;

            if (r == 0)
                break;

            ret += r;
            data = (const uint8_t*) data + r;
            size -= (size_t) r;
        }

        return ret;
    }

/* The Sample format to use */
static const pa_sample_spec ss = {
    .format = PA_SAMPLE_S16LE, .rate = 40100, .channels = 2};

//for recieving voice message; passing a pointer through the function.
//
void* receive_voice_messages(void* ptr) {
    struct Connection connection = *(struct Connection*)ptr;

    int sd = connection.sd;
    char* clientName = connection.clientName;

    pa_simple* s = NULL;
    int ret = 1;
    int error;

    /* Create a new playback stream */
    if (!(s = pa_simple_new(NULL, clientName, PA_STREAM_PLAYBACK, NULL,
                            "playback", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__ ": pa_simple_new() failed: %s\n",
                pa_strerror(error));
        goto finish;
    }
    for (;;) {
        uint8_t buf[BUFSIZE];
        ssize_t r;
        // #if 1
        //         pa_usec_t latency;
        //         if ((latency = pa_simple_get_latency(s, &error)) ==
        //         (pa_usec_t) -1) {
        //             fprintf(stderr, __FILE__":
        //             pa_simple_get_playback_latency() failed: %s\n",
        //             pa_strerror(error)); goto finish;
        //         }
        //         fprintf(stderr, "%0.0f usec    \r", (float)latency);
        // #endif
        /* Read some data ... */
        if ((r = read(sd, buf, sizeof(buf))) <= 0) {
            if (r == 0) /* EOF */
                break;
            fprintf(stderr, __FILE__ ": read() failed: %s\n", strerror(error));
            goto finish;
        }
        /* ... and play it */
        if (pa_simple_write(s, buf, (size_t)r, &error) < 0) {
            fprintf(stderr, __FILE__ ": pa_simple_write() failed: %s\n",
                    pa_strerror(error));
            goto finish;
        }
    }
    /* Make sure that every single sample was played */
    if (pa_simple_drain(s, &error) < 0) {
        fprintf(stderr, __FILE__ ": pa_simple_drain() failed: %s\n",
                pa_strerror(error));
        goto finish;
    }
    ret = 0;
finish:
    if (s) pa_simple_free(s);

    return NULL;
}

void* send_voice_messages(void* ptr) {
    struct Connection connection = *(struct Connection*)ptr;

    int sd = connection.sd;
    char* clientName = connection.clientName;

    printf("sd = %d\n", sd);
    printf("clientName = %s\n", clientName);

    pa_simple* s = NULL;
    int ret = 1;
    int error;
    /* Create the recording stream */

    if (!(s = pa_simple_new(NULL, clientName, PA_STREAM_RECORD, NULL, "record",
                            &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__ ": pa_simple_new() failed: %s\n",
                pa_strerror(error));
        goto finish;
    }
    for (;;) {
        uint8_t buf[BUFSIZE];
        /* Record some data ... */
        if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
            fprintf(stderr, __FILE__ ": pa_simple_read() failed: %s\n",
                    pa_strerror(error));
            goto finish;
        }
        /* And write it to STDOUT */
        if (loop_write(sd, buf, sizeof(buf)) != sizeof(buf)) {
            fprintf(stderr, __FILE__ ": write() failed: %s\n", strerror(error));
            goto finish;
        }
    }
    ret = 0;
finish:
    if (s) pa_simple_free(s);
    return NULL;
}
