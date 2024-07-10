#include <iostream>
#include <thread>
#include <mutex>
// g++ -o demo demo.cpp  -L/usr/lib -lssl -lcrypto -luuid -L/usr/local/lib -ggdb
// g++ -o demo demo.cpp  -L/usr/lib -lssl -lcrypto -luuid -L/usr/local/lib -ggdb -std=c++11 -pthread

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

// 线程池
class ThreadPool
{
public:
    ThreadPool(size_t threads);
    ~ThreadPool();

    template <class F, class... Args>
    auto enqueue(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

ThreadPool::ThreadPool(size_t threads)
    : stop(false)
{
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back(
            [this]
            {
                for (;;)
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                                             [this]
                                             { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                }
            });
}

template <class F, class... Args>
auto ThreadPool::enqueue(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task]()
                      { (*task)(); });
    }
    condition.notify_one();
    return res;
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers)
        worker.join();
}

int main()
{
    ThreadPool pool(4);

    std::vector<std::future<void>> results;

    for (int i = 0; i < 8; ++i)
    {
        results.emplace_back(
            pool.enqueue([i]
                         {
                std::cout << "Thread " << i << " is working\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << "Thread " << i << " has finished\n"; }));
    }

    for (auto &&result : results)
    {
        result.get();
        // std::cout << result << std::endl;
    }
    // result.get();

    return 0;
}

// // 条件变量
// std::mutex mtx;
// std::condition_variable cv;
// bool ready = false;

// void print_id(int id)
// {
//     std::unique_lock<std::mutex> lock(mtx);
//     cv.wait(lock, []
//             { return ready; });

//     std::cout << "Thread " << id << std::endl;
// }

// void set_ready()
// {
//     std::this_thread::sleep_for(std::chrono::seconds(1));
//     {
//         std::lock_guard<std::mutex> lock(mtx);
//         ready = true;
//     }
//     cv.notify_all();
// }

// int main()
// {
//     std::thread threads[5];
//     for (int i = 0; i < 5; ++i)
//         threads[i] = std::thread(print_id, i);

//     std::thread t(set_ready);

//     for (auto &th : threads)
//         th.join();
//     t.join();

//     return 0;
// }

// 互斥量同步
// std::mutex mtx;

// void print_message(const std::string &message)
// {
//     //  std::mutex 和 std::lock_guard 来保护 print_message 函数中的共享资源（即 std::cout）
//     std::lock_guard<std::mutex> lock(mtx);
//     std::cout << message << std::endl;
// }

// int main()
// {
//     std::cout << "Demo begin" << std::endl;
//     std::thread t1(print_message, "Hello from thread 1");
//     std::thread t2(print_message, "Hello from thread 2");

//     t1.join();
//     t2.join();

//     return 0;
// }
