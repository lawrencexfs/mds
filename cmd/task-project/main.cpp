#include <iostream>
#include <map>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <functional>
#include <chrono>
#include <algorithm>

#include <random>
#include <iomanip>
#include <atomic>
#include <ctime>
#include <sstream>

// 获取当前带毫秒的时间戳字符串
std::string getCurrentTimestampWithMilliseconds() {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto value = now_ms.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(value);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(value - seconds);

    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm_info;
#ifdef _WIN32
    localtime_s(&tm_info, &now_c);
#else
    localtime_r(&now_c, &tm_info);
#endif

    char buffer[26];
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", &tm_info);

    std::ostringstream oss;
    oss << buffer << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();
    return oss.str();
}

// 封装的打印带毫秒时间戳日志的函数
void logWithTimestamp(const std::string& message) {
    std::cout << getCurrentTimestampWithMilliseconds() << " - " << message << std::endl;
}

// 定义任务类型的枚举
enum class TaskType {
    ParseSourceCode = 901,
    GenerateTestCases = 911,
    ExecuteTestCases = 921,
    GenerateReport = 931,
    CalculateCoverage = 941,
    DataDisplay = 951
};


// 任务基类，包含任务的各种属性和操作
class Task {
public:
    int taskId;         // 任务ID
    int projectId;      // 项目ID，归属项目
    std::string taskName; // 任务名称，例如解析源码、生成用例等
    std::string sourceFilePath; // 源码路径
    int type;           // 任务类型，1,2,3,4,5等
    int nice;           // 任务优先级， -20+ ，默认20 ，可根据任务的执行时间、资源消耗等动态调整任务优先级，以优化整体调度效率
    std::vector<int> pre; // 前置依赖，任务ID数组，例如931 <-- [901,911,921]
    std::vector<int> next; // 串行执行设定，任务ID数组，例如951 <-- [931，941]，当前任务执行成功后，可以主动激活触发next任务ID数组
    std::string out;    // 输出，文件或者其他方法
    std::chrono::time_point<std::chrono::system_clock> time; // 执行时间
    std::chrono::seconds timeout; // 超时时间
    int flag;           // 任务执行标识，前置条件OK，准备就绪（默认0未就绪，1已就绪）
    int status;         // 任务状态（0 初始化，1 执行中 2执行成功 3异常失败），执行过程中也可以判断，可以设定多次机会，异常失败后是否可重入执行
    int change;         // 重试机会次数，默认1，执行一次便减1。根据类型设定重试次数
    std::function<void()> fexecute; // 执行函数，根据任务类型绑定函数
    std::function<void(int)> callback; // 回调函数，支持任务执行过程中的中间状态回调，如任务开始、任务进行到一定阶段等，提高系统的灵活性和可观测性

    // 构造函数，用于初始化任务的各项属性
    Task(int taskId, int projectId, const std::string& taskName, int type, int nice,
         const std::vector<int>& pre, const std::vector<int>& next, const std::string& out,
         const std::chrono::seconds& timeout, int change,
         std::function<void()> execute, std::function<void(int)> callback)
        : taskId(taskId), projectId(projectId), taskName(taskName), type(type), nice(nice),
          pre(pre), next(next), out(out), timeout(timeout), change(change),
          fexecute(execute), callback(callback) {
        flag = 0;
        status = 0;
        time = std::chrono::system_clock::now();
    }

    Task() : sourceFilePath("/") { 
        flag = 0;
        status = 0;
        time = std::chrono::system_clock::now();
    }

    // explicit Task(const std::string& sourceFilePath) : sourceFilePath(sourceFilePath) {}

    // 设置任务执行标识
    void setFlag(int f) { flag = f; }
    // 设置任务状态
    void setStatus(int s) { status = s; }
    // 设置任务执行时间
    void setTime() { time = std::chrono::system_clock::now(); }

};

