/**
 * ISA
 * @file flow.c
 * @authors Matus Vrablik (xvrab05)
 * @brief NetFlow exporter
 */

#include "flow.h"

FILE *f;
char *ip_addr = "127.0.0.1:2055";
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
    switch (ntohs(eptr->ether_type)) {
        case ETHERTYPE_IP:
            break;
        default:
            return;
    }

    // packet without datalink layer header
    u_char *noHeadPacket = packet + header_length;

    // create flow ID
    flow_id_t *flow_ID = malloc(sizeof(flow_id_t));
    flow_ID->ts = header->ts;
    // printf("time:%s", asctime(gmtime(&header->ts.tv_sec)));

    struct ip *iphdr;
    iphdr = (struct ip *)(packet + header_length);
    flow_ID->src_ip = ntohl(iphdr->ip_src.s_addr);
    flow_ID->dst_ip = ntohl(iphdr->ip_dst.s_addr);
    flow_ID->prot = iphdr->ip_p;

    struct tcphdr *tcphdr;
    struct udphdr *udphdr;
    struct icmphdr *icmphdr;
    int size_ip = iphdr->ip_hl * 4;
    switch (iphdr->ip_p) {
        case IPPROTO_TCP:
            tcphdr = (struct tcphdr *)(noHeadPacket + size_ip);
            flow_ID->src_port = tcphdr->th_sport;
            flow_ID->dst_port = tcphdr->th_dport;
            flow_ID->tcp_flags = tcphdr->th_flags;
            flow_ID->length = ntohs(iphdr->ip_len) - size_ip;
            break;

        case IPPROTO_UDP:
            udphdr = (struct udphdr *)(noHeadPacket + size_ip);
            flow_ID->src_port = udphdr->source;
            flow_ID->dst_port = udphdr->dest;
            flow_ID->tcp_flags = 0;
            flow_ID->length = ntohs(udphdr->len) - iphdr->ip_hl * 1.5;
            break;

        case IPPROTO_ICMP:
            icmphdr = (struct icmphdr *)(noHeadPacket + size_ip);
            flow_ID->length = 56;
            break;

        default:
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
            ip_addr = argv[++i];
            // host = gethostbyaddr(ip_addr, )
            // gethostbyaddr()
            // gethostbyname()
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
        exit(1);
    }
}

int main(int argc, char *argv[argc]) {
    argparse(argc, argv);

    setVars(ip_addr, active_timer, export_interval, flow_cache_size);
    initFlowArray();

    // Constants print
    printf("IP address/hostname: %s\n", ip_addr);
    printf("Active timer: %d\n", active_timer);
    printf("Export interval: %d\n", export_interval);
    printf("Flow cache size: %d\n\n", flow_cache_size);
    // Constants print - end

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

    printf("Total packets: %d\n", total_packets);
    return 0;
}

/*** Koniec suboru <nazov_suboru>.{c,h} ***/
