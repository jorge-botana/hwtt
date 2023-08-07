// -----------------------------------------------------------------------------
// ETH_C
//
// - Ethernet routines
//
// -----------------------------------------------------------------------------
// Copyright (c) 2023 Jorge Botana Mtz. de Ibarreta
//
// This source code is distributed under the terms of the MIT License. For more
// information, please see the LICENSE.txt file or refer to the following link:
//
//                                           https://opensource.org/license/mit/
// -----------------------------------------------------------------------------

#ifndef   WIN32
#error    "This program is targeted to systems running a Microsoft Windows OS!"
#else  // WIN32

// ------------------------ Private headers includes ------------------------ //

#include  <winsock2.h>
#include  <ws2tcpip.h>
#include  "public.h"

#if       (ETH == 1)

// ---------------------- Private preprocessor macros ----------------------- //

#define   MIN_IPV4_ADDR_LEN                                                    7
#define   MAX_IPV4_ADDR_LEN                                                   15

#define   MIN_TCP_PORT_LEN                                                     1
#define   MAX_TCP_PORT_LEN                                                     5

#define   MIN_TCP_PORT_NUM                                                     1
#define   MAX_TCP_PORT_NUM                                                 65535

#define   WSA_VERSION                                                          3
#define   WSA_SUBVERSION                                                       2
#define   WSA_VALUE           (WSA_VERSION << BITS_IN_ONE_BYTE) | WSA_SUBVERSION

// -------------------- Private data types declarations --------------------- //

// --------------- Public global data holders initializations --------------- //

const char coms[] = "ETH";

// -------------- Private global data holders initializations --------------- //

static int s = 0;

// --------------------- Private functions declarations --------------------- //

// ---------------------- Public functions definitions ---------------------- //

int init_coms(void) {
    // Setup the communications.
    write_header(stdout, report, "Ethernet Communications Setup");

    // Request the IPv4 address of the PCBA to be tested.
    const  char ip_addr_fie[] = " <- Remote IPv4 address       : ";
    static char ip_addr_dat[MAX_IPV4_ADDR_LEN + NULL_TERMIN_SIZE] = {
            DEF_IPV4_ADDR
    };
    size_t      ip_addr_len = strlen(ip_addr_dat);
    input(stdout, report, ip_addr_fie, ip_addr_dat, ip_addr_len,
            MIN_IPV4_ADDR_LEN, MAX_IPV4_ADDR_LEN, num_dot);

    // Check if the entered IPv4 address is valid.
    output(stdout, report, " -> Checking IPv4 address ..... ");
    char dummy[DEF_SMA_BUF_SIZE] = {0};
    ret = inet_pton(AF_INET, ip_addr_dat, dummy);
    if (ret == 0) {
        output(stdout, report, error_msg);
        output(stdout, report, "\n");
        print_error("Invalid IPv4 address.");

        return 1;
    }
    output(stdout, report, ok_msg);
    output(stdout, report, "\n");
    output(stdout, report, "\n");

    // Request the TCP port of the PCBA to be tested.
    const  char tcp_port_fie[] = " <- Remote TCP port           : ";
    static char tcp_port_dat[MAX_TCP_PORT_LEN + NULL_TERMIN_SIZE] = {
            DEF_TCP_PORT
    };
    size_t      tcp_port_len = strlen(tcp_port_dat);
    input(stdout, report, tcp_port_fie, tcp_port_dat, tcp_port_len,
            MIN_TCP_PORT_LEN, MAX_TCP_PORT_LEN, numbers);

    // Check if the entered TCP port is valid.
    output(stdout, report, " -> Checking TCP port ......... ");
    int tcp_port_num = atoi(tcp_port_dat);
    if (tcp_port_num < MIN_TCP_PORT_NUM || tcp_port_num > MAX_TCP_PORT_NUM) {
        output(stdout, report, error_msg);
        output(stdout, report, "\n");
        print_error("Invalid TCP port.");

        return 1;
    }
    output(stdout, report, ok_msg);
    output(stdout, report, "\n");
    output(stdout, report, "\n");

    // Initialize the TCP/IP stack.
    output(stdout, report, " -> Initializing TCP/IP ....... ");
    WSADATA wsaData = {0};
    ret = WSAStartup(WSA_VALUE, &wsaData);
    if (ret != 0) {
        output(stdout, report, error_msg);
        output(stdout, report, "\n");
        print_error(NULL);

        return 1;
    }
    output(stdout, report, ok_msg);
    output(stdout, report, "\n");

    // Create the socket.
    output(stdout, report, " -> Creating the socket ....... ");
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        output(stdout, report, error_msg);
        output(stdout, report, "\n");
        print_error(NULL);
        WSACleanup();

        return 1;
    }
    output(stdout, report, ok_msg);
    output(stdout, report, "\n");

    // Connect to the tested PCBA.
    output(stdout, report, " -> Connecting to server ...... ");
    struct sockaddr_in dst = {
        .sin_family      = AF_INET,
        .sin_port        = htons(tcp_port_num),
        .sin_addr.s_addr = inet_addr(ip_addr_dat)
    };
    ret = connect(s, (struct sockaddr *)&dst, sizeof(dst));
    if (ret == SOCKET_ERROR) {
        output(stdout, report, error_msg);
        output(stdout, report, "\n");
        print_error(NULL);
        WSACleanup();

        return 1;
    }
    output(stdout, report, ok_msg);
    output(stdout, report, "\n");

    return 0;
}

int shut_coms(void) {
    // End the connection.
    ret = WSACleanup();
    if (ret != 0) {
        return 1;
    }

    return 0;
}

int send_buf(const char *buf, size_t len) {
    // Send a buffer of len number of bytes.
    ret = send(s, buf, len, 0);
    if (ret == SOCKET_ERROR) {
        return 1;
    }

    return 0;
}

int recv_buf(char *buf, size_t len) {
    // Receive a buffer of len number of bytes.
    ret = recv(s, buf, len, 0);
    if (ret == SOCKET_ERROR) {
        return 1;
    }

    return 0;
}

// --------------------- Private functions definitions ---------------------- //

// -----------------------------------------------------------------------------

#endif // (ETH == 1)

#endif // WIN32