// 线程池类，负责管理线程和执行任务
class ThreadPool {
public:
    // 构造函数，创建指定数量的线程
    ThreadPool(size_t numThreads) : stop(false)  {
        for (size_t i = 0; i < numThreads; ++i) {
            // 每个线程执行的Lambda函数，从任务队列中取出任务并执行
            threads.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        // 使用unique_lock来锁定互斥锁，以保护任务队列
                        std::unique_lock<std::mutex> lock(this->queueMutex);
                        // 等待条件变量，直到线程池停止或者任务队列不为空
                        this->condition.wait(lock, [this] { return this->stop ||!this->tasks.empty(); });
                        // 如果线程池停止且任务队列为空，则退出线程
                        if (this->stop && this->tasks.empty())
                            return;
                        // 从任务队列中取出一个任务
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    // 执行任务
                    task();
                }
            });
        }
    }

    // 析构函数，停止线程池并等待所有线程完成任务
    ~ThreadPool() {
        // std::cout<< "~ThreadPool, thread over ... " <<std::endl;
        logWithTimestamp("~ThreadPool, thread over ... ");
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& thread : threads) {
            thread.join();
        }
    }

    // 将任务添加到任务队列的模板函数
    template<class F, class... Args>
    void enqueue(F&& f, Args&&... args) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (stop) // 检查线程池是否已停止
            {
                // std::cout<< "enqueue, pool have stop ... " <<std::endl;
                logWithTimestamp("enqueue, pool have stop ... ");
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            tasks.emplace(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        }
        condition.notify_one();
    }
    std::atomic<bool> stop; // 线程池停止标志  

private:
    std::vector<std::thread> threads; // 线程池中的线程向量
    std::queue<std::function<void()>> tasks; // 任务队列
    std::mutex queueMutex; // 用于保护任务队列的互斥锁
    std::condition_variable condition; // 条件变量，用于线程间同步
};

class TaskManager {
public:
    TaskManager(ThreadPool& pool) : pool(pool), bStop(false) {}

    bool shouldStop() const {
        return bStop;
    }

    void buildTask(int taskId, int projectId, const std::string& taskName, int type, int nice,
                   const std::vector<int>& pre, const std::vector<int>& next, const std::string& out,
                   const std::chrono::seconds& timeout, int change,
                   std::function<void()> execute, std::function<void(int)> callback) {
        Task task(taskId, projectId, taskName, type, nice, pre, next, out, timeout, change, execute, callback);
        tasks[taskId] = task;
    }

    void checkTasks() {
        for (auto& entry : tasks) {
            Task& task = entry.second;
            if (task.flag == 1 || task.status != 0) {
                continue;
            }
            bool allPreDone = true;
            for (int preId : task.pre) {
                auto it = tasks.find(preId);
                if (it == tasks.end() || it->second.status != 2) {
                    allPreDone = false;
                    break;
                }
            }
            if (allPreDone) {
                task.setFlag(1);
                task.setStatus(1); // 设置任务状态为执行中
                addReadyTaskToQueue(&task);
            }
        }
    }

    // void addReadyTaskToQueue(Task* task) {
    //     std::lock_guard<std::mutex> lock(poolQueueMutex);
    //     readyQueue.push(task);
    //     poolQueueCondition.notify_one();
    // }

    void addReadyTasksToQueue() {
        for (auto& entry : tasks) {
            Task& task = entry.second;
            if (task.flag == 1 && task.status == 0) {
                readyQueue.push(&task);
            }
        }
    }
        
    // 在TaskManager中修改executeTasks，添加对shouldStop的检查
    void executeTasks() {
        while (!shouldStop()) {
            std::unique_lock<std::mutex> lock(poolQueueMutex);
            poolQueueCondition.wait(lock, [this] { return !readyQueue.empty() || shouldStop(); });
            if (shouldStop() && readyQueue.empty()) 
                return;

            while (!shouldStop() &&!readyQueue.empty()) {
                Task* task = readyQueue.top();
                readyQueue.pop();
                lock.unlock();
                if (!pool.stop.load()) {
                    pool.enqueue([task, this]() {
                        task->fexecute();
                        task->setStatus(2);
                        task->callback(2);
                    });
                }
                lock.lock();
            }
        }
    }
    // 在任务添加前，确保检查到池是否已停止
    void addReadyTaskToQueue(Task* task) {
        std::lock_guard<std::mutex> lock(poolQueueMutex);
        if (!pool.stop.load()) { // 仅当线程池未停止时才添加任务
            readyQueue.push(task);
            poolQueueCondition.notify_one();
        }
    }



