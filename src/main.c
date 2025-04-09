#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include "client_handler.h"

#define LISTEN_BACKLOG 12

void sigchld_handler(int s)
{
    (void)s;
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

// TODO: Close child processes when parent exits (e.g. gets Control-C)
int main(void) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *res;
    int status = getaddrinfo(NULL, "3000", &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    int sockfd;
    struct addrinfo *p;
    for(p = res; p != NULL; p = p->ai_next) {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd == -1) {
            fprintf(stderr, "socket error: %s\n", strerror(errno));
            continue;
        }

        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
            fprintf(stderr, "setsockopt error: %s\n", strerror(errno));
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            fprintf(stderr, "bind error: %s\n", strerror(errno));
            continue;
        }

        break;
    }
    
    freeaddrinfo(res);

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, LISTEN_BACKLOG) == -1) {
        fprintf(stderr, "listen error: %s\n", strerror(errno));
        exit(1);
    }

    struct sigaction sa;
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction error: %s\n", strerror(errno));
        exit(1);
    }

    printf("Listening on port 3000...\n");

    while (1) {
        struct sockaddr_storage clientaddr;
        socklen_t clientaddr_len = sizeof(clientaddr);
        int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &clientaddr_len);
        if (clientfd == -1) {
            fprintf(stderr, "accept error: %s\n", strerror(errno));
            exit(1);
        }

        int pid = fork();
        if (pid == 0) {
            // Child
            close(sockfd);
            handle_client(clientfd);
            close(clientfd);
            exit(0);
        } else if (pid > 0) {
            // Parent
            close(clientfd);
            printf("New client made with fd: %d\n", clientfd);
        } else {
            // Error
            fprintf(stderr, "fork error: %s\n", strerror(errno));
            exit(1);
        }
    }

    return 0;
}
