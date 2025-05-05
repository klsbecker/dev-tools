#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sched.h>

struct sched_param param = { .sched_priority = 80 }; // 1–99 (maior = mais prioridade)

#define PORT 12345
#define SIZE (3 * 1024 * 1024)

double elapsed_ms(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 +
           (end.tv_usec - start.tv_usec) / 1000.0;
}

int main() {
    sched_setscheduler(0, SCHED_FIFO, &param);           // ou SCHED_RR
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = inet_addr("10.19.82.1")  // IP do servidor
    };

    connect(sock, (struct sockaddr *)&server, sizeof(server));

    char *buffer = malloc(SIZE);
    memset(buffer, 'A', SIZE);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    size_t sent = 0;
    ssize_t n = send(sock, buffer + sent, SIZE - sent, 0);


    gettimeofday(&end, NULL);
    printf("Sent %zu bytes in %.3f ms\n", sent, elapsed_ms(start, end));

    free(buffer);
    close(sock);
    return 0;
}
