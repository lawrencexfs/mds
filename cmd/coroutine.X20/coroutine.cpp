#include <coroutine>
#include <iostream>
#include <future>
#include <thread>
#include <chrono>

// g++ coroutine.cpp -o demo  -std=c++20

// struct Timer
// {
//     struct promise_type
//     {
//         Timer get_return_object() { return Timer{std::coroutine_handle<promise_type>::from_promise(*this)}; }
//         std::suspend_always initial_suspend() { return {}; }
//         std::suspend_always final_suspend() noexcept { return {}; }
//         void unhandled_exception() { std::terminate(); }
//         void return_void() {}
//     };

//     std::coroutine_handle<promise_type> handle;

//     Timer(std::coroutine_handle<promise_type> h) : handle(h) {}
//     ~Timer()
//     {
//         if (handle)
//             handle.destroy();
//     }
// };

// Timer delay(int ms)
// {
//     std::this_thread::sleep_for(std::chrono::milliseconds(ms));
//     co_return;
// }

// int main()
// {
//     auto t = delay(1000);
//     t.handle.resume();
//     std::cout << "Hello after delay!" << std::endl;
//     return 0;
// }

// OKOKOKOKOKOKOK
// OKOKOKOKOKOKOK
// OKOKOKOKOKOKOK

struct Sleeper
{
    int ms;
    Sleeper(int ms) : ms(ms) {}

    bool await_ready()
    {
        std::cout << "await_ready..." << std::endl;
        return false;
    }
    void await_suspend(std::coroutine_handle<> h)
    {
        std::cout << "await_suspend..." << std::endl;
        std::thread([this, h]()
                    {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            std::cout << "sleep_for over..." << std::endl;
            h.resume(); })
            .detach();
    }
    void await_resume()
    {
        std::cout << "await_resume..." << std::endl;
    }
};

struct Task
{
    struct promise_type
    {
        Task get_return_object() { return Task{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }
        void return_void() {}
    };

    std::coroutine_handle<promise_type> handle;

    Task(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Task()
    {
        if (handle)
            handle.destroy();
    }
};

Task delay_and_print(int ms)
{
    std::cout << "+++Waiting..." << std::endl;
    co_await Sleeper(ms);
    std::cout << "+++Done waiting!" << std::endl;
}

int main()
{
    auto task = delay_and_print(1000);
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    std::cout << "task create..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    std::cout << "task call ..." << std::endl;

    task.handle.resume();
    std::cout << "task resume..." << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // Give the coroutine time to finish
    return 0;
}
// output :
// task create...
// task call ...
// +++Waiting...
// await_ready...
// await_suspend...
// task resume...
// sleep_for over...
// await_resume...
// +++Done waiting!

// OKOKOKOKOKOKOK  g++ coroutine.cpp -o demo  -std=c++20
// OKOKOKOKOKOKOK
// OKOKOKOKOKOKOK

// struct Task
// {
//     struct promise_type
//     {
//         std::promise<void> prom;

//         Task get_return_object() { return Task{prom.get_future()}; }
//         std::suspend_never initial_suspend() { return {}; }
//         std::suspend_always final_suspend() noexcept { return {}; }
//         void unhandled_exception() { prom.set_exception(std::current_exception()); }
//         void return_void() { prom.set_value(); }
//     };

//     std::future<void> fut;

//     Task(std::future<void> fut) : fut(std::move(fut)) {}
// };

// Task delay_and_print()
// {
//     std::cout << "+++Waiting..." << std::endl;
//     co_await Sleeper(1000);
//     std::cout << "+++Done waiting!" << std::endl;
// }

// int main()
// {
//     auto task = delay_and_print();
//     std::this_thread::sleep_for(std::chrono::milliseconds(1500));
//     std::cout << ">> task create..." << std::endl;
//     std::this_thread::sleep_for(std::chrono::milliseconds(2500));
//     std::cout << ">> task.fut.get() before..." << std::endl;

//     // task.fut.get(); // 等待协程完成
//     std::cout << ">> task.fut.get() after..." << std::endl;

//     return 0;
// }
// output :
// +++Waiting...
// await_ready...
// await_suspend...
// sleep_for over...
// await_resume...
// +++Done waiting!
// >> task create...
// >> task.fut.get() before...
// >> task.fut.get() after...

// #include <iostream>
// #include <thread>
// #include <future>

// void set_value(std::promise<int> &prom)
// {
//     std::this_thread::sleep_for(std::chrono::seconds(1)); // 模拟长时间计算
//     prom.set_value(42);                                   // 设置值
// }

// int main()
// {
//     std::promise<int> prom;
//     std::future<int> fut = prom.get_future(); // 获取关联的 future

//     std::thread t(set_value, std::ref(prom)); // 在新线程中设置值

//     std::cout << "Waiting for value..." << std::endl;
//     int value = fut.get(); // 阻塞等待值的设置
//     std::cout << "Value: " << value << std::endl;

//     t.join(); // 等待线程结束
//     return 0;
// }
