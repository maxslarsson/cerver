#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>

// This function HAS TO exit. It CAN NOT simply return. If it returns, multiple 
// servers end up listening to the same parent socket for new connections.
void handle_client(int clientfd) {
    if (send(clientfd, "Hello, world!\n", 14, 0) == -1) {
        fprintf(stderr, "send error: %s\n", strerror(errno));
        exit(1);
    }

    if (close(clientfd) == -1) {
        fprintf(stderr, "close error: %s\n", strerror(errno));
        exit(1);
    }

    exit(0);
}
