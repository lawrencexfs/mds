// pull.cpp
#include <zmq.hpp>
#include <iostream>

// g++ cli.cpp -o push -lzmq
// g++ svr.cpp -o pull -lzmq
// ./push -a tcp://10.0.20.2:5555 -c 100

const char *addr = "tcp://*:5555";
const char *str = "xxxx";
int opt;
int count = 1;

int main(int argc, char *argv[])
{
    // 解析命令行参数
    while ((opt = getopt(argc, argv, "a:c:")) != -1)
    {
        switch (opt)
        {
        case 'a':
            addr = optarg;
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
    zmq::socket_t socket(context, ZMQ_PULL);
    socket.connect(addr); // 替换 <push_host> 为发送主机的 IP 地址

    while (true)
    {
        zmq::message_t zmq_message;
        zmq::recv_result_t result = socket.recv(zmq_message, zmq::recv_flags::none);
        if (result.has_value())
        {
            std::string message(static_cast<char *>(zmq_message.data()), zmq_message.size());
            std::cout << "Received: " << message << std::endl;
        }
    }

    return 0;
}
