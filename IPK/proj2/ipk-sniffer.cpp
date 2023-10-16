/**
 * @file ipk-sniffer.cpp
 * @author Petr Bartoš (xbarto0g)
 * @brief Network sniffer, project for IPK course at FIT BUT
 *
 */

#include "ipk-sniffer.hpp"

void printHelp()
{
    std::cout << "Network sniffer\n"
                 "Usage:\n"
                 "./ipk-sniffer [-i interface | --interface interface] {-p port [--tcp|-t] [--udp|-u]} "
                 "[--arp] [--icmp4] [--icmp6] [--igmp] [--mld] {-n num}\n\n"
                 "--interface <i>:     The interface to sniff (if no interface is provided,\n"
                 "                     a list of active interfaces is printed)\n"
                 "--tcp <t>:           Display TCP segments\n"
                 "--udp <u>:           Display UDP datagrams\n"
                 "-p:                  Filter TCP/UDP based on port number\n"
                 "--icmp4:             Display only ICMPv4 packets\n"
                 "--icmp6:             Display only ICMPv6 echo request/response\n"
                 "--arp:               Display only ARP frames\n"
                 "--ndp:               Display only ICMPv6 NDP packets\n"
                 "--igmp:              Display only IGMP packets\n"
                 "--mld:               Display only MLD packets\n"
                 "-n:                  Number of packets to display (1 by default)\n"
                 "--help:              Display this message\n";
}

void hexDump(const u_char *bytes, bpf_u_int32 len)
{
    std::cout << std::hex << std::setfill('0') << std::endl;
    long addr = 0;
    while (addr <= len)
    {
        int nBytes;
        char buff[16];

        // Split the payload into chunks of 16 bytes (or less, if the payload length isn't a multiple of 16)
        for (nBytes = 0; nBytes < 16 && addr + nBytes < len; nBytes++)
            buff[nBytes] = bytes[addr + nBytes];

        std::cout << "0x" << std::setw(4) << addr << ':';

        // Print the hexadecimal representation
        for (int i = 0; i < 16; i++)
        {
            if (i < nBytes)
                std::cout << ' ' << std::setw(2) << (unsigned int)(unsigned char)buff[i];
            else
                std::cout << "   ";
        }

        // Print human readable representation
        std::cout << " ";
        for (int i = 0; i < nBytes; i++)
        {
            if (i % 8 == 0)
                std::cout << ' ';
            std::cout << ((isprint(buff[i])) ? (char)buff[i] : '.');
        }
        std::cout << std::endl;
        addr += 16;
    }
    std::cout << std::endl;
}

void printMAC(uint8_t *etherHost)
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", etherHost[0], etherHost[1],
           etherHost[2], etherHost[3], etherHost[4], etherHost[5]);
}

void printIPv4(uint8_t *ipHost)
{
    printf("%d.%d.%d.%d\n", ipHost[0], ipHost[1], ipHost[2], ipHost[3]);
}

void printIPv6(struct in6_addr addr)
{
    char buff[INET6_ADDRSTRLEN];

    inet_ntop(AF_INET6, &addr, buff, INET6_ADDRSTRLEN);
    printf("%s\n", buff);
}

void printTimestamp(timeval timestamp)
{
    std::stringstream offsetString;
    tm *localTime = localtime(&timestamp.tv_sec);
    offsetString << std::put_time(localTime, "%z");
    // put_time function doesn't output usec, so 3 digits are printed manually
    // it also outputs timezone offset as 4 digits preceded by a sign, so a colon needs to be added
    std::cout << std::put_time(localTime, "%FT%T") << "."
              << std::setfill('0') << std::setw(3) << std::to_string(timestamp.tv_usec).substr(0, 3)
              << offsetString.str().insert(3, ":") << std::endl;
}

