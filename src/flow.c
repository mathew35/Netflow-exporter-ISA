/**
 * ISA
 * @file flow.c
 * @authors Matus Vrablik (xvrab05)
 * @brief NetFlow exporter
 */

#include "flow.h"

FILE *f;
char *ip_addr = "127.0.0.1";
char *port = "2055";
int active_timer = 60;
int export_interval = 10;
int flow_cache_size = 1024;
int header_length = 0;
struct hostent *host;

int total_packets = 0;
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    total_packets++;
    struct ether_header *eptr;
    eptr = (struct ether_header *)packet;
    if (ntohs(eptr->ether_type) != ETHERTYPE_IP) return;

    u_char *noHeadPacket = packet + header_length;

    // create flow ID
    flow_id_t *flow_ID = malloc(sizeof(flow_id_t));
    if (flow_ID == NULL) {
        printf("exited by flow_ID=NULL\n");
        exit(5);
    }
    flow_ID->ts = header->ts;

    struct ip *iphdr;
    iphdr = (struct ip *)(noHeadPacket);
    flow_ID->src_ip = ntohl(iphdr->ip_src.s_addr);
    flow_ID->dst_ip = ntohl(iphdr->ip_dst.s_addr);
    flow_ID->prot = iphdr->ip_p;
    flow_ID->tos = iphdr->ip_tos;

    struct tcphdr *tcphdr;
    struct udphdr *udphdr;
    struct icmphdr *icmphdr;
    int size_ip = iphdr->ip_hl * 4;
    switch (iphdr->ip_p) {
        case IPPROTO_TCP:
            tcphdr = (struct tcphdr *)(noHeadPacket + size_ip);
            flow_ID->src_port = tcphdr->source;
            flow_ID->dst_port = tcphdr->dest;
            flow_ID->tcp_flags = (tcphdr->fin | tcphdr->syn << 1 | tcphdr->rst << 2 | tcphdr->psh << 3 | tcphdr->ack << 4 | tcphdr->urg << 5);
            flow_ID->length = ntohs(iphdr->ip_len) - size_ip;
            break;

        case IPPROTO_UDP:
            udphdr = (struct udphdr *)(noHeadPacket + size_ip);
            flow_ID->src_port = udphdr->source;
            flow_ID->dst_port = udphdr->dest;
            flow_ID->tcp_flags = 0;
            flow_ID->length = ntohs(udphdr->len) - 4 * 2;
            break;

        case IPPROTO_ICMP:
            icmphdr = (struct icmphdr *)(noHeadPacket + size_ip);
            flow_ID->length = (ntohs(iphdr->ip_len) - size_ip - 8);
            flow_ID->src_port = icmphdr->type;
            flow_ID->dst_port = icmphdr->code;
            flow_ID->tcp_flags = 0;
            break;

        default:
            printf("unknown prot:%d\n", iphdr->ip_p);
            return;
            break;
    }
    updateFlow(flow_ID);
    free(flow_ID);
}

void argparse(int argc, char *argv[argc]) {
    f = stdin;
    // host = getaddrinfo();
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            f = fopen(argv[++i], "r");
            continue;
        }
        if (strcmp(argv[i], "-c") == 0) {
            char *ip = argv[++i];
            // separate address and port
            ip_addr = strsep(&ip, ":");
            if (ip != NULL) {
                port = ip;
            }
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
            if (flow_cache_size < 1) flow_cache_size = 1;
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
        exit(1);
    }
}

int main(int argc, char *argv[argc]) {
    argparse(argc, argv);

    openSocket(ip_addr, port);
    setVars(ip_addr, active_timer, export_interval, flow_cache_size);
    initFlowArray();

    // Init pcap
    pcap_t *packetHandler = pcap_fopen_offline(f, "r");
    // Get link type (ETHERNET WIFI SSL)
    int link_type = pcap_datalink(packetHandler);
    switch (link_type) {
        case DLT_NULL:
            header_length = 4;
            break;
        case DLT_EN10MB:
            header_length = 14;
            break;
        case DLT_SLIP:
        case DLT_PPP:
            header_length = 24;
            break;
        case DLT_LINUX_SLL:
            header_length = 16;
            break;
        default:
            printf("Unsupported datalink (%d)\n", link_type);
            return 2;
    }
    pcap_loop(packetHandler, -1, got_packet, NULL);
    pcap_close(packetHandler);
    exportFlowAll();
    freeFlows();
    closeSocket();

    // printf("Total packets: %d\n", total_packets);
    return 0;
}

/*** Koniec suboru <nazov_suboru>.{c,h} ***/
