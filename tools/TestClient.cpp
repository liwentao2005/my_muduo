#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

const int PORT = 8088;
const char* SERVER_IP = "127.0.0.1";
const int BUFFER_SIZE = 1024;

int main() {
    // Create socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server address
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(client_socket);
        return 1;
    }

    // Connect to server
    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        return 1;
    }

    std::cout << "Connected to server at " << SERVER_IP << ":" << PORT << std::endl;

    // Communication loop
    char buffer[BUFFER_SIZE];
    // while (true) {
    for (int i = 0; i < 1; ++i)
    {
        std::cout << "Enter message: ";
        std::string input("test.");
        // std::getline(std::cin, input);

        if (input == "exit") break;

        // Send message
        ssize_t bytes_sent = send(client_socket, input.c_str(), input.length(), 0);
        if (bytes_sent < 0) {
            perror("Send failed");
            break;
        }

        // Receive response
        ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0)
                std::cout << "Server disconnected" << std::endl;
            else
                perror("Receive failed");
            break;
        }

        buffer[bytes_received] = '\0';
        std::cout << "Server response: " << buffer << std::endl;
    }

    close(client_socket);
    return 0;
}
