#include <czmq.h>

static volatile int keep_running = 1;

void int_handler(int dummy)
{
    keep_running = 0;
}

int handle(zsock_t *sock, char *str)
{

    zstr_send(sock, str);
    char *reply = zstr_recv(sock);
    printf("Received: %s\n", reply);
    zstr_free(&reply);

    zsock_destroy(&sock);
    return 0;
}

int main(int argc, char *argv[])
{
    // 设置信号处理函数
    zsys_handler_set(int_handler);
    int opt;
    char *addr = NULL;
    char *str = NULL;

    struct sockaddr_in server_addr;

    // 解析命令行参数
    while ((opt = getopt(argc, argv, "a:s:")) != -1)
    {
        switch (opt)
        {
        case 'a':
            addr = optarg;
            break;
        case 's':
            str = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s -a <IP_ADDRESS> -s hello.x1\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (addr == NULL || str == NULL)
    {
        fprintf(stderr, "IP address not provided.\n");
        fprintf(stderr, "Usage: %s -a <IP_ADDRESS>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    zsock_t *sock = zsock_new(ZMQ_REQ);
    zsock_connect(sock, addr);
    handle(sock, str);
    return 0;
}
