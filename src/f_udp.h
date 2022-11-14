/**
 * ISA
 * @file f_udp.h
 * @authors Matus Vrablik (xvrab05)
 * @brief NetFlow udp client
 */

// Header Guard
#ifndef F_UDP_H
#define F_UDP_H

#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
// #include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
// #include <unistd.h>

void openSocket(char *ip, char *port);

void sendUdp(unsigned char *packet, int packet_size);

void closeSocket();

#endif
/*** Koniec suboru f_udp.h ***/
