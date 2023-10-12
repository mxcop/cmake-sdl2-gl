#include "net.h"

#include <thread>
#include <vector>
#include <stdio.h>

#include "../engine/sockets/common.h"

void net_client(std::atomic_bool* should_exit, std::atomic_bool* msg_queued, const char* msg, const char* addr, const char* port) {
    ClientSocket client = ClientSocket();
    if (client.connect(addr, port) == false) {
        perror("Error connecting to server!\n");
        return;
    }

    bool succ = client.send("Hello, world!", 14);

    for(;;) {
        if (*should_exit) break;
        if (*msg_queued == false) continue;

        if (client.send(msg, 128) == false) break;
        *msg_queued = false;
    }

    // client.send("Hello, world!", 14);
    client.close();
}

void net_handle_client(std::atomic_bool* should_exit, std::vector<std::string>* msg_log, ClientSocket* client) {
    char buf[128] = {};
    for(;;) {
        if (*should_exit) break;
        if (client->recv(buf, 128) <= 0) continue;

        printf("[Server] received : %s\n", buf);
        msg_log->push_back(std::string(buf));
        memset(buf, 0, 128);
    }

    client->close();
    delete client;
}

void net_server(std::atomic_bool* should_exit, std::vector<std::string>* msg_log, const char* port) {
    ServerSocket server = ServerSocket();
    if (server.bind(port) == false) {
        perror("Error binding server!\n");
        return;
    }

    /* Listen for clients */
    std::vector<std::thread*> clients;

    for(;;) {
        if (*should_exit) break;

        ClientSocket* client = server.accept();
        if (client == nullptr) continue;

        printf("[Server] client connected.\n");

        /*! Thread will free the client */
        clients.push_back(new std::thread(net_handle_client, should_exit, msg_log, client));
    }

    /* Join client threads! */
    for (std::thread* th : clients) {
        if (th) th->join();
    }

    server.close();
}

void net_thread(std::atomic_bool* should_exit, std::atomic_bool* msg_queued, std::vector<std::string>* msg_log, const char* msg, NetType type, const char* addr, const char* port)
{
    switch (type) {
        case NetType::CLIENT:
            printf("Client connecting to %s:%s\n", addr, port);

            net_client(should_exit, msg_queued, msg, addr, port);
            break;

        case NetType::SERVER:
            printf("Server hosting at 127.0.0.1:%s\n", port);

            net_server(should_exit, msg_log, port);
            break;

        default: break;
    }
}
