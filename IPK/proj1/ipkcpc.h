/**
 * @file ipkcpc.h
 * @author Petr Bartoš (xbarto0g)
 * @brief Header file for the IPK Calculator Protocol client
 *
 */
#define _XOPEN_SOURCE 700
#include <stdbool.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <errno.h>
#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#define TCPBUFFSIZE 1024
#define UDPBUFFSIZE 258

typedef struct
{
    char *host;
    int port;
    int mode;
} OPTS;

/**
 * @brief Custom SIGINT handler, which sets the terminated flag to 1.
 *
 */
void handleInterrupt();

/**
 * @brief Prints help message with program parameters and their formats.
 *
 */
void printHelp();

/**
 * @brief Stop reception and transmission, then close the socket.
 *
 * @param clientSocket Socket used for communication.
 */
void closeConnection(int clientSocket);

/**
 * @brief Estabilishes a connection on provided socket and starts communicating with
 * the server in textual variant over TCP.
 *
 * @param clientSocket Socket used for communication.
 * @param serverAddress Address of a server.
 */
void commOverTCP(int clientSocket, struct sockaddr_in serverAddress);

/**
 * @brief Starts communicating with the server in binary variant over UDP.
 *
 * @param clientSocket Socket used for communication.
 * @param serverAddress Address of a server.
 */
void commOverUDP(int clientSocket, struct sockaddr_in serverAddress);

/**
 * @brief Checks for existence of needed parameters and stores them in a option structure.
 * Raises an error in case of unrecognized parameter or parameter not following format.
 *
 * @param argc Number of arguments.
 * @param argv Argument vector.
 * @return OPTS Structure with user specified parameters.
 */
OPTS parseArgs(int argc, char *argv[]);
