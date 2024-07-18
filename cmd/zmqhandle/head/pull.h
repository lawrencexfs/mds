#ifndef _PULL_H
#define _PULL_H

#include <string>
#include <czmq.h>

class ZMQPull
{
private:
    // std::unique_ptr<zsock_t> pull;
    zsock_t *pull;
    char *message;

public:
    ~ZMQPull();
    ZMQPull(const std::string &addr);

public:
    bool status();
    std::string recv();
    void free();
};

#endif