void captureCallback(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes)
{
    (void)user;
    // Cast the payload to ethernet header
    struct ether_header *ethernetHeader = (struct ether_header *)bytes;

    std::cout << "timestamp: ";
    printTimestamp(h->ts);
    std::cout << "src MAC: ";
    printMAC(ethernetHeader->ether_shost);
    std::cout << "dst MAC: ";
    printMAC(ethernetHeader->ether_dhost);
    std::cout << std::dec << "frame length: " << h->len << " bytes" << std::endl;

    switch (ntohs(ethernetHeader->ether_type))
    {
    case ETHERTYPE_IP:
    {
        // Manually decapsulate the packet to obtain IPv4 data
        struct ip *ipHeader = (struct ip *)&bytes[sizeof(struct ether_header)];
        std::cout << "src IP: " << inet_ntoa(ipHeader->ip_src) << std::endl;
        std::cout << "dst IP: " << inet_ntoa(ipHeader->ip_dst) << std::endl;
        size_t ipHeaderLen = ipHeader->ip_hl << 2;
        if (ipHeader->ip_p == IPPROTO_TCP)
        {
            // Decapsulate once more, this time to obtain TCP/UDP data
            struct tcphdr *tcpHeader = (struct tcphdr *)&bytes[sizeof(struct ether_header) + ipHeaderLen];
            std::cout << "src port: " << ntohs(tcpHeader->th_sport) << std::endl;
            std::cout << "dst port: " << ntohs(tcpHeader->th_dport) << std::endl;
        }
        if (ipHeader->ip_p == IPPROTO_UDP)
        {
            struct udphdr *udpHeader = (struct udphdr *)&bytes[sizeof(struct ether_header) + ipHeaderLen];
            std::cout << "src port: " << ntohs(udpHeader->uh_sport) << std::endl;
            std::cout << "dst port: " << ntohs(udpHeader->uh_dport) << std::endl;
        }
        break;
    }

    case ETHERTYPE_IPV6:
    {
        // Handling of IPv6 is similar to the one of IPv4, the actual address is printed differently
        // and protocol information is accessed as a next header
        struct ip6_hdr *ipHeader = (struct ip6_hdr *)&bytes[sizeof(struct ether_header)];
        std::cout << "src IP: ";
        printIPv6(ipHeader->ip6_src);
        std::cout << "dst IP: ";
        printIPv6(ipHeader->ip6_dst);
        // 40 bytes as declared by RFC8200
        size_t ipHeaderLen = 40;
        if (ipHeader->ip6_ctlun.ip6_un1.ip6_un1_nxt == IPPROTO_TCP)
        {
            struct tcphdr *tcpHeader = (struct tcphdr *)&bytes[sizeof(struct ether_header) + ipHeaderLen];
            std::cout << "src port: " << ntohs(tcpHeader->th_sport) << std::endl;
            std::cout << "dst port: " << ntohs(tcpHeader->th_dport) << std::endl;
        }
        if (ipHeader->ip6_ctlun.ip6_un1.ip6_un1_nxt == IPPROTO_UDP)
        {
            struct udphdr *udpHeader = (struct udphdr *)&bytes[sizeof(struct ether_header) + ipHeaderLen];
            std::cout << "src port: " << ntohs(udpHeader->uh_sport) << std::endl;
            std::cout << "dst port: " << ntohs(udpHeader->uh_dport) << std::endl;
        }
        break;
    }

    case ETHERTYPE_ARP:
    {
        struct ether_arp *arpHeader = (struct ether_arp *)&bytes[sizeof(struct ether_header)];
        std::cout << "src IP: ";
        printIPv4(arpHeader->arp_spa);
        std::cout << "dst IP: ";
        printIPv4(arpHeader->arp_tpa);
        break;
    }

    default:
        break;
    }

    hexDump(bytes, h->len);
}

std::string getFilter(OPTS options)
{
    // Filters made using examples found at https://www.tcpdump.org/manpages/pcap-filter.7.html
    std::vector<std::string> modes = {
        "tcp",
        "udp",
        "icmp6[icmp6type]=icmp6-echo or icmp6[icmp6type]=icmp6-echoreply",
        "icmp",
        "arp",
        "icmp6[icmp6type]=icmp6-neighborsolicit or icmp6[icmp6type]=icmp6-neighboradvert",
        "igmp",
        "icmp6[icmp6type]=icmp6-multicastlistenerquery or icmp6[icmp6type]=icmp6-multicastlistenerreportv1 or "
        "icmp6[icmp6type]=icmp6-multicastlistenerdone or icmp6[icmp6type]=icmp6-multicastlistenerreportv2"};
    std::string filter;
    bool noFilter = options.filters.none();
    for (std::size_t i = 0; i < options.filters.size(); ++i)
    {
        if (options.filters[i] || noFilter)
            filter.append("(" + modes[i] + ") or ");
        if (options.port != -1 && i < 2 && options.filters[i])
            filter.insert(filter.find_last_of(")"), " port " + std::to_string(options.port));
    }
    return filter.substr(0, filter.size() - 4);
}

std::vector<std::string> getInterfaces()
{
    std::vector<std::string> interfaces;

    pcap_if_t *interfaceList;
    char errBuff[PCAP_ERRBUF_SIZE];
    if (pcap_findalldevs(&interfaceList, errBuff) != 0)
    {
        std::cerr << "ERROR: Couldn't get a list of network interfaces\n";
        exit(1);
    }

    pcap_if_t *interface = interfaceList;
    while (interface)
    {
        interfaces.push_back(interface->name);
        interface = interface->next;
    }
    pcap_freealldevs(interfaceList);

    return interfaces;
}

