#include <czmq.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/task_group.h>
#include <tbb/parallel_invoke.h>
#include <tbb/task_arena.h>
#include <coroutine>

static volatile int keep_running = 1;
char *addr = NULL;
const char *str = "{\"CheckAccount\":\"2354739169@qq.com\",\"EnableDataBaseType\":0,\"VerifyRange\":\"emailSum\",\"level\":\"user\",\"order\":\"CheckAccount\",\"receive\":\"redisService\",\"sender\":\"web\"}";

class mySend
{
private:
    const std::string &_msg;
    zsock_t *_sock;
    std::string result;

public:
    inline mySend(zsock_t *sock, const std::string &msg)
        : _msg(msg),
          _sock(sock)
    {
    }

    /**
     * @description: Used to determine whether a coroutine is suspended. Currently, all coroutines are always suspended.
     * @return {*}
     */
    inline bool await_ready() const noexcept { return false; }
    /**
     * @description: Called when a coroutine is suspended
     * @param {coroutine_handle<>} h Coroutine handle, automatically passed when the coroutine is suspended
     * @return {*}
     */
    inline void await_suspend(std::coroutine_handle<> h)
    {
        tg.run([h, this]()
               { char *reply = zstr_recv(this->_sock);
    printf("Received: %s\n", reply);
    zstr_free(&reply);h.resume(); });
    }
    /**
     * @description: The return value when suspension ends.
     * @return {*}
     */
    inline std::string await_resume() const noexcept
    {
        return result;
    }
    static tbb::task_group tg;
};
// Specify the return value of the function that calls the coroutine
struct Task
{
    struct promise_type
    {
        Task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
};

tbb::task_group mySend::tg;

void int_handler(int dummy)
{
    keep_running = 0;
}

Task handle(zsock_t *sock)
{
    std::string strppppp;
    co_await mySend(sock, strppppp);
    co_return;
}

void task()
{
    // auto start = std::chrono::high_resolution_clock::now();
    // zsock_t *sock = zsock_new_req("tcp://10.0.20.189:10010");

    // zstr_send(sock, "123213213");
    // char *reply = zstr_recv(sock);
    // printf("Received: %s\n", reply);
    // zstr_free(&reply);
    // zsock_destroy(&sock);
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // std::cout << std::endl
    //           << "Time taken: " << duration.count() << " milliseconds" << std::endl;
}

//  Asynchronous client-to-server (DEALER to ROUTER)
//
//  While this example runs in a single process, that is to make
//  it easier to start and stop the example. Each task conceptually
//  acts as a separate process.

#include "czmq.h"

//  This is our client task
//  It connects to the server, and then sends a request once per second
//  It collects responses as they arrive, and it prints them out. We will
//  run several client tasks in parallel, each with a different random ID.

static void
client_task(zsock_t *pipe, void *args)
{
    zsock_signal(pipe, 0);
    int i = 0;
    zsock_t *client = zsock_new(ZMQ_DEALER);

    //  Set random identity to make tracing easier (must be done before zsock_connect)
    char identity[10];
    sprintf(identity, "%04X-%04X", randof(0x10000), randof(0x10000));
    zsock_set_identity(client, identity);
    zsock_connect(client, "tcp://localhost:5570");

    zpoller_t *poller = zpoller_new(pipe, client, NULL);
    zpoller_set_nonstop(poller, true);

    zsock_set_rcvhwm(client, 100000);
    zsock_set_sndhwm(client, 100000);
    bool signaled = false;
    int request_nbr = 0;
    auto start = std::chrono::high_resolution_clock::now();

    std::thread run2([client, &request_nbr]()
                     {
                         int i = 0;
                         while (i < 1000)
                         {
                             ++i;
                             auto content = zstr_recv(client);
                             if (content)
                             {
                                 std::cout << "count is " << i << std::endl;
                                 
                                 zstr_free(&content);
                             }
                             

                         } });
    std::thread run([client, &request_nbr]()
                    {
                        int i = 0;
                        while (i < 1000)
                    {
                        zstr_send(client, "request");
                        ++i;
                        // if (!(i % 1000))
                        // {
                        //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        // }
                                        } });
    run.detach();

    run2.join();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << std::endl
              << "Time taken: " << duration.count() << " milliseconds" << std::endl;
    std::cout << "end" << std::endl;
    // while (i < 100000)
    // {
    //     //  Tick once per second, pulling in arriving messages
    //     for (int k = 0; k < 100; k++)
    //     {
    //         zstr_sendf(client, "request%d", i);
    //         ++i;
    //     }
    //     while (!signaled)
    //     {
    //         zsock_t *ready = static_cast<zsock_t *>(zpoller_wait(poller, 1000 * ZMQ_POLL_MSEC));
    //         if (ready == NULL)
    //         {
    //             signaled = true;
    //             break;
    //         }
    //         else if (ready == pipe)
    //         {
    //             signaled = true;
    //             break;
    //         }
    //         else
    //             assert(ready == client);

    //         auto content = zstr_recv(client);
    //         // std::cout << "count is " << i << std::endl;
    //         std::cout << content << std::endl;
    //         zstr_free(&content);
    //     }

    //     // zmsg_t *msg = zmsg_recv(client);
    //     // zframe_print(zmsg_last(msg), identity);
    //     // zmsg_destroy(&msg);
    // }
    zpoller_destroy(&poller);
    zsock_destroy(&client);
}

//  .split server task
//  This is our server task.
//  It uses the multithreaded server model to deal requests out to a pool
//  of workers and route replies back to clients. One worker can handle
//  one request at a time but one client can talk to multiple workers at
//  once.

static void server_worker(zsock_t *pipe, void *args);

static void server_task(zsock_t *pipe, void *args)
{
    zsock_signal(pipe, 0);

    //  Launch pool of worker threads, precise number is not critical
    enum
    {
        NBR_THREADS = 5
    };
    zactor_t *threads[NBR_THREADS];
    int thread_nbr;
    for (thread_nbr = 0; thread_nbr < 2; thread_nbr++)
        threads[thread_nbr] = zactor_new(server_worker, NULL);

    //  Connect backend to frontend via a zproxy
    zactor_t *proxy = zactor_new(zproxy, NULL);
    zstr_sendx(proxy, "FRONTEND", "ROUTER", "tcp://*:5570", NULL);
    zsock_set_rcvhwm(proxy, 100000);
    zsock_set_sndhwm(proxy, 100000);
    zsock_wait(proxy);

    zstr_sendx(proxy, "BACKEND", "DEALER", "inproc://backend", NULL);
    zsock_set_rcvhwm(proxy, 100000);
    zsock_set_sndhwm(proxy, 100000);
    zsock_wait(proxy);

    // Wait for shutdown signal
    zsock_wait(pipe);
    zactor_destroy(&proxy);

    for (thread_nbr = 0; thread_nbr < 16; thread_nbr++)
        zactor_destroy(&threads[thread_nbr]);
}

//  .split worker task
//  Each worker task works on one request at a time and sends a random number
//  of replies back, with random delays between replies:

static void
server_worker(zsock_t *pipe, void *args)
{
    zsock_signal(pipe, 0);

    zsock_t *worker = zsock_new_dealer("inproc://backend");
    zsock_set_rcvhwm(worker, 100000);
    zsock_set_sndhwm(worker, 100000);
    zpoller_t *poller = zpoller_new(pipe, worker, NULL);
    zpoller_set_nonstop(poller, true);

    while (true)
    {
        // zsock_t *ready = static_cast<zsock_t *>(zpoller_wait(poller, -1));
        // if (ready == NULL)
        //     continue;
        // else if (ready == pipe)
        //     break;
        // else
        //     assert(ready == worker);

        //  The DEALER socket gives us the reply envelope and message

        zmsg_t *msg = zmsg_recv(worker);
        zmsg_send(&msg, worker);
        zmsg_destroy(&msg);
        // zframe_send(&identity, worker, ZFRAME_REUSE | ZFRAME_MORE | ZFRAME_DONTWAIT);
        // zframe_send(&content, worker, ZFRAME_REUSE | ZFRAME_DONTWAIT);
        //  Send 0..4 replies back
        // int reply, replies = 100;
        // for (reply = 0; reply < replies; reply++)
        // {
        //     //  Sleep for some fraction of a second
        //     // zclock_sleep(randof(1000) + 1);
        //     zframe_send(&identity, worker, ZFRAME_REUSE | ZFRAME_MORE | ZFRAME_DONTWAIT);
        //     zframe_send(&content, worker, ZFRAME_REUSE | ZFRAME_DONTWAIT);
        // }
        // zframe_destroy(&identity);
        // zframe_destroy(&content);
    }

    zpoller_destroy(&poller);
    zsock_destroy(&worker);
}

//  The main thread simply starts several clients and a server, and then
//  waits for the server to finish.

// int main(void)
// {
//     auto start = std::chrono::high_resolution_clock::now();
//     zactor_t *server = zactor_new(server_task, NULL);

//     zactor_t *client1 = zactor_new(client_task, NULL);
//     // zactor_t *client2 = zactor_new(client_task, NULL);
//     // zactor_t *client3 = zactor_new(client_task, NULL);

//     while (true)
//     {
//     }

//     auto end = std::chrono::high_resolution_clock::now();
//     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//     std::cout << std::endl
//               << "Time taken: " << duration.count() << " milliseconds" << std::endl;
//     zactor_destroy(&server);
//     zactor_destroy(&client1);
//     // zactor_destroy(&client2);
//     // zactor_destroy(&client3);
//     return 0;
// }

#include <czmq.h>

// 任务发生器
void task_vent(zsock_t *pipe, void *args)
{
    // 创建PUSH套接字
    auto sender = zsock_new_push("tcp://10.0.20.3:10010");
    // 发送100个任务
    for (int task_nbr = 0; task_nbr < 100000; task_nbr++)
    {
        int workload = task_nbr;
        auto i = zstr_sendf(sender, "%d", workload);
    }
}

void task_server2(zsock_t *pipe, void *args)
{
    // 创建PULL套接字
    auto receiver = zsock_new_pull("tcp://10.0.20.2:10011");
    for (int task_nbr = 0; task_nbr < 100000; task_nbr++)
    {
        int workload = task_nbr;
        char *string = zstr_recv(receiver);
        std::cout << string << std::endl;
        delete string;
    }
}

void task_server(zsock_t *pipe, void *args)
{
    // 创建PUSH套接字
    auto sender = zsock_new_pair("tcp://10.0.20.3:10011");
    // 发送100个任务
    for (int task_nbr = 0; task_nbr < 100000; task_nbr++)
    {
        int workload = task_nbr;
        auto i = zstr_sendf(sender, "%d", workload);
    }
}

// 工作节点
void task_work(zsock_t *pipe, void *args)
{
    // 创建PULL套接字
    auto receiver = zsock_new_pull("tcp://localhost:5557");

    // 创建PUSH套接字

    auto sender = zsock_new_push("tcp://localhost:5558");
    // 处理任务
    while (true)
    {
        char *string = zstr_recv(receiver);
        zstr_send(sender, string);
        int workload = atoi(string);
        free(string);

        // 假设工作负载是工作时间
        // zclock_sleep(workload);

        // 发送结果
        // zstr_send(sender, "123123");
    }
}

// 结果收集器
void task_sink(zsock_t *pipe, void *args)
{
    // 创建PULL套接字
    auto receiver = zsock_new_pull("tcp://*:5558");

    // 收集结果
    for (int task_nbr = 0; task_nbr < 100000; task_nbr++)
    {
        char *string = zstr_recv(receiver);
        std::cout << string << std::endl;
        free(string);
    }
}

int main(int argc, char *argv[])
{
    // // 设置信号处理函数
    zsys_handler_set(int_handler);
    // int opt;
    // // char *str = NULL;
    // int count = 1;

    // struct sockaddr_in server_addr;

    // // 解析命令行参数
    // while ((opt = getopt(argc, argv, "a:s:c:")) != -1)
    // {
    //     switch (opt)
    //     {
    //     case 'a':
    //         addr = optarg;
    //         break;
    //     case 's':
    //         // str = optarg;
    //         break;
    //     case 'c':
    //         count = atoi(optarg);
    //         break;
    //     default:
    //         fprintf(stderr, "Usage: %s -a <IP_ADDRESS> -c 10000\n", argv[0]);
    //         exit(EXIT_FAILURE);
    //     }
    // }
    // if (addr == NULL || str == NULL)
    // {
    //     fprintf(stderr, "Usage: %s -a <IP_ADDRESS> -c 10000\n", argv[0]);
    //     exit(EXIT_FAILURE);
    // }
    // int i = 0;
    // auto start = std::chrono::high_resolution_clock::now();
    // std::vector<zsock_t *> sockAeey;
    // sockAeey.reserve(100000);
    // zsys_set_max_sockets(100000);
    // while (i < 100000)
    // {
    //     sockAeey.push_back(zsock_new_req("tcp://10.0.20.3:10010"));
    //     zstr_send(sockAeey[i], "123213213");
    //     mySend::tg.run([i, &sockAeey]()
    //                    {
    //                     char *reply = zstr_recv(sockAeey[i]);
    //                     printf("Received: %s\n", reply);
    //                     zstr_free(&reply);
    //                     zsock_destroy(&sockAeey[i]); });
    //     ++i;
    // }
    // mySend::tg.wait();
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // std::cout << std::endl
    //           << "Time taken: " << duration.count() << " milliseconds" << std::endl;
    // return 0;
    auto start = std::chrono::high_resolution_clock::now();
    std::thread run(task_server2, nullptr, nullptr);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    // std::thread run1(task_sink, nullptr, nullptr);
    // std::this_thread::sleep_for(std::chrono::milliseconds(20));

    std::thread run2(task_vent, nullptr, nullptr);

    // run.detach();
    // run1.join();
    run2.join();
    run.join();
    std::cout << "1" << std::endl;
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << std::endl
              << "Time taken: " << duration.count() << " milliseconds" << std::endl;
}
