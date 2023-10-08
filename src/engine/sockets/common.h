#pragma once

typedef unsigned long long SOCKET;
struct sockaddr;
struct sockaddr_in;

/**
 * @brief Setup the program for using Sockets. (necessary for Windows)
 */
bool socket_setup();

/**
 * @brief Cleanup the sockets of this program. (necessary for Windows)
 */
void socket_cleanup();

bool socket_open(short port, unsigned int timeout_ms, SOCKET *out_socket);
void socket_udp_timeout(SOCKET socket, unsigned int ms);
void socket_inet_pton(const char* host, sockaddr_in& saddr_in);
void socket_close(SOCKET socket);

/**
 * @brief Send a msg through a socket.
 * 
 * @param socket The socket to use.
 * @param buf A pointer to the start of the msg data.
 * @param len The length of the msg data buffer.
 * @param target The target address to send the msg to.
 */
void socket_send(SOCKET socket, const char* buf, int len, const sockaddr_in* target);

/**
 * @brief Receive a msg through a socket.
 * 
 * @param socket The socket to use.
 * @param buf A pointer to the buffer to write the received data into.
 * @param len The length of the receive buffer.
 * @param out_sender Socket address of the sender of the received msg.
 * @param out_len Length of the socket address of the sender.
 * @return True if a msg was received.
 */
bool socket_recv(SOCKET socket, char* buf, int len, sockaddr* out_sender, int* out_len);
