#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUF_SIZE (3 * 1024 * 1024)

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUF_SIZE];
    ssize_t n;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    printf("Servidor esperando dados na porta %d...\n", PORT);
    while (1) {
        n = recvfrom(sockfd, (char *)buffer, BUF_SIZE, 0, (struct sockaddr *) &client_addr, &addr_len);
        if (n < 0) {
            perror("recvfrom");
            exit(1);
        }
        buffer[n] = '\0'; // adiciona o NULL terminator
        printf("Recebido: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
