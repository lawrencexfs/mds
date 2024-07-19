#include <thread>
#include <unistd.h> // for close
#include "tbb/concurrent_queue.h"
#include <czmq.h>
#include "pull.h"
#include <iostream>
#include <vector>
#include <json/reader.h>
#include "pgsqlClient.hpp"
#include "order.h"

//  g++ -o pushs push.cpp -lczmq
//  g++ -o pullr pull.cpp -lczmq

Singleton &global = Singleton::getInstance();
const char *paddr = "10.0.20.3:10086";
tbb::concurrent_queue<std::string> globalTaskQ;
// std::vector<std::string> globalTask;
volatile bool run = true;

void HandleMQ(zsock_t *socket, void *arg)
{
    {
        ZMQPull pull(paddr);
        if (!pull.status())
        {
            fprintf(stderr, "pull connect %s failure \n", paddr);
            exit(EXIT_FAILURE);
        }
        while (run)
        {
            auto message = pull.recv();
            globalTaskQ.push(message);
            // std::cout << "zstr_recv ... " << message << std::endl;
        }
        run = false;
    }
    zsock_destroy(&socket);
}

int HandleMessage()
{
    // std::string connectStr = "dbname=postgres user=yugabyte hostaddr=10.0.20.9 port=5433";
    std::string connectStr = "dbname=devdb user=dev hostaddr=127.0.0.1 port=5432";
    PgsqlClient client(connectStr);
    int count = 0;
    while (run)
    {
        // if (globalTask.size() == 0)
        // {
        //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //     continue;
        // }
        std::string req;
        if (globalTaskQ.try_pop(req))
        {
            ++count;
            std::cout << count << " > globalTaskQ pop task: " << req << std::endl;
            // Json::Value jreq;
            // bool parsingSuccessful = Json::Reader().parse(req.c_str(), jreq);
            auto jreq = nlohmann::json::parse(req.c_str());
            auto iter = jreq.find("Sql");
            //  判断处理结果
            if (iter != jreq.end())
            {
                // auto sql = jreq["Sql"].asString();
                auto sql = iter.value().get<std::string>();
                // std::cout << "globalTaskQ pop sql: " << sql << std::endl;
                pqxx::result reply;
                if (client.sqlExec(sql, reply))
                {
                    // std::cout << "exec sql ok... " << std::endl;
                }
                else
                {
                    std::cout << "exec sql fail... " << std::endl;
                }
            }
            else
            {
                std::cout << "Json req parse fail..." << std::endl;
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int opt;
    // 解析命令行参数
    while ((opt = getopt(argc, argv, "a:c:")) != -1)
    {
        switch (opt)
        {
        case 'a':
            paddr = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s -a <IP_ADDRESS> \n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (paddr == NULL)
    {
        fprintf(stderr, "Usage: %s -a <IP_ADDRESS>  \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    std::thread tHandleMessage(HandleMessage);

    auto tzmq = zactor_new(HandleMQ, nullptr);

    while (!zsys_interrupted)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    run = false;

    // HandleMQ();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    tHandleMessage.join();

    // 等待线程退出
    // 销毁 actor
    zactor_destroy(&tzmq);
    // zactor_wait(&tzmq);
    std::cout << "zsys_interrupted ... exit" << std::endl;
}

// int main(void)
// {
//     // 创建一个 PULL 套接字并连接到发送者
//     zsock_t *pull = zsock_new_pull(">tcp://10.0.20.2:5556");
//     if (pull == nullptr)
//     {
//         std::cout << "err!" << std::endl;
//         return -1;
//     }
//     while (!zsys_interrupted)
//     {
//         // 接收消息
//         char *message = zstr_recv(pull);
//         std::cout << "zstr_recv ..." << std::endl;
//         if (message)
//         {
//             printf("Received: %s\n", message);
//             zstr_free(&message);
//         }
//     }

//     zsock_destroy(&pull);
//     return 0;
// }
