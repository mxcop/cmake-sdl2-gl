#pragma once

/** Socket ID Type */
typedef unsigned long long SOCKET;

struct ClientSocket
{
    SOCKET id = 0;

    ClientSocket();

    /**
     * @brief Attempt to connect to a remote host.
     *
     * @param addr Server address.
     * @param port Server port.
     * @return True when connection was established succesfully.
     */
    bool connect(const char *addr, const char *port);

    /**
     * @brief Send a message to the connected host. (non-blocking)
     *
     * @param buf The message buffer.
     * @param len The length of the message to send in bytes.
     * @return True when message was send succesfully.
     */
    bool send(const char *buf, int len);

    /**
     * @brief Receive n bytes from client. (non-blocking)
     *
     * @param out_buf Output buffer.
     * @param out_len Output buffer length.
     * @return The number of bytes received.
     */
    int recv(char *out_buf, int out_len);

    /**
     * @brief Close the socket.
     */
    void close();
};

struct ServerSocket
{
    SOCKET id;

    ServerSocket();

    /**
     * @brief Attempt to bind to a network port.
     *
     * @param port Server port.
     * @return True when binding was succesful.
     */
    bool bind(const char *port);

    /**
     * @brief Accept incoming clients. (non-blocking)
     *
     * @return nullptr if accepting failed.
     */
    ClientSocket *accept();

    /**
     * @brief Close the socket.
     */
    void close();
};
