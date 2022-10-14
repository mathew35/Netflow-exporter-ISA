/**
 * ISA
 * @file flow.c
 * @authors Matus Vrablik (xvrab05)
 * @brief NetFlow exporter
 */

#include "flow.h"
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *f;
char *ip_addr = "127.0.0.1:2055";
int active_timer = 60;
int export_interval = 10;
int flow_cache_size = 1024;

int main(int argc, char *argv[argc]) {
    f = stdin;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            f = fopen(argv[++i], "r");
            continue;
        }
        if (strcmp(argv[i], "-c") == 0) {
            ip_addr = argv[++i];
            continue;
        }
        if (strcmp(argv[i], "-a") == 0) {
            active_timer = atoi(argv[++i]);
            continue;
        }
        if (strcmp(argv[i], "-i") == 0) {
            export_interval = atoi(argv[++i]);
            continue;
        }
        if (strcmp(argv[i], "-m") == 0) {
            flow_cache_size = atoi(argv[++i]);
            continue;
        }
        if (strcmp(argv[i], "-h") == 0) {
            printf("Usage: ./flow [-f <file>] [-c <netflow_collector>[:<port>]] [-a <active_timer>] [-i <inactive_timer>] [-m <count>]\n");
            printf("\t-f <file> jméno analyzovaného souboru nebo STDIN\n");
            printf("\t-a <active_timer> - interval v sekundách, po kterém se exportují aktivní záznamy na kolektor (60, pokud není specifikováno)\n");
            printf("\t-i <seconds> - interval v sekundách, po jehož vypršení se exportují neaktivní záznamy na kolektor (10, pokud není specifikováno)\n");
            printf("\t-m <count> - velikost flow-cache. Při dosažení max. velikosti dojde k exportu nejstaršího záznamu v cachi na kolektor (1024, pokud není specifikováno)\n");
            continue;
        }
        printf("Wrong option \"%s\".\n", argv[i]);
        printf("Use \"./flow -h\" for help\n");
        return 1;
    }
    printf("IP address/hostanme: %s\n", ip_addr);
    printf("Active timer: %d\n", active_timer);
    printf("Export interval: %d\n", export_interval);
    printf("Flow cache size: %d\n\n", flow_cache_size);

    // placeholder

    return 0;
}

/*** Koniec suboru <nazov_suboru>.{c,h} ***/
