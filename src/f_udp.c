/**
 * ISA
 * @file f_udp.c
 * @authors Matus Vrablik (xvrab05)
 * @brief NetFlow udp client
 */
/*
 */
#include "f_udp.h"
/*
    Following code is taken from https://moodle.vut.cz/pluginfile.php/502893/mod_folder/content/0/udp/echo-udp-client2.c?forcedownload=1 and adjusted.
    Author: Petr Matousek
*/
int sock;                        // socket descriptor
struct sockaddr_in server, from; // address structures of the server and the client
socklen_t len;

void openSocket(char *ip, char *port) {
    struct hostent *servent;            // network host entry required by gethostbyname()
    memset(&server, 0, sizeof(server)); // erase the server structure
    server.sin_family = AF_INET;

    // make DNS resolution of the first parameter using gethostbyname()
    if ((servent = gethostbyname(ip)) == NULL) // check the first parameter
        errx(1, "gethostbyname() failed\n");

    // copy the first parameter to the server.sin_addr structure
    memcpy(&server.sin_addr, servent->h_addr, servent->h_length);

    server.sin_port = htons(atoi(port)); // server port (network byte order)

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) // create a client socket
        err(1, "socket() failed\n");

    printf("* Server socket created\n");

    len = sizeof(server);
    // fromlen = sizeof(from);

    printf("* Creating a connected UDP socket using connect()\n");
    // create a connected UDP socket
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
        err(1, "connect() failed");
}

void sendUdp(unsigned char *packet, int packet_size) {
    // send data to the server
    int i = send(sock, packet, packet_size, 0); // send data to the server
    if (i == -1)                                // check if data was sent correctly
        err(1, "send() failed");
    else if (i != packet_size)
        err(1, "send(): buffer written partially");

    // obtain the local IP address and port using getsockname()
    if (getsockname(sock, (struct sockaddr *)&from, &len) == -1)
        err(1, "getsockname() failed");

    printf("* Data sent from %s, port %d (%d) to %s, port %d (%d)\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port), from.sin_port, inet_ntoa(server.sin_addr), ntohs(server.sin_port), server.sin_port);
}

void closeSocket() {
    close(sock);
    printf("* Closing the client socket ...\n");
}

/*** Koniec suboru f_udp.c ***/
