#include <iostream>
#include <string>
#include <thread>
#include <unistd.h> // for close
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
// g++ svr.cpp -o svrp -pthread

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

void handle_client(int client_socket)
{
    char buffer[1024];
    while (true)
    {
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0)
        {
            std::cerr << "Error receiving data or client disconnected." << std::endl;
            close(client_socket);
            return;
        }

        buffer[bytes_received] = '\0';
        std::cout << "Message received: " << buffer;
        std::cout << ".len: " << bytes_received << std::endl;
        std::cout << "Message from : " << std::string() << std::endl;
        std::string reply = std::string("ni hao <. to {} ") + std::string(buffer);
        get_remote_address(client_socket);

        send(client_socket, reply.c_str(), reply.length(), 0);
    }
}

int main()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << "Error setting socket options: " << errno << std::endl;
        close(server_socket);
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8081);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cerr << "Error binding socket." << std::endl;
        return 1;
    }

    if (listen(server_socket, 5) == -1)
    {
        std::cerr << "Error listening on socket." << std::endl;
        return 1;
    }

    std::cout << "Server listening on port 8081" << std::endl;

    while (true)
    {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr *)&client_addr, &client_len);
        if (client_socket == -1)
        {
            std::cerr << "Error accepting client connection." << std::endl;
            continue;
        }

        std::thread(handle_client, client_socket).detach();
    }

    close(server_socket);
    return 0;
}
