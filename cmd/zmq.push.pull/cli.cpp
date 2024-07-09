
// push.cpp
#include <zmq.hpp>
#include <iostream>
#include <thread>
#include <chrono>

const char *addr = "tcp://*:5555";
const char *str = "xxxx";
int opt;
int count = 1;

int main(int argc, char *argv[])
{
    // 解析命令行参数
    while ((opt = getopt(argc, argv, "a:s:c:")) != -1)
    {
        switch (opt)
        {
        case 'a':
            addr = optarg;
            break;
        case 's':
            // str = optarg;
            break;
        case 'c':
            count = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Usage: %s -a <IP_ADDRESS> -c 10000\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (addr == NULL || str == NULL)
    {
        fprintf(stderr, "Usage: %s -a <IP_ADDRESS> -c 10000\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_PUSH);
    // std::string pushAddr = "tcp://";
    socket.bind(addr);

    for (int i = 0; i < count; ++i)
    {
        std::string message = "Message " + std::to_string(i);
        std::cout << "Sending: " << message << std::endl;
        zmq::message_t zmq_message(message.size());
        memcpy(zmq_message.data(), message.c_str(), message.size());
        socket.send(zmq_message, zmq::send_flags::none);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