OPTS parseArgs(int argc, char *argv[])
{
    std::vector<std::string> interfaces = getInterfaces();
    if (argc == 1)
    {
        for (std::string interface : interfaces)
        {
            std::cout << interface << std::endl;
        }
        exit(0);
    }

    OPTS options = {.interface = "err", .filters = {0}, .port = -1, .packets = 1};
    int c;

    struct option longOpts[] =
        {
            {"help", no_argument, NULL, 'h'},
            {"interface", optional_argument, NULL, 'i'},
            {"tcp", no_argument, NULL, 't'},
            {"udp", no_argument, NULL, 'u'},
            {"icmp4", no_argument, NULL, '4'},
            {"icmp6", no_argument, NULL, '6'},
            {"arp", no_argument, NULL, 'a'},
            {"ndp", no_argument, NULL, 'd'},
            {"igmp", no_argument, NULL, 'g'},
            {"mld", no_argument, NULL, 'm'},
            {0, 0, 0, 0}};
    char shortOpts[] = "i::p:n:tu";
    int optIndex = 0;

    while ((c = getopt_long(argc, argv, shortOpts, longOpts, &optIndex)) != -1)
    {
        if (c == -1)
            break;

        switch (c)
        {
        case 'h':
            printHelp();
            exit(0);
            break;
        case 'i':
        {
            // Check if user has provided an optional argument
            if (!optarg && optind < argc && argv[optind] != NULL && argv[optind][0] != '\0' && argv[optind][0] != '-')
            {
                // Check if the provided interface is available
                std::string providedInterface = options.interface = argv[optind++];
                if (std::find(interfaces.begin(), interfaces.end(), providedInterface) != interfaces.end())
                {
                    options.interface = providedInterface;
                }
                else
                {
                    std::cerr << "ERROR: Provided interface isn't valid\n";
                    exit(1);
                }
            }
            else
            {
                if (argc == 2)
                {
                    for (std::string interface : interfaces)
                    {
                        std::cout << interface << std::endl;
                    }
                    exit(0);
                }
                else
                {
                    std::cerr << "ERROR: To show available interfaces, run the program without any argument or only with -i\n";
                    exit(1);
                }
            }
            break;
        }
        case 't':
            options.filters[0] = true;
            break;
        case 'u':
            options.filters[1] = true;
            break;
        case '6':
            options.filters[2] = true;
            break;
        case '4':
            options.filters[3] = true;
            break;
        case 'a':
            options.filters[4] = true;
            break;
        case 'd':
            options.filters[5] = true;
            break;
        case 'g':
            options.filters[6] = true;
            break;
        case 'm':
            options.filters[7] = true;
            break;
        case 'p':
        {
            char *tail;
            long port = strtol(optarg, &tail, 10);
            if (*tail != '\0')
            {
                std::cerr << "ERROR: Provided port couldn't be converted to a number\n";
                exit(1);
            }

            if (port < 0 || port > 65535)
            {
                std::cerr << "ERROR: Port out of supported range\n";
                exit(1);
            }
            options.port = (int)port;
            break;
        }
        case 'n':
        {
            char *tail;
            long packets = strtol(optarg, &tail, 10);
            if (*tail != '\0')
            {
                std::cerr << "ERROR: Provided nomber of packets couldn't be converted to an integer\n";
                exit(1);
            }

            if (packets < 0)
            {
                std::cerr << "ERROR: Number of packets has to be a positive integer\n";
                exit(1);
            }

            options.packets = packets;
            break;
        }
        default:
            std::cerr << "ERROR: Unrecognized argument, use ./ipk-sniffer --help to show help\n";
            exit(1);
        }
    }
    if ((!(options.filters[0] || options.filters[1]) && options.port != -1) || !options.interface.compare("err"))
    {
        std::cerr << "ERROR: Client cannot start with provided arguments, use ./ipk-sniffer --help to show help\n";
        exit(1);
    }
    return options;
}

void handleInterrupt(int signalNumber)
{
    (void)signalNumber;
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, handleInterrupt);
    OPTS options = parseArgs(argc, argv);

    char errBuffer[PCAP_ERRBUF_SIZE];
    bpf_u_int32 networkNumber;
    bpf_u_int32 networkMask;
    if (pcap_lookupnet(options.interface.c_str(), &networkNumber, &networkMask, errBuffer) == -1)
    {
        std::cerr << errBuffer << std::endl;
        return 1;
    }

    pcap_t *captDescr = pcap_open_live(options.interface.c_str(), BUFSIZ, 0, 1337, errBuffer);
    if (captDescr == NULL)
    {
        std::cerr << errBuffer << std::endl;
        return 1;
    }

    struct bpf_program filterProgram;
    std::string filter = getFilter(options);
    if (pcap_compile(captDescr, &filterProgram, filter.data(), 0, networkNumber) == -1)
    {
        std::cerr << "Compilation of filter program has failed" << std::endl;
        return 1;
    }

    if (pcap_setfilter(captDescr, &filterProgram) == -1)
    {
        std::cerr << "Couldn't specify a filter program" << std::endl;
        return 1;
    }

    if (pcap_loop(captDescr, options.packets, captureCallback, NULL) == -1)
    {
        std::cerr << "An error has occured when processing packets" << std::endl;
        return 1;
    }

    return 0;
}
