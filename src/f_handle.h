/**
 * ISA
 * @file f_handle.h
 * @authors Matus Vrablik (xvrab05)
 * @brief NetFlow flows handler
 */

// Header Guard
#ifndef F_HANDLE_H
#define F_HANDLE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include "f_time.h"

typedef struct flow_id {
    // char src_ip[256];
    uint32_t src_ip;
    uint16_t src_port;
    // char dst_ip[256];
    uint32_t dst_ip;
    uint16_t dst_port;
    int prot;
    // aditional variables for completness of flow
    struct timeval ts;
    uint8_t tcp_flags;
    int length;
} flow_id_t;

typedef struct flow {
    // 48B
    flow_id_t flow_id;
    int dPkts;
    struct timeval first;
    struct timeval last;
    int tcp_flags;
    int tos;
    int bytes;
} flow_t;

void initFlowArray();

void freeFlows();

void setVars(char *ip_addr, int active_timer, int inactive_timer, int flow_cache_size);

void exportFlow(flow_t *flow);

void exportFlowAll();

bool flowIDcmp(flow_id_t *f1, flow_id_t *f2);

void updateFlow(flow_id_t *flowID);
#endif
/*** Koniec suboru f_handle.h ***/
