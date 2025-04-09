#include <stdlib.h>
#include <unistd.h>

// This function HAS TO exit. It cannot simply return. If it returns, multiple 
// servers end up listening to the same parent socket for new connections.
void handle_client(int clientfd) {
    // printf("Client");
    close(clientfd);
    exit(0);
}
