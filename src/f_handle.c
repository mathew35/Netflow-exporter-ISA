/**
 * ISA
 * @file f_handle.c
 * @authors Matus Vrablik (xvrab05)
 * @brief NetFlow flows handler
 */

#include "f_handle.h"

char *ip = "127.0.0.1:2055";
int active = 60;
int export = 10;
int flow_size = 1024;

flow_t **flow_cache;
int flow_cnt = 0;
flow_t **expiredFlows;
int expired_cnt = 0;

void initFlowArray() {
    flow_cache = malloc(sizeof(flow_t *) * flow_size);
    for (int i = 0; i < flow_size; i++) {
        // flow_cache[i] = malloc(sizeof(flow_t));
    }
    expiredFlows = malloc(sizeof(flow_t *) * 30);
    for (int i = 0; i < 30; i++) {
        expiredFlows[i] = malloc(sizeof(flow_t));
    }
}

void freeFlows() {
    if (expiredFlows != NULL) {
        for (int i = 0; i < 30; i++) {
            free(expiredFlows[i]);
        }
    }
    free(expiredFlows);
    // free(flow_cache);
}

void setVars(char *ip_addr, int active_timer, int inactive_timer, int flow_cache_size) {
    ip = ip_addr;
    active = active_timer;
    export = inactive_timer;
    flow_size = flow_cache_size;
}

void exportFlow(flow_t *flow) {
    printf("\n");
    printf("number of packets:%d\n", flow->dPkts);
    printf("Bytes:%d\n", flow->bytes);
    printf("First time: %s", asctime(gmtime(&flow->first.tv_sec)));
    printf("Last time: %s", asctime(gmtime(&flow->last.tv_sec)));
    printf("src port: %u\n", ntohs(flow->flow_id.src_port));
    printf("dst port: %u\n", ntohs(flow->flow_id.dst_port));
    printf("protocol (6 = TCP, 2 = ICMP 17 = UDP): %d\n", flow->prot);
    // flow_t newFlow;
    // *flow = newFlow;
    // free(flow);
    // flow = NULL;
}

void exportFlowAll() {
    printf("Exporting all flows!\n");
    for (int i = 0; i < flow_cnt; i++) {
        printf("\nflow no.%d", i);
        exportFlow(flow_cache[i]);
        free(flow_cache[i]);
    }
    free(flow_cache);
}

bool flowIDcmp(flow_id_t *f1, flow_id_t *f2) {
    if (f1 != NULL && f2 != NULL)
        if (strcmp(f1->dst_ip, f2->dst_ip) == 0)
            if (strcmp(f1->src_ip, f2->src_ip) == 0)
                if (f1->src_port == f2->src_port)
                    if (f1->dst_port == f2->dst_port)
                        if (f1->prot == f2->prot) return true;
    return false;
}

void updateFlow(flow_id_t *flow_ID) {
    flow_t *flow = NULL;
    int fn = 0;
    for (int i = 0; i < flow_cnt; i++) {
        if (flowIDcmp(&flow_cache[i]->flow_id, flow_ID)) {
            flow = flow_cache[i];
            fn = i;
            break;
        }
    }
    if (flow == NULL) {
        if (flow_cnt == 1024) {
            // exportOldest();
            printf("\nflow_cnt=30!!!!");
            flow_cnt--;
            exportFlow(flow_cache[1023]);
            free(flow_cache[1023]);
            flow_cache[flow_cnt] = NULL; // free?
        }
        flow_t *newFlow = malloc(sizeof(flow_t));
        newFlow->flow_id = *flow_ID;
        newFlow->dPkts = 1;
        newFlow->first = flow_ID->ts;
        newFlow->last = newFlow->first;
        newFlow->prot = flow_ID->prot;
        newFlow->bytes = flow_ID->length;
        flow_cache[flow_cnt] = newFlow;
        flow_cnt++;
        return;
    }

    if (flow_ID->ts.tv_usec - flow->first.tv_usec > active * 1000000) {
        exportFlow(flow_cache[fn]);
        free(flow_cache[fn]);
        flow_cache[fn] = flow_cache[flow_cnt];
        flow_cache[flow_cnt] = NULL;
        flow_cnt--;
        updateFlow(flow_ID);
        return;
    }

    if (sysTime().tv_usec - flow->last.tv_usec > export * 1000000) {
        exportFlow(flow_cache[fn]);
        free(flow_cache[fn]);
        flow_cache[fn] = flow_cache[flow_cnt];
        flow_cache[flow_cnt] = NULL;
        flow_cnt--;
        updateFlow(flow_ID);
        return;
    }
    flow->bytes += flow_ID->length;
    flow->dPkts++;
    flow->last = flow_ID->ts;
    setLatest(flow_ID->ts);
}

/*** Koniec suboru f_handle.c ***/
