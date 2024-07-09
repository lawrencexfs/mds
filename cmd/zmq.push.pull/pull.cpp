#include <czmq.h>
#include <iostream>
//  g++ -o pushs push.cpp -lczmq
//  g++ -o pullr pull.cpp -lczmq

int main(void)
{
    // 创建一个 PULL 套接字并连接到发送者
    zsock_t *pull = zsock_new_pull("tcp://10.0.20.2:5551");
    if (pull == nullptr)
    {
        std::cout << "err!" << std::endl;
        return -1;
    }
    while (!zsys_interrupted)
    {
        // 接收消息
        char *message = zstr_recv(pull);
        std::cout << "zstr_recv ..." << std::endl;
        if (message)
        {
            printf("Received: %s\n", message);
            zstr_free(&message);
        }
    }

    zsock_destroy(&pull);
    return 0;
}
