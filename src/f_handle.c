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
bool ipOK = false;

flow_t **flow_cache;
int flow_cnt = 0;
flow_t **expiredFlows;
int expired_cnt = 0;
uint32_t all_flows_cnt = 0;

void initFlowArray() {
    flow_cache = malloc(sizeof(flow_t *) * flow_size);
    expiredFlows = malloc(sizeof(flow_t *) * 30);
    // for (int i = 0; i < 30; i++) {
    //     expiredFlows[i] = malloc(sizeof(flow_t));
    // }
}

void freeFlows() {
    // for (int i = 0; i < 30; i++) {
    //     free(expiredFlows[i]);
    // }
    free(expiredFlows);
    // free(flow_cache);
}

void setVars(char *ip_addr, int active_timer, int inactive_timer, int flow_cache_size, bool gotIP) {
    ip = ip_addr;
    active = active_timer;
    export = inactive_timer;
    flow_size = flow_cache_size;
    ipOK = gotIP;
}

void exportExpired() {
    unsigned char header[24];
    unsigned char data[24 + 48 * expired_cnt];
    // uint32_t sysT = sysTime().tv_sec + sysTime().tv_usec/1000000;
    // version
    header[0] = 0x00;
    header[1] = 0x05;
    // flow count
    header[2] = expired_cnt >> 8;
    header[3] = expired_cnt;
    // SysUptime
    header[4] = sysUptime() / 1000 >> 24;
    header[5] = sysUptime() / 1000 >> 16;
    header[6] = sysUptime() / 1000 >> 8;
    header[7] = sysUptime() / 1000;
    // unix_secs
    header[8] = sysTime().tv_sec >> 24;
    header[9] = sysTime().tv_sec >> 16;
    header[10] = sysTime().tv_sec >> 8;
    header[11] = sysTime().tv_sec;
    // unix_nsecs
    header[12] = (sysTime().tv_usec * 1000) >> 24;
    header[13] = (sysTime().tv_usec * 1000) >> 16;
    header[14] = (sysTime().tv_usec * 1000) >> 8;
    header[15] = (sysTime().tv_usec * 1000);
    // flow_sequence
    header[16] = all_flows_cnt >> 24;
    header[17] = all_flows_cnt >> 16;
    header[18] = all_flows_cnt >> 8;
    header[19] = all_flows_cnt;
    // fill in rest with 0
    header[20] = 0;
    header[21] = 0;
    header[22] = 0;
    header[23] = 0;
    for (int i = 0; i < 24; i++) {
        data[i] = header[i];
    }

    unsigned char record[48];
    for (int i = 0; i < expired_cnt; i++) {
        // 0-3 srcip
        record[0] = expiredFlows[i]->flow_id.src_ip >> 24;
        record[1] = expiredFlows[i]->flow_id.src_ip >> 16;
        record[2] = expiredFlows[i]->flow_id.src_ip >> 8;
        record[3] = expiredFlows[i]->flow_id.src_ip;
        // 4-7 dstip
        record[4] = expiredFlows[i]->flow_id.dst_ip >> 24;
        record[5] = expiredFlows[i]->flow_id.dst_ip >> 16;
        record[6] = expiredFlows[i]->flow_id.dst_ip >> 8;
        record[7] = expiredFlows[i]->flow_id.dst_ip;
        // 8-15 000000
        record[8] = 0;
        record[9] = 0;
        record[10] = 0;
        record[11] = 0;
        record[12] = 0;
        record[13] = 0;
        record[14] = 0;
        record[15] = 0;
        // 16-19 packets in flow
        record[16] = expiredFlows[i]->dPkts >> 24;
        record[17] = expiredFlows[i]->dPkts >> 16;
        record[18] = expiredFlows[i]->dPkts >> 8;
        record[19] = expiredFlows[i]->dPkts;
        // 20-23 byte size
        record[20] = expiredFlows[i]->bytes >> 24;
        record[21] = expiredFlows[i]->bytes >> 16;
        record[22] = expiredFlows[i]->bytes >> 8;
        record[23] = expiredFlows[i]->bytes;
        // 24-27 first time
        record[24] = expiredFlows[i]->first / 1000 >> 24;
        record[25] = expiredFlows[i]->first / 1000 >> 16;
        record[26] = expiredFlows[i]->first / 1000 >> 8;
        record[27] = expiredFlows[i]->first / 1000;
        // 28-31 last time
        record[28] = expiredFlows[i]->last / 1000 >> 24;
        record[29] = expiredFlows[i]->last / 1000 >> 16;
        record[30] = expiredFlows[i]->last / 1000 >> 8;
        record[31] = expiredFlows[i]->last / 1000;
        // 32-33 srcport
        record[32] = expiredFlows[i]->flow_id.src_port;
        record[33] = expiredFlows[i]->flow_id.src_port >> 8;
        // 34-35 dstport
        record[34] = expiredFlows[i]->flow_id.dst_port;
        record[35] = expiredFlows[i]->flow_id.dst_port >> 8;
        // 36 000000
        record[36] = 0;
        // 37 tcp_flags
        record[37] = expiredFlows[i]->flow_id.tcp_flags;
        // 38 prot
        record[38] = expiredFlows[i]->flow_id.prot;
        // 39 tos
        record[39] = expiredFlows[i]->tos;
        // 40-47 000000
        record[40] = 0;
        record[41] = 0;
        record[42] = 0;
        record[43] = 0;
        record[44] = 0;
        record[45] = 0;
        record[46] = 0;
        record[47] = 0;
        int k = 0;
        for (int j = 24 + 48 * i; j < 24 + 48 * (i + 1); j++, k++) {
            data[j] = record[k];
        }
        // fwrite(record, sizeof(record), 1, f);
        free(expiredFlows[i]);
        expiredFlows[i] = NULL;
    }
    FILE *f = fopen("datafile", "wb");
    fwrite(data, sizeof(data), 1, f);
    fclose(f);
    char *rest = "127.0.0.1";
    if (ipOK) {
        rest = strsep(&ip, ":");
        // printf("delmited %s and rest %s\n", ip, rest);
        if (ip == NULL) {
            ip = "2055";
        }
    } else {
        ip = "2055";
    }
    char *udpcall;
    if (0 > asprintf(&udpcall, "udp-client/echo-udp-client2 %s %s %d < datafile", rest, ip, 24 + 48 * expired_cnt)) exit(6);
    FILE *fd = popen(udpcall, "r");
    free(udpcall);
    pclose(fd);
    all_flows_cnt += expired_cnt;
    expired_cnt = 0;
}

