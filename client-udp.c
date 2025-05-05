#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>


#define PORT 12345
#define BUF_SIZE (3 * 1024 * 1024)

double elapsed_ms(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 +
           (end.tv_usec - start.tv_usec) / 1000.0;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];
    ssize_t n;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("10.19.82.1");  // Endereço IP do servidor

    // Preenche os dados que você quer enviar
    memset(buffer, 'A', BUF_SIZE);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    n = sendto(sockfd, (const char *)buffer, BUF_SIZE, 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
    if (n < 0) {
        perror("sendto");
        exit(1);
    }

    gettimeofday(&end, NULL);
    printf("Sent %zu bytes in %.3f ms\n", send, elapsed_ms(start, end));

    close(sockfd);
    return 0;
}
