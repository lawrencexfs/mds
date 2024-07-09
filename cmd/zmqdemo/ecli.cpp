#include <czmq.h>
#include <iostream>
#include <thread>

#include <chrono>

static volatile int keep_running = 1;
char *addr = NULL;
const char *str = "{\"CheckAccount\":\"2354739169@qq.com\",\"EnableDataBaseType\":0,\"VerifyRange\":\"emailSum\",\"level\":\"user\",\"order\":\"CheckAccount\",\"receive\":\"redisService\",\"sender\":\"web\"}";

void int_handler(int dummy)
{
    keep_running = 0;
}

int handle(zsock_t *sock, int count)
{
    while (--count >= 0)
    {
        char *reply = zstr_recv(sock);
        printf("Received: %s\n", reply);
        zstr_free(&reply);
    }

    return 0;
}

void task(int count)
{
    auto start = std::chrono::high_resolution_clock::now();
    zsock_t *sock = zsock_new(ZMQ_REQ);
    zsock_connect(sock, addr);
    zsock_set_rcvhwm(sock, count);
    zsock_set_sndhwm(sock, count);
    std::thread run(handle, sock, count);
    while (--count >= 0)
    {
        zstr_send(sock, str);
        // handle(sock, str);
        // std::cout << ".";
    }
    run.join();
    zsock_destroy(&sock);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << std::endl
              << "Time taken: " << duration.count() << " milliseconds" << std::endl;
}

int main(int argc, char *argv[])
{
    // 设置信号处理函数
    zsys_handler_set(int_handler);
    int opt;
    // char *str = NULL;
    int count = 1;

    struct sockaddr_in server_addr;

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
    std::thread run1(task, count / 3);
    std::thread run2(task, count / 3);
    std::thread run3(task, count / 3);

    run1.join();
    run2.join();
    run3.join();

    return 0;
}
