#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

std::vector<std::thread> workers;
std::queue<std::function<void()>> tasks;
std::mutex queue_mutex;
std::condition_variable cond_var;
bool stop = false; // 控制线程退出的标记

void threadPoolTask()
{
    while (true)
    {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            cond_var.wait(lock, []
                          { return !tasks.empty() || stop; });

            if (stop && tasks.empty())
            {
                // 如果停止标志被设置且队列为空，则退出循环
                break;
            }

            task = std::move(tasks.front());
            tasks.pop();
        }

        // 执行任务
        task();

        // 如果任务执行抛出异常，这里应该进行处理
    }
}

void addTask(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        tasks.push(std::move(task));
    }
    cond_var.notify_one();
}

int main()
{
    int numThreads = 4; // 假设我们想要4个工作线程
    for (int i = 0; i < numThreads; ++i)
    {
        workers.emplace_back(threadPoolTask);
    }

    // 添加一些任务到线程池
    addTask([]()
            { std::cout << "Task 1 executed by thread\n"; });
    addTask([]()
            { std::cout << "Task 2 executed by thread\n"; });

    // 等待所有任务完成
    for (auto &th : workers)
    {
        if (th.joinable())
        {
            th.join();
        }
    }

    // 停止所有线程
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        stop = true;
    }
    cond_var.notify_all();
    std::cout << "Pool over\n";

    return 0;
}