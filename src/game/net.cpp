#include "net.h"

#include <SDL.h>
#include <stdio.h>

#include <thread>
#include <vector>

#include "../engine/sockets/common.h"

void net_client(std::atomic_bool *should_exit, std::atomic_bool *msg_queued, std::vector<std::string> *msg_log,
                const char *msg, const char *addr, const char *port)
{
    /* Create the client socket and connect to the server */
    ClientSocket client = ClientSocket();
    if (client.connect(addr, port) == false)
    {
        perror("Error connecting to server!\n");
        return;
    }

    char buf[128] = {};
    for (;;)
    {
        if (*should_exit)
            break;

        /* Receive incoming message if there is one */
        if (client.recv(buf, 128) > 0)
        {
            printf("[Client] received : %s\n", buf);
            msg_log->push_back(std::string(buf));
            memset(buf, 0, 128);
        }

        /* Send a message if it was queued */
        if (*msg_queued)
        {
            *msg_queued = false;

            if (client.send(msg, 128) == false)
                break;
            msg_log->push_back(std::string(msg));
        }

        SDL_Delay(10);
    }

    client.close();
}

void net_server(std::atomic_bool *should_exit, std::atomic_bool *msg_queued, std::vector<std::string> *msg_log,
                const char *msg, const char *port)
{
    ServerSocket server = ServerSocket();
    if (server.bind(port) == false)
    {
        perror("Error binding server!\n");
        return;
    }

    /* Listen for clients */
    std::vector<ClientSocket *> clients;

    char buf[128] = {};
    for (;;)
    {
        if (*should_exit)
            break;

        /* Accept incoming client */
        ClientSocket *client = server.accept();
        if (client)
        {
            printf("[Server] client connected.\n");
            clients.push_back(client);
        }

        /* Check if any message were send by clients */
        for (ClientSocket *client : clients)
        {
            if (client == nullptr)
                continue;

            if (client->recv(buf, 128) <= 0)
                continue;

            printf("[Server] received : %s\n", buf);

            /* Echo message to all other clients */
            for (ClientSocket *other_client : clients)
            {
                if (other_client == nullptr)
                    continue;
                if (client->id == other_client->id)
                    continue;

                other_client->send(buf, 128);
            }

            msg_log->push_back(std::string(buf));
            memset(buf, 0, 128);
        }

        /* Send a message if it was queued */
        if (*msg_queued)
        {
            *msg_queued = false;

            for (ClientSocket *client : clients)
            {
                if (client == nullptr)
                    continue;
                client->send(msg, 128);
            }

            msg_log->push_back(std::string(msg));
        }

        SDL_Delay(10);
    }

    /* Join client threads! */
    for (ClientSocket *client : clients)
    {
        if (client == nullptr)
            continue;

        client->close();
        delete client;
    }

    server.close();
}

void net_thread(std::atomic_bool *should_exit, std::atomic_bool *msg_queued, std::vector<std::string> *msg_log,
                const char *msg, NetType type, const char *addr, const char *port)
{
    switch (type)
    {
    case NetType::CLIENT:
        printf("Client connecting to %s:%s\n", addr, port);

        net_client(should_exit, msg_queued, msg_log, msg, addr, port);
        break;

    case NetType::SERVER:
        printf("Server hosting at 127.0.0.1:%s\n", port);

        net_server(should_exit, msg_queued, msg_log, msg, port);
        break;

    default:
        break;
    }
}
