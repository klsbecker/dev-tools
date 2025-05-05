#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "ptp_clock.h"
#include <string.h>
#include <errno.h>

#define PTP_DEV_PATH "/dev/ptp0" // Caminho do dispositivo PTP



int main() {
    int fd;
    struct ptp_sys_offset offset;
    int err;

    // Abrir o dispositivo PTP
    fd = open(PTP_DEV_PATH, O_RDWR);
    if (fd < 0) {
        perror("Falha ao abrir dispositivo PTP");
        return -1;
    }

    // Inicializar a estrutura de offsets
    offset.n_samples = 3; // Número de amostras a serem capturadas

    // Chamar o ioctl PTP_SYS_OFFSET para obter os offsets
    err = ioctl(fd, PTP_SYS_OFFSET, &offset);
    if (err < 0) {
        perror("Falha ao chamar ioctl PTP_SYS_OFFSET");
        close(fd);
        return -1;
    }

    // Exibir os resultados das amostras
    printf("Resultados das amostras:\n");
    for (int i = 0; i < offset.n_samples; i++) {
        printf("Amostra %d: %ld segundos, %ld nanosegundos\n",
               i, offset.ts[i].sec, offset.ts[i].nsec);
    }

    // Fechar o dispositivo PTP
    close(fd);
    return 0;
}
