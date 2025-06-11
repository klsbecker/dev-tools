#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFFER_SIZE (3 * 1024 * 1024)
#define SERVER_PORT 12345

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 1);

    char *buffer = malloc(BUFFER_SIZE);
    size_t received = 0;

    int n;
    while(1) {
        received = 0;
        int client_fd = accept(server_fd, NULL, NULL);
        while ((n = recv(client_fd, buffer, BUFSIZ, 0)) > 0)
            received += n;
        printf("Received %zu bytes\n", received);
        close(client_fd);
    }

    free(buffer);
    close(server_fd);
    return 0;
}
