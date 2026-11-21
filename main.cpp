#include <iostream>
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
   
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1) {
        std::perror("Socket creation failed");
        return 1;
    }

    std::cout << "TCP socket created successfully!" << std::endl;

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
        std::perror("Bind failed");
        close(serverSocket);
        return 1;
    }
    std::cout << "Socket bound to 127.0.0.1:8080!!" << std::endl;

    close(serverSocket);
}