#include <stdio.h>
#include <netpacket/packet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define ETHERTYPE_DEBUG 0x88B7

int main(int argc, char* argv[]) {

    // Check for the correct number of arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <interface>\n", argv[0]);
        return 1;
    }

    // Get the interface name from the command line argument
    const char* interface = argv[1];
    unsigned short ethertype = htons(ETHERTYPE_DEBUG);

    // Create a raw socket
    int sockfd = socket(AF_PACKET, SOCK_RAW, ethertype);
    if (sockfd < 0) {
        perror("Failed to create socket");
        return 1;
    }

    // Get the interface index
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
        perror("Failed to get interface index");
        close(sockfd);
        return 1;
    }

    // Set the interface index
    struct sockaddr_ll sa;
    memset(&sa, 0, sizeof(sa));
    sa.sll_family = AF_PACKET;
    sa.sll_protocol = ethertype;
    sa.sll_ifindex = ifr.ifr_ifindex;
    if (bind(sockfd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        perror("Failed to bind socket to interface");
        close(sockfd);
        return 1;
    }

    // Prepare the frame data
    char frame[ETH_FRAME_LEN];
    memset(frame, 0, ETH_FRAME_LEN);

    // Set the destination MAC address (broadcast)
    unsigned char dest_mac[ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    memcpy(frame, dest_mac, ETH_ALEN);
    // Set the source MAC address (use your own MAC address)
    unsigned char src_mac[ETH_ALEN] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}; // Replace with your MAC address
    memcpy(frame + ETH_ALEN, src_mac, ETH_ALEN);
    // Set the EtherType
    memcpy(frame + 12, &ethertype, sizeof(ethertype));
    // Set the payload (dummy data)
    const char* payload = "Hello, this is a dummy frame!";
    size_t payload_len = strlen(payload);
    if (payload_len > ETH_FRAME_LEN - 14) {
        fprintf(stderr, "Payload too large\n");
        close(sockfd);
        return 1;
    }
    memcpy(frame + 14, payload, payload_len);

    // Send the frame
    if (send(sockfd, frame, ETH_FRAME_LEN, 0) < 0) {
        perror("Failed to send frame");
        close(sockfd);
        return 1;
    }

    // Cleanup
    close(sockfd);

    return 0;
}
