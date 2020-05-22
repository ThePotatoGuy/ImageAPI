#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "server.h"
#include "logging.h"
#include "http_parser.h"

static http_parser_settings httpParserSettings;

int urlCallback(http_parser* parser, const char *at, size_t length) {

    // Read length of url and copy it to data ptr on parser
    parser->data = malloc(length);
    memcpy(parser->data, at, length);
    return 0;

}

void *connection(void *p) {

    int *connfd_thread = (int *)p;
    int nrecv = 0;
    int parsed = 0;
    char recv_buf[1024];
    char *send_buf = NULL;

    log_Debug("Incoming Request");

    // Create parser
    http_parser *httpParser = malloc(sizeof(http_parser));
    http_parser_init(httpParser, HTTP_REQUEST);

    // Accept only 1024 bytes
    nrecv = recv(*connfd_thread, recv_buf, sizeof(recv_buf), 0);
    if (nrecv < 5) {  // -1 = recv err, 1-4 should be "GET "
        log_Warning("Recv err");
        goto exit;
    }

    // Only accept GET requests
    if (strncmp(recv_buf, "GET ", 4)) {
        log_Debug("Not a GET request");
        send(*connfd_thread, "HTTP/1.1 400 Bad Request\n\n", 26, 0);
        goto exit;
    }

    // Parse request
    parsed = http_parser_execute(httpParser, &httpParserSettings, recv_buf, nrecv);
    if (parsed != nrecv) {
        log_Warning("Parser failure, recieved bytes not same as parsed bytes.\n");
        send(*connfd_thread, "HTTP/1.1 500 Internal Server Error\n\n", 36, 0);
        goto exit;
    }

    send(*connfd_thread, "HTTP/1.1 200 OK\n\n", 17, 0);

exit:
    free(send_buf);
    free(httpParser->data);
    free(httpParser);
    close(*connfd_thread);
    pthread_exit(NULL);

}

int server_Start(char *sockPath) {

    int sfd;
    struct sockaddr_un addr;
    int running = 0;
    int connfd;
    pthread_t thread;

    // Setup http parser settings
    httpParserSettings.on_url = urlCallback;

    // Create socket file descriptor
    if((sfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
        log_Warning("Socket error.");
        return -1;
    }

    // Create addr struct
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, sockPath);
    unlink(sockPath);

    // Bind socket to addr
    if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        log_Warning("Failed to bind socket.");
        return -1;
    }

    log_Info("Server running.");

    // Loop over threads
    running = 1;
    while (running) {

        if (listen(sfd, 10)) {
            log_Warning("Listen err");
            running = 0;
            goto exit;
        }

        connfd = accept(sfd, NULL, NULL);
        if (connfd < 0) {
            log_Warning("Accept Error");
            running = 0;
            goto exit;
        }
        
        // Create threads
        pthread_create(&thread, NULL, connection, &connfd);
        pthread_join(thread, NULL);

    }

exit:
    log_Info("Closing...");
    close(sfd);
    return 0;

}
