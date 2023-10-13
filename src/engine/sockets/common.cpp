#include "common.h"

/* Winsock2 library */
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>

ClientSocket::ClientSocket()
{
    /* Startup winsock */
    WSADATA wsa_data;
	assert(!WSAStartup(MAKEWORD(2, 0), &wsa_data));

    /* Create the socket */
	this->id = socket(AF_INET, SOCK_STREAM, 0);
}

bool ClientSocket::connect(const char* addr, const char* port)
{
    SOCKADDR_IN sockaddr;

    /* Get the internet address of the server */
    inet_pton(AF_INET, addr, &sockaddr.sin_addr.s_addr);

    uint32_t msec = 0;
    setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, (char *) &msec, sizeof(msec));

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(atoi(port));

    /* Mark the socket as non-blocking */
    u_long NonBlock = 1;
    if (ioctlsocket(id, FIONBIO, &NonBlock) == SOCKET_ERROR) {
        return false;
    }

    return ::connect(id, reinterpret_cast<SOCKADDR *>(&sockaddr), sizeof(sockaddr)) <= 0;
}

bool ClientSocket::send(const char* buf, int len)
{
    return ::send(id, buf, len, 0) > 0;
}

int ClientSocket::recv(char* out_buf, int out_len)
{
    return ::recv(id, out_buf, out_len, 0);
}

void ClientSocket::close()
{
    closesocket(id);
	// WSACleanup();
}

ServerSocket::ServerSocket()
{
    /* Startup winsock */
    WSADATA wsa_data;
	assert(!WSAStartup(MAKEWORD(2, 2), &wsa_data));

    /* Create the socket */
	this->id = socket(AF_INET, SOCK_STREAM, 0);
}

bool ServerSocket::bind(const char* port)
{
	SOCKADDR_IN addr;
    addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));

    /* Bind the socket, and start listening */
	if (::bind(id, reinterpret_cast<SOCKADDR *>(&addr), sizeof(addr)) > 0) {
        return false;
    }
	if (::listen(id, 0) > 0) { 
        return false;
    }

    /* Mark the socket as non-blocking */
    u_long NonBlock = 1;
    if (ioctlsocket(id, FIONBIO, &NonBlock) == SOCKET_ERROR) {
        return false;
    }
    return true;
}

ClientSocket* ServerSocket::accept()
{
    SOCKET client;

    SOCKADDR_IN client_addr;
    int client_addr_len = sizeof(SOCKADDR_IN);
    
    /* Block until client connects */
    SOCKET client_socket = ::accept(id, reinterpret_cast<SOCKADDR *>(&client_addr), &client_addr_len);

    /* Return 'nullptr' if the socket is invalid */
    if (client_socket == INVALID_SOCKET) return nullptr;

    /* Otherwise return a new client socket */
    ClientSocket* socket = new ClientSocket();
    socket->id = client_socket;
    return socket;

    // const int last_error = WSAGetLastError();
    // if(last_error > 0)
    // {
    //     std::cout << "Error: " << last_error << std::endl;
    // }
}

void ServerSocket::close()
{
    closesocket(id);
	// WSACleanup();
}
