#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sched.h>

#define BUFFER_SIZE (3 * 1024 * 1024)
#define SERVER_IP "192.168.16.70"
#define SERVER_PORT 12345

double elapsed_ms(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 +
           (end.tv_usec - start.tv_usec) / 1000.0;
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT),
        .sin_addr.s_addr = inet_addr(SERVER_IP)
    };

    connect(sock, (struct sockaddr *)&server, sizeof(server));

    char *buffer = malloc(BUFFER_SIZE);
    memset(buffer, 'A', BUFFER_SIZE);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    ssize_t n = send(sock, buffer, BUFFER_SIZE, 0);

    gettimeofday(&end, NULL);
    printf("Sent %zu bytes in %.3f ms\n", n, elapsed_ms(start, end));

    free(buffer);
    close(sock);
    return 0;
}
