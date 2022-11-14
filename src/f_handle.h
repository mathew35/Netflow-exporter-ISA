/**
 * ISA
 * @file f_handle.h
 * @authors Matus Vrablik (xvrab05)
 * @brief NetFlow flows handler
 */

// Header Guard
#ifndef F_HANDLE_H
#define F_HANDLE_H

#define _GNU_SOURCE // for asprintf()
#ifndef TH_FIN
#define TH_FIN 0x01
#endif
#ifndef TH_RST
#define TH_RST 0x04
#endif

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
#include "f_udp.h"

typedef struct flow_id {
    uint32_t src_ip;
    uint16_t src_port;
    uint32_t dst_ip;
    uint16_t dst_port;
    int prot;
    uint8_t tos;
    // aditional variables for completness of flow
    struct timeval ts;
    uint8_t tcp_flags;
    int length;
} flow_id_t;

typedef struct flow {
    // 48B
    flow_id_t flow_id;
    int dPkts;
    time_t first;
    time_t last;
    int bytes;
    int exportUptime;
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
