#include <iostream>
#include <string>
#include <thread>
#include <unistd.h> // for close
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
// g++ cli.cpp -o clip -pthread

void get_remote_address(int sockfd)
{
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    if (getpeername(sockfd, (sockaddr *)&addr, &addr_len) == -1)
    {
        std::cerr << "Error getting peer name." << std::endl;
        return;
    }

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip_str, sizeof(ip_str));

    std::cout << ".Remote IP address: " << ip_str << std::endl;
    std::cout << ".Remote port: " << ntohs(addr.sin_port) << std::endl;
}

int main()
{
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8081);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cerr << "Error connecting to server." << std::endl;
        return 1;
    }

    std::cout << "Connected to server" << std::endl;

    std::string message;
    char buffer[1024];

    while (true)
    {
        std::cout << "Enter message: ";
        std::getline(std::cin, message);

        if (send(client_socket, message.c_str(), message.size(), 0) == -1)
        {
            std::cerr << "Error sending message." << std::endl;
            break;
        }

        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0)
        {
            std::cerr << "Error receiving response or server disconnected." << std::endl;
            break;
        }

        buffer[bytes_received] = '\0';
        std::cout << "Reply from server: " << buffer << std::endl;
        std::cout << "Message from : " << std::string() << std::endl;
        get_remote_address(client_socket);
    }

    close(client_socket);
    return 0;
}
