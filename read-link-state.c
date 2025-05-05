#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define IFACE_COUNT 6
const char *ifaces[IFACE_COUNT] = {"lan22", "lan21", "lan25", "lan12", "lan13", "lan14"};

int read_operstate(const char *iface) {
    char path[64], state[16];
    snprintf(path, sizeof(path), "/sys/class/net/%s/operstate", iface);
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    fgets(state, sizeof(state), f);
    fclose(f);
    return (state[0] == 'u') ? 1 : 0; // 'up' -> 1, else 0
}

int main() {
    FILE *log = fopen("/codesys/NextoUser/Files/eth_status_log.txt", "w");
    if (!log) {
        perror("fopen");
        return 1;
    }

    while (1) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        fprintf(log, "%ld.%03ld,", ts.tv_sec, ts.tv_nsec / 1000000);

        for (int i = 0; i < IFACE_COUNT; ++i) {
            int state = read_operstate(ifaces[i]);
            fprintf(log, "%s=%d%s", ifaces[i], state, i < IFACE_COUNT - 1 ? "," : "\n");
        }

        fflush(log);
        usleep(100000);
    }

    fclose(log);
    return 0;
}
