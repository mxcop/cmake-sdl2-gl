#include "common.h"

// Windows
#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
#define WIN32_LEAN_AND_MEAN
#undef TEXT
#include <winsock2.h>
#include <ws2tcpip.h>

// Linux
#else
#define sprintf_s sprintf
typedef int SOCKET;
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
static const int INVALID_SOCKET = -1;
static const int SOCKET_ERROR   = -1;
#endif

#include <stdio.h>

bool socket_setup()
{
#ifdef _WIN32
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0) 
    {
        printf("WSAStartup() failed with error: %d\n", err);
        return false;
    }
#endif
    return true;
}

void socket_cleanup()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

bool socket_open(short port, unsigned int timeout_ms, SOCKET *out_socket)
{
    /* Create the new socket */
    *out_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (*out_socket == INVALID_SOCKET) {
        printf("socket() failed");
        return false;
    }

    /* Prepare the socket address */
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    /* Bind the new socket to the correct address */
    if (bind(*out_socket, reinterpret_cast<struct sockaddr *>(&server), sizeof(server)) == SOCKET_ERROR) {
        printf("bind() failed");
        return false;
    }

    socket_udp_timeout(*out_socket, timeout_ms);

    return true;
}

void socket_udp_timeout(SOCKET socket, unsigned int ms)
{
#ifdef _WIN32
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&ms), sizeof(ms));
#else
    struct timeval timeout;
    timeout.tv_sec = ms / 1000;
    timeout.tv_usec = (ms * 1000) % 1000000;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif
}

void socket_inet_pton(const char* host, sockaddr_in& saddr_in)
{
#ifdef _WIN32
    WCHAR wsz[64];
    swprintf_s(wsz, L"%S", host);
    InetPton(AF_INET, reinterpret_cast<char*>(wsz), &(saddr_in.sin_addr.s_addr));
#else
    inet_pton(AF_INET, host, &(saddr_in.sin_addr));
#endif
}

void socket_close(SOCKET socket)
{
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}

void socket_send(SOCKET socket, const char* buf, int len, const sockaddr_in* target)
{
    sendto(socket, buf, len, 0, reinterpret_cast<const sockaddr*>(target), sizeof(*target));
}

bool socket_recv(SOCKET socket, char* buf, int len, sockaddr* out_sender, int* out_len)
{
    return recvfrom(socket, buf, len, 0, out_sender, out_len) == *out_len;
}
