#include <iostream>
#include "pull.h"
#include <czmq.h>

ZMQPull::ZMQPull(const std::string &addr)
{
    std::string inaddr = std::string(">tcp://") + addr;
    pull = zsock_new_pull(inaddr.c_str());
    message = nullptr;
}

ZMQPull::~ZMQPull()
{
    zsock_destroy(&pull);
}

bool ZMQPull::status()
{
    return pull != nullptr;
}

std::string ZMQPull::recv()
{
    // 接收消息
    message = zstr_recv(pull);
    std::string tp = (message != nullptr) ? std::string(message) : std::string();
    // std::cout << "zstr_recv ..." << std::endl;
    free();
    return tp;
}

void ZMQPull::free()
{
    if (message)
    {
        zstr_free(&message);
        message = nullptr;
    }
}
