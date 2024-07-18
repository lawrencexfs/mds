#include <czmq.h>
#include <thread>
#include <iostream>

// ./push -a tcp://10.0.20.2:5555 -c 100
// ./pull -a tcp://10.0.20.2:5555
// czmq 库 只能适用 多对一，多个 push 发送, 单个 pull 接收， pull 端绑定地址
// zmq 库 只能适用 一对多，单个 push 发送， 多个 pull 接收， push 端绑定地址

static volatile int keep_running = 1;

void int_handler(int dummy)
{
    keep_running = 0;
}

void handle(zsock_t *socket, void *arg);
int main(void)
{
    // 设置信号处理函数
    zsys_handler_set(int_handler);

    // auto r1 = zactor_new(handle, nullptr);
    // auto r2 = zactor_new(handle, nullptr);
    // auto r3 = zactor_new(handle, nullptr);

    handle(nullptr, nullptr);
    // while (keep_running)
    // {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(1));
    // }
    // zstr_send(r1, "1");

    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void handle(zsock_t *socket, void *arg)
{
    // 设置信号处理函数
    zsys_handler_set(int_handler);

    // zsock_signal(socket, 0);
    // 创建一个 PUSH 套接字并绑定到端口 5555
    zsock_t *push = zsock_new_push("@tcp://10.0.20.2:5556");
    assert(push);

    int count = 1000000;

    while (!zsys_interrupted && count > 0 && keep_running)
    {
        // // 发送消息
        // auto res = zstr_recv_nowait(socket);
        // if (res != nullptr)
        // {
        //     std::cout << "hello" << std::endl;
        //     break;
        // }

        const char *message = "Hello, World";
        printf("push: %s\n", message);
        zstr_send(push, message);
        // zclock_sleep(1); // 每秒发送一次消息
        count--;
    }

    zsock_destroy(&push);
    // return 0;
}
