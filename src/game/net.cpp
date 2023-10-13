#include "net.h"

#include <stdio.h>

#include <thread>
#include <vector>

#include "../engine/sockets/common.h"

void net_client(std::atomic_bool* should_exit, std::atomic_bool* msg_queued,
                std::vector<std::string>* msg_log, const char* msg, const char* addr, const char* port)
{
    ClientSocket client = ClientSocket();
    if (client.connect(addr, port) == false) {
        perror("Error connecting to server!\n");
        return;
    }

    // client.send("Hello, world!", 14);
    char buf[128] = {};

    for (;;) {
        if (*should_exit) break;

        if (client.recv(buf, 128) > 0) {
            printf("[Client] received : %s\n", buf);
            msg_log->push_back(std::string(buf));
            memset(buf, 0, 128);
        }

        if (*msg_queued == false) continue;

        if (client.send(msg, 128) == false) break;
        msg_log->push_back(std::string(msg));
        *msg_queued = false;
    }

    client.close();
}

// void net_handle_client(std::atomic_bool* should_exit,
//                        std::vector<std::string>* msg_log, ClientSocket* client)
// {
//     char buf[128] = {};
//     for (;;) {
//         if (*should_exit) break;
//         if (client->recv(buf, 128) <= 0) continue;

//         printf("[Server] received : %s\n", buf);
//         msg_log->push_back(std::string(buf));
//         memset(buf, 0, 128);
//     }

//     client->close();
//     delete client;
// }

void net_server(std::atomic_bool* should_exit,
                std::vector<std::string>* msg_log, const char* port)
{
    ServerSocket server = ServerSocket();
    if (server.bind(port) == false) {
        perror("Error binding server!\n");
        return;
    }

    /* Listen for clients */
    std::vector<ClientSocket*> clients;

    char buf[128] = {};
    for (;;) {
        if (*should_exit) break;

        ClientSocket* client = server.accept();
        if (client) {
            printf("[Server] client connected.\n");
            clients.push_back(client);
        }

        for (ClientSocket* client : clients)
        {
            if (client == nullptr) continue;

            if (client->recv(buf, 128) <= 0) continue;

            printf("[Server] received : %s\n", buf);

            for (ClientSocket* other_client : clients)
            {
                if (other_client == nullptr) continue;
                if (client->id == other_client->id) continue;

                other_client->send(buf, 128);
            }

            msg_log->push_back(std::string(buf));
            memset(buf, 0, 128);
        }
    }

    /* Join client threads! */
    for (ClientSocket* client : clients) {
        if (client == nullptr) continue;

        client->close();
        delete client;
    }

    server.close();
}

void net_thread(std::atomic_bool* should_exit, std::atomic_bool* msg_queued,
                std::vector<std::string>* msg_log, const char* msg,
                NetType type, const char* addr, const char* port)
{
    switch (type) {
        case NetType::CLIENT:
            printf("Client connecting to %s:%s\n", addr, port);

            net_client(should_exit, msg_queued, msg_log, msg, addr, port);
            break;

        case NetType::SERVER:
            printf("Server hosting at 127.0.0.1:%s\n", port);

            net_server(should_exit, msg_log, port);
            break;

        default: break;
    }
}
