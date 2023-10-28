#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <termios.h>
#include <thread>

const char *SERVER_IP = "127.0.0.1";
const int TCP_PORT = 12345;
const int UDP_PORT = 12346; 
const int BUFFER_SIZE = 1024;

void listen_to_server(int sockfd) {
    char buffer[BUFFER_SIZE];
    while (true) {
        struct sockaddr_in serverAddr;
        socklen_t serverAddrLen = sizeof(serverAddr);

        ssize_t recvLen = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&serverAddr, &serverAddrLen);
        if (recvLen == -1) {
            perror("recvfrom");
            continue; // You might decide to handle this differently.
        }

        buffer[recvLen] = '\0';
        std::cout << "Received: " << buffer << std::endl;
    }
}

char getch() {
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0) perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("tcsetattr ~ICANON");
    return buf;
}

void start_tcp_client() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("connect");
        close(sockfd);
        exit(1);
    }

    // TCP related stuff.
    // For simplicity, we will assume the handshake is done without any actual data exchange.
    close(sockfd);
}

void start_udp_client() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDP_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    std::thread listener_thread(listen_to_server, sockfd);

    while (true) {
        char input = getch();
        if (input == 'b') break;  // Let's use 'q' to quit

        std::string message = "PM:";  // PM for PlayerMoved
        message += input;
        ssize_t sendLen = sendto(sockfd, message.c_str(), message.length(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        if (sendLen == -1) {
            perror("sendto");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

    }

    listener_thread.join();
    close(sockfd);
}

int main() {
    start_tcp_client();
    start_udp_client();

    return 0;
}
