/*
** EPITECH PROJECT, 2023
** B-CPP-500-PAR-5-1-rtype-sofiane.bassaler
** File description:
** server_tcp
*/

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <thread>

const int TCP_PORT = 12345;
const int UDP_PORT = 12346;
const int BUFFER_SIZE = 1024;
std::vector<std::pair<struct sockaddr_in, int>> clients;
int clientIdCounter = 0;

void add_client(struct sockaddr_in new_client)
{
    for (const auto &pair : clients)
    {
        if (pair.first.sin_addr.s_addr == new_client.sin_addr.s_addr && pair.first.sin_port == new_client.sin_port)
        {
            return; // Client already exists
        }
    }
    clients.push_back({new_client, clientIdCounter++});
}

void broadcast(int sockfd, const char *message, ssize_t len, struct sockaddr_in *exclude_addr = nullptr)
{
    for (const auto &pair : clients)
    {
        const struct sockaddr_in &client = pair.first;
        if (exclude_addr && client.sin_addr.s_addr == exclude_addr->sin_addr.s_addr && client.sin_port == exclude_addr->sin_port)
        {
            continue; // Skip sending back to the source client
        }
        sendto(sockfd, message, len, 0, (struct sockaddr *)&client, sizeof(client));
    }
}

void handle_tcp_client(int client_sock)
{
    char buffer[BUFFER_SIZE];

    ssize_t bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0)
    {
        perror("recv");
        close(client_sock);
        return;
    }

    buffer[bytes_received] = '\0';
    std::cout << "Received (TCP): " << buffer << std::endl;

    close(client_sock); // Close the client's socket when done.
}

int start_tcp_server()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("bind");
        close(sockfd);
        return -1;
    }

    listen(sockfd, 5);

    while (true)
    {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        int client_sock = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (client_sock == -1)
        {
            perror("accept");
            continue; // Handle the error and continue accepting other clients.
        }

        std::thread client_thread(handle_tcp_client, client_sock);
        client_thread.detach(); // Detach so the main thread doesn't need to join the client threads.
    }

    close(sockfd);
    return 0;
}

void start_udp_server()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDP_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    while (true)
    {
        char buffer[BUFFER_SIZE];
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        ssize_t recvLen = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (recvLen == -1)
        {
            perror("recvfrom");
            break;
        }

        buffer[recvLen] = '\0';
        add_client(clientAddr);

        int client_id = -1;
        for (const auto &pair : clients)
        {
            if (pair.first.sin_addr.s_addr == clientAddr.sin_addr.s_addr && pair.first.sin_port == clientAddr.sin_port)
            {
                client_id = pair.second;
                break;
            }
        }

        if (client_id != -1)
        {
            std::cout << "Received from client_" << client_id << ": " << buffer << std::endl;
        }
        else
        {
            std::cout << "Received from unknown client: " << buffer << std::endl;
        }

        broadcast(sockfd, buffer, recvLen, &clientAddr); // Send to all other clients
    }
    close(sockfd);
}

int main()
{
    std::thread tcp_thread(start_tcp_server);
    std::thread udp_thread(start_udp_server);

    tcp_thread.join();
    udp_thread.join();

    return 0;
}
