/**
 * @file ipk-sniffer.cpp
 * @author Petr Bartoš (xbarto0g)
 * @brief Header file for network sniffer
 *
 */

#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <getopt.h>
#include <pcap.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <signal.h>

typedef struct
{
    std::string interface;
    // tcp, udp, icmp6, icmp4, arp, ndp, igmp, mld
    std::bitset<8> filters;
    int port;
    long packets;
} OPTS;

/**
 * @brief Prints help message with accepted arguments
 *
 */
void printHelp();

/**
 * @brief Prints contents of a packet in hexadecimal and human readable formats
 *
 * @param bytes Data of a packet
 * @param len Length of the data
 */
void hexDump(const u_char *bytes, bpf_u_int32 len);

/**
 * @brief Prints MAC address of a host in human readable format
 *
 * @param etherHost Address of a host
 */
void printMAC(uint8_t *etherHost);

/**
 * @brief Prints IPv4 address of a host in human readable format
 *
 * Not to be confused with inet_ntoa
 *
 * @param ipHost Host address obtained from ARP packet
 */
void printIPv4(uint8_t *ipHost);

/**
 * @brief Prints IPv6 address of a host in human readable format
 *
 * @param addr Address of a host
 */
void printIPv6(struct in6_addr addr);

/**
 * @brief Prints timestamp in RFC 3339 format
 *
 * @param timestamp Timestamp of the capture
 */
void printTimestamp(timeval timestamp);

/**
 * @brief Callback fuction for further processing of captured packets
 *
 * @param user User-defined parameter that contains the state of the capture session
 * @param h Header associated by the capture driver to the packet
 * @param bytes Data of the packet, including the protocol headers
 */
void captureCallback(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes);

/**
 * @brief Generates a string for pcap filter
 *
 * @param options Application options
 * @return std::string String compliant with pcap filter format
 */
std::string getFilter(OPTS options);

/**
 * @brief Get a list of available interfaces
 *
 * @return std::vector<std::string> Array of interfaces
 */
std::vector<std::string> getInterfaces();

/**
 * @brief Checks validity of provided arguments
 *
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @return OPTS Options for the application
 */
OPTS parseArgs(int argc, char *argv[]);