void exportFlow(flow_t *flow) {
    // time_t secs = sysTime().tv_sec;
    // time_t nsecs = sysTime().tv_usec;
    // time_t curr = secs + nsecs / 1000000;
    // time_t first = curr - sysUptime() / 1000000 + flow->first / 1000; // flow->first.tv_sec + flow->first.tv_usec / 1000000;
    // time_t last = curr - sysUptime() / 1000000 + flow->last / 1000;   // flow->last.tv_sec + flow->last.tv_usec / 1000000;
    // printf("\n");
    // printf("number of packets:%d\n", flow->dPkts);
    // printf("Bytes:%d\n", flow->bytes);
    // printf("num in first:%ld\n", first); // flow->first.tv_sec);
    // printf("num in last:%ld\n", last);   // flow->last.tv_sec);
    // printf("First time: %s", asctime(gmtime(&first)));
    // printf("Last time: %s", asctime(gmtime(&last)));
    // printf("src port: %u\n", ntohs(flow->flow_id.src_port));
    // printf("dst port: %u\n", ntohs(flow->flow_id.dst_port));
    // printf("protocol (6 = TCP, 1 = ICMP 17 = UDP): %d\n", flow->flow_id.prot);

    // real export
    if (sysUptime() - flow->last > flow->last / 1000 + export * 1000) {
        flow->exportUptime = flow->last / 1000 + export * 1000;
    } else {
        flow->exportUptime = (sysUptime() - flow->last) / 1000;
    }
    if (expired_cnt < 30) {
        expiredFlows[expired_cnt] = flow;
        // expiredFlows[expired_cnt] = flow;
        expired_cnt++;
        return;
    }
    exportExpired();
    expiredFlows[expired_cnt] = flow;
    // expiredFlows[expired_cnt] = flow;
    expired_cnt++;
}

void exportFlowAll() {
    // printf("Exporting all flows!\n");
    for (int i = 0; i < flow_cnt; i++) {
        // printf("\nflow no.%d", i);
        exportFlow(flow_cache[i]);
        flow_cache[i] = NULL;
    }
    exportExpired();
    free(flow_cache);
    flow_cache = NULL;
}

bool flowIDcmp(flow_id_t *f1, flow_id_t *f2) {
    if (f1->prot == 1) {
        return false;
    }
    if (f1 != NULL && f2 != NULL)
        if (f1->dst_ip == f2->dst_ip)
            if (f1->src_ip == f2->src_ip)
                if (f1->src_port == f2->src_port)
                    if (f1->dst_port == f2->dst_port)
                        if (f1->prot == f2->prot) return true;
    return false;
}

void updateFlow(flow_id_t *flow_ID) {
    if (flow_ID == NULL) {
        printf("exited by flow_ID=NULL\n");
        exit(5);
    }
    flow_t *flow = NULL;
    int fn = 0;
    for (int i = 0; i < flow_cnt; i++) {
        if (flowIDcmp(&flow_cache[i]->flow_id, flow_ID)) {
            flow = flow_cache[i];
            fn = i;
            break;
        }
    }
    setLatest(flow_ID->ts);
    if (flow == NULL) {
        setFirst(flow_ID->ts);
        if (flow_cnt == 1024) {
            // exportOldest();
            int fl_int = 0;
            flow_t *fl = flow_cache[fl_int];
            for (int i = 1; i < flow_cnt; i++) {
                if (fl->last > flow_cache[i]->last) {
                    fl_int = i;
                    fl = flow_cache[fl_int];
                }
            }
            exportFlow(fl);
            flow_cache[fl_int] = flow_cache[1023];
            flow_cache[1023] = NULL;
            printf("\nflow_cnt=30!!!!");
            flow_cnt--;
        }
        flow_t *newFlow = malloc(sizeof(flow_t));
        newFlow->flow_id = *flow_ID;
        newFlow->dPkts = 1;
        newFlow->first = sysUptime();
        newFlow->last = newFlow->first;
        newFlow->bytes = flow_ID->length;
        flow_cache[flow_cnt] = newFlow;
        flow_cnt++;
        return;
    }

    if ((flow->last - flow->first) / 1000000 > active) {
        exportFlow(flow_cache[fn]);
        flow_cnt--;
        flow_cache[fn] = flow_cache[flow_cnt];
        flow_cache[flow_cnt] = NULL;
        updateFlow(flow_ID);
        return;
    }
    if ((sysUptime() - flow->last) / 1000000 > export) {
        exportFlow(flow_cache[fn]);
        flow_cnt--;
        flow_cache[fn] = flow_cache[flow_cnt];
        flow_cache[flow_cnt] = NULL;
        updateFlow(flow_ID);
        return;
    }

    flow->flow_id.tcp_flags |= flow_ID->tcp_flags;
    flow->bytes += flow_ID->length;
    flow->dPkts++;
    flow->last = sysUptime();
    flow = NULL;
}

/*** Koniec suboru f_handle.c ***/
