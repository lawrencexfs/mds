#include <czmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

// sudo systemctl status nginx
// sudo systemctl reload nginx
// echo 'export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH' >> /etc/bash.bashrc
// echo 'export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH' >>  /etc/bash.bashrc
// source   /etc/bash.bashrc

// g++ -o cli cli.cpp $(pkg-config --cflags --libs libczmq)
// g++ - o svr svr.cpp $(pkg - config-- cflags-- libs libczmq)

// ./cli -a tcp://127.0.0.1:8090 -s hello.ww1  && ./cli -a tcp://127.0.0.1:8090 -s hello.ww2 && ./cli -a tcp://127.0.0.1:8090 -s hello.ww3

//  ---------------------------------------------------------------------
//  消息处理
//
//  程序开始运行时调用s_catch_signals()函数；
//  在循环中判断s_interrupted是否为1，是则跳出循环；
//  很适用于zmq_poll()。

static int s_interrupted = 0;
static void s_signal_handler(int signal_value)
{
    s_interrupted = 1;
}

static void s_catch_signals(void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

std::string get_current_time()
{
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 转换为 time_t
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    // 获取毫秒部分
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    // 转换为 tm 结构
    std::tm now_tm = *std::localtime(&now_time_t);

    // 使用 stringstream 来格式化时间字符串
    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y:%m:%d %H:%M:%S");                // 格式化为 "2022:12:21 10:56:13"
    oss << ':' << std::setw(3) << std::setfill('0') << now_ms.count(); // 添加毫秒部分

    return oss.str();
}

int handle(zsock_t *sock)
{
    while (1)
    {
        char *request = zstr_recv(sock);
        printf("Received: %s\n", request);
        zstr_free(&request);

        // Send reply back to client
        zstr_send(sock, get_current_time().c_str());
        if (s_interrupted)
        {
            printf("W: 收到中断消息，程序中止...\n");
            break;
        }
        // //  阻塞式的读取会在收到信号时停止
        // zmq_msg_t message;
        // zmq_msg_init(&message);
        // zmq_recv(socket, &message, 0);
        // if (s_interrupted)
        // {
        //     printf("W: 收到中断消息，程序中止...\n");
        //     break;
        // }
        // printf("Received: %s\n", message);
        // // zstr_free(&request);

        // // Send reply back to client
        // zstr_send(sock, "World");
        // zmq_send(socket, message, 0)
    }

    zsock_destroy(&sock);
    return 0;
}

int main(int argc, char *argv[])
{
    // 设置信号处理函数
    // zsys_handler_set();
    int opt;
    char *addr = NULL;
    struct sockaddr_in server_addr;

    // 解析命令行参数
    while ((opt = getopt(argc, argv, "a:")) != -1)
    {
        switch (opt)
        {
        case 'a':
            addr = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s -a <IP_ADDRESS>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (addr == NULL)
    {
        fprintf(stderr, "IP address not provided.\n");
        fprintf(stderr, "Usage: %s -a <IP_ADDRESS>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 其他程序逻辑
    // void *context = zmq_init(1);
    // void *socket = zmq_socket(context, ZMQ_REP);
    zsock_t *sock = zsock_new(ZMQ_REP);

    // zsock_bind(sock, "tcp://*:5555");
    zsock_bind(sock, addr);
    s_catch_signals();
    handle(sock);

    zsock_destroy(&sock);
    return 0;
}
