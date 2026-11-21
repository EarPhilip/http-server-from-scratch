#include <iostream>
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <csignal>

int main()
{
    std::signal(SIGPIPE, SIG_IGN);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1)
    {
        std::perror("Socket creation failed");
        return 1;
    }

    std::cout << "TCP socket created successfully!" << std::endl;

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int reuseAddress = 1;

    if (setsockopt(
            serverSocket,
            SOL_SOCKET,
            SO_REUSEADDR,
            &reuseAddress,
            sizeof(reuseAddress)) == -1)
    {
        std::perror("setsockopt failed");
        close(serverSocket);
        return 1;
    }

    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == -1)
    {
        std::perror("Bind failed");
        close(serverSocket);
        return 1;
    }
    std::cout << "Socket bound to 127.0.0.1:8080!!" << std::endl;

    if (listen(serverSocket, 10) == -1)
    {
        std::perror("Listen failed");
        close(serverSocket);
        return 1;
    }
    std::cout << "Server listening on port 8080..." << std::endl;
    while (true)
    {
        int clientSocket = accept(serverSocket, nullptr, nullptr);

        if (clientSocket == -1)
        {
            std::perror("Accept failed");
            close(serverSocket);
            return 1;
        }
        std::cout << "Client connected!" << std::endl;

        std::string request;
        char buffer[4096];

        while (request.find("\r\n\r\n") == std::string::npos)
        {
            ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

            if (bytesRead <= 0)
            {
                if (bytesRead < 0)
                {
                    std::perror("Receive failed");
                }

                close(clientSocket);
                close(serverSocket);
                return 1;
            }

            request.append(buffer, static_cast<std::size_t>(bytesRead));

            // Limit how much request data we collect.
            if (request.size() > 16384)
            {
                std::cerr << "Request headers too large." << std::endl;
                close(clientSocket);
                close(serverSocket);
                return 1;
            }
        }

        std::cout << "Received:\n"
                  << request << std::endl;

        std::string body = "Hello, Philip!";

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " +
            std::to_string(body.size()) + "\r\n"
                                          "Connection: close\r\n"
                                          "\r\n" +
            body;

        // send() can write fewer bytes than requested.
        std::size_t totalSent = 0;

        while (totalSent < response.size())
        {
            ssize_t bytesSent = send(
                clientSocket,
                response.data() + totalSent,
                response.size() - totalSent,
                0);

            if (bytesSent <= 0)
            {
                std::cerr << "Could not finish sending the response." << std::endl;
                close(clientSocket);
                close(serverSocket);
                return 1;
            }

            totalSent += static_cast<std::size_t>(bytesSent);
        }

        std::cout << "Response sent!" << std::endl;

        close(clientSocket);
    }
    close(serverSocket);
    return 0;
}