/**
 * @file ipkcpc.c
 * @author Petr Bartoš (xbarto0g)
 * @brief Client for the IPK Calculator Protocol
 *
 * This program was built using stubs provided by doc. Ryšavý, which can be found at:
 * https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs
 *
 */
#include "ipkcpc.h"

volatile sig_atomic_t terminated = 0;
void handleInterrupt()
{
    terminated = 1;
}

void printHelp()
{
    printf("Client for the IPK Calculator Protocol\n\n");
#ifndef _WIN32
    printf("Usage: ipkcpc -h <host> -p <port> -m <mode>\n");
#else
    printf("Usage: ipkcpc <host> <port> <mode>\n");
#endif
    printf("  host is the IPv4 address of the server\n");
    printf("  port the server port\n");
    printf("  mode is either tcp or udp\n");
}

void closeConnection(int clientSocket)
{
    int status = 0;
#ifdef _WIN32
    status = shutdown(clientSocket, SD_BOTH);
    if (status == 0)
        status = closesocket(clientSocket);
    // Terminate use of the Winsock DLL
    WSACleanup();
#else
    status = shutdown(clientSocket, SHUT_RDWR);
    if (status == 0)
        status = close(clientSocket);
#endif
}

void commOverTCP(int clientSocket, struct sockaddr_in serverAddress)
{
    char buff[TCPBUFFSIZE];
    int sentBts, recvBts;

    if (connect(clientSocket, (const struct sockaddr *)&serverAddress, sizeof(serverAddress)) != 0)
    {
        fprintf(stderr, "ERROR: Couldn't open a connection on socket\n");
        closeConnection(clientSocket);
        exit(1);
    }

    while (1)
    {
        memset(buff, 0, TCPBUFFSIZE);
        if (fgets(buff, TCPBUFFSIZE, stdin) == NULL)
        {
            // Catch Ctrl-C or Ctrl-D (EOF)
            if ((errno == EINTR && terminated) || (!ferror(stdin) && buff[0] == '\0'))
                strcpy(buff, "BYE\n");
            else
            {
                fprintf(stderr, "ERROR: Couldn't read user's input\n");
                closeConnection(clientSocket);
                exit(1);
            }
        }

        sentBts = send(clientSocket, buff, strlen(buff), 0);
        if (sentBts < 0)
        {
            fprintf(stderr, "ERROR: Couldn't send data\n");
            closeConnection(clientSocket);
            exit(1);
        }

        memset(buff, 0, TCPBUFFSIZE);
        recvBts = recv(clientSocket, buff, TCPBUFFSIZE, 0);
        if (recvBts < 0)
        {
            fprintf(stderr, "ERROR: Couldn't receive data\n");
            closeConnection(clientSocket);
            exit(1);
        }

        printf("%s", buff);

        if (!strncmp("BYE", buff, strlen("BYE")))
            break;
    }
}

void commOverUDP(int clientSocket, struct sockaddr_in serverAddress)
{
    char buff[UDPBUFFSIZE];
    socklen_t serverlen;
    int sentBts, recvBts;

    while (1)
    {
        memset(buff, 0, UDPBUFFSIZE);
        // Only 255 bytes can be sent over UDP, there's no need to read more characters
        if (fgets(buff, UDPBUFFSIZE - 2, stdin) == NULL)
        {
            if ((errno == EINTR && terminated) || (!ferror(stdin) && buff[0] == '\0'))
                break;
            else
            {
                fprintf(stderr, "ERROR: Couldn't read user's input\n");
                closeConnection(clientSocket);
                exit(1);
            }
        }

        // Shift the buffer 2 bytes and insert opcode and payload length
        int payloadLength = strlen(buff) - 1;
        memmove(buff + 2, buff, strlen(buff) - 1);
        buff[0] = '\0';
        buff[1] = payloadLength;

        serverlen = sizeof(serverAddress);
        sentBts = sendto(clientSocket, buff, payloadLength + 2, 0, (struct sockaddr *)&serverAddress, serverlen);
        if (sentBts < 0)
        {
            fprintf(stderr, "ERROR: Couldn't send data\n");
            closeConnection(clientSocket);
            exit(1);
        }

        memset(buff, 0, UDPBUFFSIZE);
        recvBts = recvfrom(clientSocket, buff, UDPBUFFSIZE, 0, (struct sockaddr *)&serverAddress, &serverlen);
        if (recvBts < 0)
        {
            fprintf(stderr, "ERROR: Couldn't receive data\n");
            closeConnection(clientSocket);
            exit(1);
        }
        if (buff[1] == 0)
            printf("OK:%.*s\n", buff[2], buff + 3);
        else
            printf("ERR:%.*s\n", buff[2], buff + 3);
    }
}

