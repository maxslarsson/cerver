#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>

#include "http_responses.h"

// This function can not exit. It has to return. When this function returns,
// the clientfd is closed and the child process is exited.
void handle_client(int clientfd) {
    char contents[8192];
    int bytes_recv = recv(clientfd, &contents, sizeof(contents)-1, 0);
    if (bytes_recv == -1) {
        fprintf(stderr, "recv error: %s\n", strerror(errno));
    }
    contents[bytes_recv] = '\0';

    char *content_head = &contents[0];
    char *http_method = strsep(&content_head, " ");
    char *path = strsep(&content_head, " ");
    char *http_protocol = strsep(&content_head, "\r\n");

    if (strncmp(http_method, "GET", 4) != 0) { 
        send(clientfd, HTML_INVALID_METHOD, strlen(HTML_INVALID_METHOD), 0);
        return;
    }

    if (strncmp(http_protocol, "HTTP/1.1", 9) != 0) {
        send(clientfd, HTML_INVALID_PROTOCOL, strlen(HTML_INVALID_PROTOCOL), 0);
        return;
    }

    if (strncmp(path, "/", 2) != 0) {
        send(clientfd, HTML_NOT_FOUND, strlen(HTML_NOT_FOUND), 0);
        return;
    }

    send(clientfd, HTML_SUCCESS, strlen(HTML_SUCCESS), 0);
}
