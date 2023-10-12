#include "net.h"

#include <thread>
#include <vector>
#include <stdio.h>

#include "../engine/sockets/common.h"

void net_client(const char* addr, const char* port) {
    ClientSocket client = ClientSocket();
    if (client.connect(addr, port) == false) {
        perror("Error connecting to server!\n");
        return;
    }
    client.send("Hello, world!", 14);
    client.close();
}

void net_handle_client(ClientSocket* client) {
    char buf[128] = {};
    while(client->recv(buf, 128) > 0) {
        printf("[Server] received : %s\n", buf);
        memset(buf, 0, 128);
    }

    client->close();
    delete client;
}

void net_server(std::atomic_bool* should_exit, const char* port) {
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
        clients.push_back(new std::thread(net_handle_client, client));
    }

    /* Join client threads! */
    for (std::thread* th : clients) {
        if (th) th->join();
    }

    server.close();
}

void net_thread(std::atomic_bool* should_exit, NetType type, const char* addr, const char* port)
{
    switch (type) {
        case NetType::CLIENT:
            printf("Client connecting to %s:%s\n", addr, port);

            net_client(addr, port);
            break;

        case NetType::SERVER:
            printf("Server hosting at 127.0.0.1:%s\n", port);

            net_server(should_exit, port);
            break;

        default: break;
    }
}