    // 更新止步逻辑，
    void stop() {
        {
            // 设置停止状态
            std::lock_guard<std::mutex> lock(poolQueueMutex);
            bStop = true;
        }
        poolQueueCondition.notify_all();
        // 尝试清空readyQueue以避免在终止期间处理任务
        std::lock_guard<std::mutex> lock(poolQueueMutex);
        while (!readyQueue.empty()) readyQueue.pop();
    }

    void saveTaskResults() {
        for (auto& entry : tasks) {
            Task& task = entry.second;
            std::cout << "Task " << task.taskId << " result: " << (task.status == 2 ? "Success" : "Failure") << std::endl;
        }
    }

    struct CompareTasks {
        bool operator()(Task* a, Task* b) {
            return a->nice < b->nice;
        }
    };

private:
    ThreadPool& pool;
    std::map<int, Task> tasks;
    std::priority_queue<Task*, std::vector<Task*>, CompareTasks> readyQueue;
    std::mutex poolQueueMutex;
    std::condition_variable poolQueueCondition;
    bool bStop;
};

// 示例执行函数，解析源码
void parseSourceCode() {
    // std::cout << "Parsing source code..." << std::endl;
    logWithTimestamp("Parsing source code...");
}

// 示例执行函数，生成测试用例
void generateTestCases() {
    // std::cout << "Generating test cases..." << std::endl;
    logWithTimestamp("Generating test cases...");
}

// 示例执行函数，执行测试用例
void runTestCases() {
    logWithTimestamp("Runing test cases...");
}

// 示例执行函数，覆盖率计算
void CalculateCoverage() {
    logWithTimestamp("Calculate coverage ...");
}

// 根据不同状态输出相应信息
void callbackFunction(int status) {
    if (status == 1) {
        std::cout << "Task started." << std::endl;
    } else if (status == 2) {
        std::cout << "Task completed successfully." << std::endl;
    } else if (status == 3) {
        std::cout << "Task failed, retrying..." << std::endl;
    } else if (status == 4) {
        std::cout << "Task failed and no more retries." << std::endl;
    } else if (status == 5) {
        std::cout << "Task timed out." << std::endl;
    }
}

int main() {

    try {
        ThreadPool pool(std::thread::hardware_concurrency());
        TaskManager manager(pool);

        manager.buildTask(901, 1, "解析源码", 1, 20, {}, {}, "", std::chrono::seconds(10), 3,
                        parseSourceCode, callbackFunction);
        manager.buildTask(911, 1, "生成用例", 2, 20, {901}, {}, "", std::chrono::seconds(90), 3,
                        generateTestCases, callbackFunction);
        manager.buildTask(921, 1, "执行用例", 2, 20, {911}, {}, "", std::chrono::seconds(90), 3,
                        runTestCases, callbackFunction);
        manager.buildTask(931, 1, "覆盖率计算", 2, 20, {911}, {}, "", std::chrono::seconds(90), 3,
                        CalculateCoverage, callbackFunction);

        // manager.buildTask(902, 1, "解析源码", 1, 20, {}, {}, "", std::chrono::seconds(10), 3,
        //                   parseSourceCode, callbackFunction);

        std::thread checker([&manager] {
            while (!manager.shouldStop()) {
                logWithTimestamp("checker cycle ...");
                manager.checkTasks();
                manager.addReadyTasksToQueue();
                std::this_thread::sleep_for(std::chrono::milliseconds(400));
            }
        });

        std::thread executor([&manager] {
            manager.executeTasks();
        });

        std::cout << "Press Enter to stop..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cin.get();

        logWithTimestamp("demo stoping...");
        manager.stop(); // 停止任务管理器

        checker.join(); // 等待检查线程结束
        executor.join(); // 等待执行线程结束    
        
    } catch (const std::exception& e) {
        // 标准异常，可以直接调用what()方法
        std::cerr << "Caught a standard exception: " << e.what() << std::endl;
    } catch (...) {
        // 非标准异常，无法获取更多信息
        std::cerr << "Caught an unknown exception" << std::endl;
    }
    

    // manager.saveTaskResults();

    return 0;
}
