#include <iostream>
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>

int main() {
   
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1) {
        std::perror("Socket creation failed");
        return 1;
    }

    std::cout << "TCP socket created successfully!" << std::endl;

    close(serverSocket);
}