OPTS parseArgs(int argc, char *argv[])
{
    OPTS options = {.port = -1, .host = NULL, .mode = -1};
#ifndef _WIN32
    int c;

    struct option longOpts[] =
        {
            {"help", no_argument, NULL, 'e'},
            {"host", required_argument, NULL, 'h'},
            {"port", required_argument, NULL, 'p'},
            {"mode", required_argument, NULL, 'm'},
            {0, 0, 0, 0}};
    char *shortOpts = "h:p:m:";
    int optIndex = 0;

    while ((c = getopt_long(argc, argv, shortOpts, longOpts, &optIndex)) != -1)
    {
        if (c == -1)
            break;

        switch (c)
        {
        case 'e':
            printHelp();
            exit(0);
            break;
        case 'h':
            options.host = optarg;
            break;
        case 'p':
            char *tail;
            long port = strtol(optarg, &tail, 10);
            if (*tail != '\0')
            {
                fprintf(stderr, "ERROR: Provided port couldn't be converted to a number\n");
                exit(1);
            }

            if (port < 0 || port > 65535)
            {
                fprintf(stderr, "ERROR: Port out of supported range\n");
                exit(1);
            }
            options.port = (int)port;
            break;
        case 'm':
            if (!strcmp(optarg, "tcp"))
            {
                options.mode = 0;
                break;
            }
            if (!strcmp(optarg, "udp"))
            {
                options.mode = 1;
                break;
            }
            fprintf(stderr, "ERROR: Unrecognized mode\n");
            exit(1);
            break;
        default:
            fprintf(stderr, "ERROR: Unrecognized argument\n");
            exit(1);
        }
    }
#else
    if (argc == 2)
    {
        if (!strcmp("--help", argv[1]))
        {
            printHelp();
            exit(0);
        }
        else
        {
            fprintf(stderr, "ERROR: Unrecognized argument\n");
            exit(1);
        }
    }

    if (argc != 4)
    {
        fprintf(stderr, "ERROR: Not enough arguments provided\n");
        exit(1);
    }
    else
    {
        options.host = argv[1];
        char *tail;
        long port = strtol(argv[2], &tail, 10);
        if (*tail != '\0')
        {
            fprintf(stderr, "ERROR: Provided port couldn't be converted to a number\n");
            exit(1);
        }

        if (port < 0 || port > 65535)
        {
            fprintf(stderr, "ERROR: Port out of supported range\n");
            exit(1);
        }
        options.port = (int)port;
        if (!strcmp(argv[3], "tcp"))
            options.mode = 0;
        if (!strcmp(argv[3], "udp"))
            options.mode = 1;
    }
#endif
    if (options.port == -1 || options.mode == -1 || options.host == NULL)
    {
        fprintf(stderr, "ERROR: Client cannot start with provided arguments\n");
        exit(1);
    }
    return options;
}

int main(int argc, char *argv[])
{
    OPTS options = parseArgs(argc, argv);
#ifdef _WIN32
    // Initiate use of the Winsock DLL
    WSADATA winData;
    if (WSAStartup(MAKEWORD(1, 1), &winData))
    {
        fprintf(stderr, "ERROR: Initiation of the Winsock DLL has failed\n");
        exit(1);
    }
    signal(SIGINT, handleInterrupt);
#else
    struct sigaction action = {0};
    action.sa_handler = &handleInterrupt;
    sigaction(SIGINT, &action, NULL);
#endif

    struct hostent *server;
    if ((server = gethostbyname(options.host)) == NULL)
    {
        fprintf(stderr, "ERROR: Couldn't find host %s\n", options.host);
        exit(1);
    }

    struct sockaddr_in serverAddress;
    memset((char *)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    memcpy((char *)&serverAddress.sin_addr.s_addr, (char *)server->h_addr_list[0], server->h_length);
    serverAddress.sin_port = htons(options.port);

    int clientSocket;
    if ((clientSocket = socket(AF_INET, (options.mode == 0) ? SOCK_STREAM : SOCK_DGRAM, 0)) <= 0)
    {
        fprintf(stderr, "ERROR: Couldn't create socket\n");
        exit(1);
    }

    // Set UDP timeout to prevent hanging
#ifdef _WIN32
    unsigned long timeout = 10 * 1000;
#else
    struct timeval timeout = {10, 0};
#endif
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout);

    if (options.mode == 0)
        commOverTCP(clientSocket, serverAddress);
    else
        commOverUDP(clientSocket, serverAddress);

    closeConnection(clientSocket);

    return 0;
}
