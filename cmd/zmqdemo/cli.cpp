#include <czmq.h>
#include <iostream>
#include <chrono>

static volatile int keep_running = 1;
char *addr = NULL;
const char *str = "{\"CheckAccount\":\"2354739169@qq.com\",\"EnableDataBaseType\":0,\"VerifyRange\":\"emailSum\",\"level\":\"user\",\"order\":\"CheckAccount\",\"receive\":\"redisService\",\"sender\":\"web\"}";

void int_handler(int dummy)
{
    keep_running = 0;
}

int handle(const char *str)
{
    zsock_t *sock = zsock_new(ZMQ_REQ);
    zsock_connect(sock, addr);

    zstr_send(sock, str);
    char *reply = zstr_recv(sock);
    // printf("Received: %s\n", reply);
    zstr_free(&reply);

    zsock_destroy(&sock);
    return 0;
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
    auto start = std::chrono::high_resolution_clock::now();
    while (--count >= 0)
    {
        handle(str);
        std::cout << ".";
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << std::endl
              << "Time taken: " << duration.count() << " milliseconds" << std::endl;

    return 0;
}
