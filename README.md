# mds
meta data server

# 本地编译运行
go build -o demo.exe .\mds\mds_demo.go

# 编译跨平台程序
CGO_ENABLED=0 GOOS=linux GOARCH=arm GOARM=7 CC=arm-linux-gnueabi-gcc-4.7 go build -o ./qilin/mds mds.go

yum whatprovides iostat -y

exec: "arm-linux-gnueabi-gcc-4.7": executable file not found in $PATH

export GOARCH=arm

  SET CGO_ENABLED=0
  SET GOOS=linux
  SET GOARCH=amd64
go build -o demo.linux .\mds\mds_demo.go
.
# go env
set GO111MODULE=
set GOARCH=amd64
set GOBIN=
set GOCACHE=C:\Users\YY\AppData\Local\go-build
set GOENV=C:\Users\YY\AppData\Roaming\go\env

set GOHOSTARCH=amd64
set GOHOSTOS=windows

set GOOS=windows
set GOPATH=E:\gospace

set GOPROXY=https://mirrors.aliyun.com/goproxy/,direct
set GOPROXY=https://goproxy.cn,https://proxy.golang.org,direct
set GOROOT=E:\tools\go
set GOSUMDB=sum.golang.org
set GOTOOLDIR=E:\tools\go\pkg\tool\windows_amd64

set GCCGO=gccgo
set AR=ar
set CC=gcc
set CXX=g++
set CGO_ENABLED=1


# 教程
https://zhuanlan.zhihu.com/p/98215258

Windows 下编译 Mac 和 Linux 64位可执行程序
  SET CGO_ENABLED=0
  SET GOOS=darwin
  SET GOARCH=amd64
  go build main.go
  ​
  SET CGO_ENABLED=0
  SET GOOS=linux
  SET GOARCH=amd64
  go build main.go


(1)首先进入go/src 源码所在目录，执行如下命令创建目标平台所需的包和工具文件。

$ cd /usr/local/go/src
$ CGO_ENABLED=0 GOOS=linux GOARCH=amd64 ./make.bash
 

如果是 Windows 则修改 GOOS 即可。

$ CGO_ENABLED=0 GOOS=windows GOARCH=amd64 ./make.bat
 
(2) 现在可以编译 Linux 和 Windows 平台所需的执行文件了。

$ CGO_ENABLED=0 GOOS=linux GOARCH=amd64 go build
$ CGO_ENABLED=0 GOOS=windows GOARCH=amd64 go build


flxa 开发环境

ftp://10.6.25.145/

CLion 2024.2.1
ftp://10.6.25.145/周小川/clion/CLion-2024.2.1.exe
1.17 GB (1,262,259,504 字节)

http://localhost:8083/#/index

VDI
20241218

禅道
http://192.169.1.96:8034/my.html
20241218fL

svn://192.169.1.27/valiantsec/%E4%B8%89%E5%BA%93/%E7%A0%94%E5%8F%91%E5%BA%93/iUnit/iUnit6.0.4-dev

http://192.169.128.2:8181/users/sign_in

vscode
C/C++
C/C++ Extension Pack
C/C++ Themes
C++ Intellisense
Chinese (Simplified)
翻译(英汉词典)
CMake
CMake Tools
CMake Integration
Git Graph
Go
Go Nightly

wsl --install

wsl --install -d Ubuntu

# 安装依赖
sudo apt - get install build - essential git cmake libprotobuf - dev protobuf - compiler libgflags - dev libgtest - dev
# 克隆BRPC仓库
git clone https://github.com/apache/brpc.git
cd brpc
# 编译安装
mkdir build && cd build
cmake..
make - j$(nproc)
sudo make install


https://github.com/sylar-yin/sylar.git



C:\Users\Administrator> vcpkg install protobuf
Computing installation plan...
The following packages are already installed:
    protobuf:x64-windows@5.29.1
protobuf:x64-windows is already installed
Total install time: 91.7 us
protobuf provides CMake targets:

  # this is heuristically generated, and may not be correct
  find_package(protobuf CONFIG REQUIRED)
  target_link_libraries(main PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

protobuf provides pkg-config modules:

  # Google's Data Interchange Format
  protobuf-lite

  # Google's Data Interchange Format
  protobuf

  1. **Boost库依赖解决**
   - **安装Boost库**：
     - **使用vcpkg**：
       - 首先，确保你已经安装了vcpkg。如果没有安装，可以从vcpkg的GitHub仓库（https://github.com/microsoft/vcpkg）克隆或下载vcpkg。打开命令提示符，切换到vcpkg目录，运行`bootstrap - vcpkg.bat`（在Windows上）来安装vcpkg。
       - 安装完成后，运行`vcpkg install boost`来安装Boost库。vcpkg会自动下载、编译和安装Boost库及其依赖项。安装完成后，vcpkg会提示安装路径，例如`C:\vcpkg\installed\x64 - windows`。
     - **手动下载和编译**：
       - 从Boost官方网站（https://www.boost.org/users/download/）下载Boost库的源代码压缩包。解压下载的压缩包到一个目录，例如`C:\boost_1_83_0`。
       - 打开命令提示符，切换到解压后的Boost目录，运行`bootstrap.bat`，这会生成`b2.exe`构建工具。然后运行`b2 install`进行编译和安装。安装路径可以在`b2`命令中通过`--prefix`选项指定，默认会安装到系统的`Program Files`目录下。
   - **配置CMake使用Boost**：
     - 如果使用vcpkg安装Boost，在CMake中需要设置`CMAKE_TOOLCHAIN_FILE`变量。在你的`CMakeLists.txt`文件开头添加以下内容：
       ```cmake
       set(CMAKE_TOOLCHAIN_FILE C:/vcpkg/scripts/buildsystems/vcpkg.cmake)
       ```
       - 这里假设vcpkg安装在`C:\vcpkg`目录下。之后，`find_package(Boost REQUIRED)`就可以找到Boost库了。
     - 如果手动安装Boost，确保`Boost_INCLUDE_DIRS`环境变量指向Boost库的包含目录（例如`C:\boost_1_83_0`），并且`Boost_LIBRARY_DIRS`指向Boost库的库文件目录（例如`C:\boost_1_83_0\stage\lib`）。然后`find_package(Boost REQUIRED)`就能正常工作。

2. **MySQL依赖解决**
   - **安装MySQL开发包**：
     - 从MySQL官方网站（https://dev.mysql.com/downloads/）下载MySQL的开发包（MySQL Connector/C）。下载完成后，运行安装程序。在安装过程中，选择安装开发文件（通常包含头文件和库文件），安装路径可以选择默认路径或自定义路径。
   - **配置CMake使用MySQL**：
     - 在`CMakeLists.txt`中，确保`MYSQL_DIR`环境变量指向MySQL开发包的安装目录（例如`C:\Program Files\MySQL\MySQL Connector C 6.1`）。然后`find_package(MySQL REQUIRED)`会在该目录下查找MySQL的配置文件，找到后`MYSQL_FOUND`会被设置为真，`MYSQL_INCLUDE_DIR`会包含MySQL的头文件路径。

3. **Protobuf依赖解决**
   - **安装Protobuf**：
     - **使用vcpkg**：运行`vcpkg install protobuf`来安装Protobuf库及其依赖项。vcpkg会自动处理安装过程。
     - **手动安装**：从Protobuf官方GitHub仓库（https://github.com/protocol - buffers/protobuf/releases）下载预编译的二进制文件或源代码。如果下载二进制文件，解压后将`bin`目录添加到系统路径，`include`目录添加到`Protobuf_INCLUDE_DIRS`环境变量（如果需要手动设置）。如果从源代码编译，按照官方文档的说明进行编译和安装。
   - **配置CMake使用Protobuf**：
     - 如果使用vcpkg安装，设置`CMAKE_TOOLCHAIN_FILE`变量后，`find_package(Protobuf)`可以正常工作。如果手动安装，确保`Protobuf_DIR`环境变量指向Protobuf的安装目录（例如`C:\protobuf - installed - dir\cmake`），这样`find_package(Protobuf)`就能找到Protobuf库。

4. **OpenSSL依赖解决**
   - **安装OpenSSL**：
     - **使用vcpkg**：运行`vcpkg install openssl`，vcpkg会自动下载、编译和安装OpenSSL库。
     - **手动安装**：从OpenSSL官方网站（https://www.openssl.org/source/）下载源代码。解压后，使用命令行工具（如`nmake`）按照官方文档的说明进行编译和安装。安装完成后，记录安装路径。
   - **配置CMake使用OpenSSL**：
     - 如果使用vcpkg安装，设置`CMAKE_TOOLCHAIN_FILE`变量后，`find_package(OpenSSL REQUIRED)`可以正常工作。如果手动安装，确保`OPENSSL_ROOT_DIR`环境变量指向OpenSSL的安装目录（例如`C:\OpenSSL - Win64`），这样`find_package(OpenSSL REQUIRED)`就能找到OpenSSL库，`OPENSSL_INCLUDE_DIR`会包含OpenSSL的头文件路径。



1. **优化设计思路**
    - **使用智能指针管理任务对象**：在C++中，为了更好地管理任务对象的生命周期，避免内存泄漏，使用`std::unique_ptr`或`std::shared_ptr`来管理任务对象。
    - **线程安全**：考虑到任务调度可能涉及多线程操作，对任务池、就绪队列和执行队列的操作需要保证线程安全。可以使用互斥锁（`std::mutex`）和条件变量（`std::condition_variable`）来实现线程同步。
    - **优先级队列实现**：使用`std::priority_queue`来实现任务就绪优先级队列，并自定义比较函数以根据任务优先级进行排序。
    - **回调函数封装**：使用`std::function`来封装回调函数，使其可以接受不同类型的可调用对象。

2. **C++ 代码实现**

```cpp
#include <iostream>
#include <queue>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>

// 任务类型枚举
enum class TaskType {
    ParseSourceCode = 1,
    GenerateTestCases,
    ExecuteTestCases,
    GenerateReport,
    CalculateCoverage,
    DisplayData
};

// 任务状态枚举
enum class TaskStatus {
    Initialized = 0,
    InProgress,
    Success,
    Failed
};

class Task;

// 任务比较函数，用于优先级队列
struct TaskComparator {
    bool operator()(const std::shared_ptr<Task>& a, const std::shared_ptr<Task>& b) {
        return a->nice > b->nice;
    }
};

class Task {
public:
    Task(int taskId, int projectId, TaskType type, int nice = 20)
        : taskId(taskId), projectId(projectId), taskType(type), nice(nice), flag(0), status(TaskStatus::Initialized) {}

    int taskId;
    int projectId;
    TaskType taskType;
    int nice;
    std::vector<int> pre;
    std::vector<int> next;
    std::string out;
    int time;
    int timeout;
    int flag;
    TaskStatus status;
    std::function<void(const Task&)> callback;
};

class TaskScheduler {
public:
    void addTask(std::shared_ptr<Task> task) {
        std::unique_lock<std::mutex> lock(taskPoolMutex);
        taskPool[task->taskId] = task;
    }

    void checkReadyTasks() {
        while (true) {
            std::unique_lock<std::mutex> lock(taskPoolMutex);
            for (auto& entry : taskPool) {
                auto task = entry.second;
                if (task->status == TaskStatus::Initialized && allPreconditionsMet(task)) {
                    task->flag = 1;
                    std::unique_lock<std::mutex> readyLock(readyQueueMutex);
                    readyQueue.push(task);
                    readyLock.unlock();
                    readyCondition.notify_one();
                }
            }
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void executeTasks() {
        while (true) {
            std::unique_lock<std::mutex> lock(readyQueueMutex);
            readyCondition.wait(lock, [this] { return!readyQueue.empty(); });
            auto task = readyQueue.top();
            readyQueue.pop();
            lock.unlock();

            std::unique_lock<std::mutex> execLock(executionQueueMutex);
            executionQueue.push(task);
            execLock.unlock();

            std::thread([this, task] {
                task->status = TaskStatus::InProgress;
                std::this_thread::sleep_for(std::chrono::seconds(task->time));
                if (task->status == TaskStatus::InProgress) {
                    if (task->callback) task->callback(*task);
                    task->status = TaskStatus::Success;
                } else {
                    task->status = TaskStatus::Failed;
                }
                std::unique_lock<std::mutex> execLock(executionQueueMutex);
                executionQueue.pop();
                if (task->status == TaskStatus::Success) {
                    // 处理输出
                    handleOutput(task);
                    // 触发后续任务
                    triggerNextTasks(task);
                } else {
                    // 处理失败
                    handleFailure(task);
                }
            }).detach();
        }
    }

private:
    bool allPreconditionsMet(const std::shared_ptr<Task>& task) {
        for (int preId : task->pre) {
            auto it = taskPool.find(preId);
            if (it == taskPool.end() || it->second->flag == 0) {
                return false;
            }
        }
        return true;
    }

    void handleOutput(const std::shared_ptr<Task>& task) {
        // 处理任务输出逻辑
        std::cout << "Task " << task->taskId << " output: " << task->out << std::endl;
    }

    void triggerNextTasks(const std::shared_ptr<Task>& task) {
        std::unique_lock<std::mutex> lock(taskPoolMutex);
        for (int nextId : task->next) {
            auto it = taskPool.find(nextId);
            if (it!= taskPool.end()) {
                it->second->flag = 1;
            }
        }
    }

    void handleFailure(const std::shared_ptr<Task>& task) {
        // 处理任务失败逻辑
        std::cout << "Task " << task->taskId << " failed." << std::endl;
    }

    std::unordered_map<int, std::shared_ptr<Task>> taskPool;
    std::priority_queue<std::shared_ptr<Task>, std::vector<std::shared_ptr<Task>>, TaskComparator> readyQueue;
    std::queue<std::shared_ptr<Task>> executionQueue;
    std::mutex taskPoolMutex;
    std::mutex readyQueueMutex;
    std::mutex executionQueueMutex;
    std::condition_variable readyCondition;
};

```
```

在上述设计流程图中：
 - **任务构建并推送至任务池**：表示创建新任务并将其添加到任务池。
 - **任务池定时检查**：任务池定期检查任务的前置依赖是否就绪以及任务状态是否为初始化。
 - **按优先级加入就绪队列**：满足条件的任务按照优先级加入到就绪队列。
 - **从就绪队列取任务到执行队列**：就绪队列中的任务按优先级顺序被取出，放入执行队列准备执行。
 - **执行任务**：执行队列中的任务开始执行。
 - **判断任务执行结果**：检查任务是否成功执行。
 - **任务成功处理**：如果任务执行成功，修改任务状态为成功并从执行队列中移除，然后输出任务结果并保存文件，最后触发`Next`任务。
 - **任务失败处理**：若任务执行失败，修改任务状态为失败，接着判断该任务是否可重入执行。如果可重入，任务回到任务池等待下次检查执行；若不可重入，则结束任务并按失败情况处理。

上述C++代码实现了一个基本的任务调度系统，包括任务的添加、调度和执行，并考虑了线程安全和优先级队列的使用。你可以根据实际需求进一步扩展和优化该代码。


class Task {
    // ...
    int retryCount = 0;
    static constexpr int maxRetries = 3;

    void execute() {
        for (int i = 0; i < maxRetries; ++i) {
            try {
                doWork();
                finish(true);
                break;
            } catch (const std::exception& e) {
                retryCount++;
                if (i < maxRetries - 1) {
                    continue;
                }
                finish(false);
            }
        }
    }
};



### 任务属性详细说明

- **任务ID (Taskid)**：唯一标识，例如901、902。
- **项目ID (Projectid)**：任务所属的项目。
- **任务名称 (Taskname)**：具体名称，如“解析源码”、“生成用例”等。
- **任务类型 (Type)**：任务的类型，用数字表示，如1、2、3等。
- **优先级 (Nice)**：任务的优先级，数值越低优先级越高，默认值为20。
- **前置依赖 (Pre)**：任务执行前需要完成的其他任务的ID数组。
- **后续任务 (Next)**：当前任务完成后需要触发的其他任务的ID数组。
- **输出 (Out)**：任务的输出方式，可以是文件或其他形式。
- **执行时间 (Time)**：任务的执行时间。
- **超时时间 (Timeout)**：任务的超时时间。
- **任务执行标识 (Flag)**：表示任务是否就绪，0表示未就绪，1表示已就绪。
- **任务状态 (Status)**：任务的当前状态，0表示初始化，1表示执行中，2表示执行成功，3表示异常失败。
- **回调函数 (Callback)**：任务执行过程中的回调函数，用于报告任务的进展和状态变化。

### 任务执行方式

任务的执行过程可以分为以下几个步骤：

1. **任务构建**：将任务推送到任务池中。
2. **任务池管理**：任务池是一个任务映射表，负责存储和管理所有任务。
3. **定时检查**：定期检查任务的执行条件，包括前置依赖任务的完成情况和任务状态。
4. **任务就绪**：当任务的所有前置依赖任务完成且任务状态为就绪时，将其添加到任务就绪优先级队列中。
5. **任务就绪优先级队列**：根据任务的优先级进行排序，确保高优先级的任务先执行。
6. **任务执行队列**：从任务就绪优先级队列中取出任务，开始执行。
7. **任务执行**：执行任务，如果成功则修改任务状态为执行成功，失败则修改为异常失败，并进行相应的处理。
8. **任务结果输出**：将任务的执行结果输出到指定的位置，如文件或其他形式。

•	集成日志系统，记录任务的生命周期事件，便于问题追踪和性能分析。
•	实现监控指标，如任务队列长度、执行时间统计和资源使用情况。


### 优化方向
1. **资源管理与监控**
    - **动态资源分配**：考虑为每个任务分配资源（如CPU时间片、内存空间）。可以根据任务的优先级和预估资源需求，在运行时动态分配资源。例如，高优先级且资源需求低的任务优先获得资源。
    - **资源监控**：添加资源监控功能，实时跟踪系统资源的使用情况。当资源紧张时，调整任务的优先级或暂停一些低优先级任务，以保证关键任务的执行。
2. **任务依赖处理优化**
    - **依赖图优化**：对于复杂的任务依赖关系，可以构建更高效的依赖图数据结构。例如，使用邻接表来表示任务之间的依赖关系，这样在检查前置依赖时可以更快速地遍历。
    - **循环依赖检测**：增加循环依赖检测机制，避免因任务之间的循环依赖导致死锁。在任务添加到任务池时，检查是否存在循环依赖。
3. **并发控制与性能优化**
    - **多线程执行**：利用多线程技术并行执行独立的任务，提高整体执行效率。但要注意线程安全问题，对共享资源（如任务池、就绪队列等）的访问需要进行同步。
    - **异步执行**：采用异步编程模型，例如使用`std::async`或线程池来异步执行任务，避免阻塞主线程。
    - **减少锁争用**：在多线程环境下，优化锁的使用，尽量减少锁的粒度和持有时间。例如，可以为不同的任务队列（任务池、就绪队列、执行队列）分别使用不同的锁，避免一把大锁锁住所有资源。
4. **任务状态管理与日志记录**
    - **详细状态跟踪**：除了现有的任务状态，增加更多中间状态，如“等待资源”“暂停”等，以便更精确地跟踪任务的执行过程。
    - **日志记录**：引入详细的日志记录功能，记录任务的创建、调度、执行、完成、失败等各个阶段的关键信息。这有助于调试和监控系统的运行状态。
5. **可扩展性与灵活性**
    - **插件化设计**：采用插件化架构，使得新的任务类型和执行函数可以方便地添加到系统中，而无需修改核心代码。例如，通过定义接口，每个任务类型的执行函数实现该接口，然后在运行时动态加载插件。
    - **配置文件支持**：将任务的一些参数（如优先级、重试次数、资源需求等）通过配置文件进行管理，方便在不修改代码的情况下调整任务调度策略。

### C++ 编码注意事项
1. **数据结构选择**
    - **任务池**：使用`std::unordered_map<int, Task>`来存储任务，其中`int`为`Taskid`，这样可以快速通过任务ID查找任务。
    - **就绪队列**：使用`std::priority_queue`，并自定义比较函数来实现根据任务优先级排序。
    - **执行队列**：可以使用`std::queue`来存储正在执行的任务。
2. **内存管理**
    - 使用智能指针（如`std::shared_ptr`或`std::unique_ptr`）来管理任务对象，避免内存泄漏。例如，`std::unordered_map<int, std::shared_ptr<Task>> taskPool;`
3. **线程安全**
    - **锁的使用**：对任务池、就绪队列和执行队列的访问需要加锁。例如，使用`std::mutex`来保护共享资源。
    - **条件变量**：结合`std::condition_variable`来实现线程间的同步。例如，当就绪队列中有新任务时，通知等待的线程去执行。
4. **函数指针与回调**
    - 使用`std::function`来定义执行函数和回调函数，这样可以接受不同类型的可调用对象，提高代码的灵活性。例如，`std::function<void(Task&)> execute;` 和 `std::function<void(const Task&, int progress)> callback;`
5. **异常处理**
    - 在任务执行过程中，可能会出现各种异常（如资源不足、执行函数抛出异常等）。使用`try - catch`块来捕获和处理异常，确保任务的状态能够正确更新，并且不会导致程序崩溃。
6. **模板与泛型编程**
    - 如果任务的某些操作具有通用性，可以使用模板来实现泛型代码。例如，对于任务的执行函数，如果不同类型的任务执行逻辑有相似之处，可以通过模板来减少重复代码。

以下是一个简单的C++ 代码框架示例，展示了部分上述要点：

```cpp
#include <iostream>
#include <unordered_map>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>

// 任务类型枚举
enum class TaskType {
    ParseSourceCode = 1,
    GenerateTestCases,
    ExecuteTestCases,
    GenerateReport,
    CalculateCoverage,
    DisplayData
};

// 任务状态枚举
enum class TaskStatus {
    Initialized = 0,
    InProgress,
    Success,
    Failed,
    WaitingForResources,
    Paused
};

class Task;

// 任务比较函数，用于优先级队列
struct TaskComparator {
    bool operator()(const std::shared_ptr<Task>& a, const std::shared_ptr<Task>& b) {
        return a->nice > b->nice;
    }
};

class Task {
public:
    Task(int taskId, int projectId, TaskType type, int nice = 20)
        : taskId(taskId), projectId(projectId), taskType(type), nice(nice), flag(0), status(TaskStatus::Initialized), change(1) {}

    int taskId;
    int projectId;
    TaskType taskType;
    int nice;
    std::vector<int> pre;
    std::vector<int> next;
    std::string out;
    int time;
    int timeout;
    int flag;
    TaskStatus status;
    std::function<void(Task&)> execute;
    std::function<void(const Task&, int progress)> callback;
    int change;
};

class TaskScheduler {
public:
    void addTask(std::shared_ptr<Task> task) {
        std::unique_lock<std::mutex> lock(taskPoolMutex);
        taskPool[task->taskId] = task;
    }

    void checkReadyTasks() {
        while (true) {
            std::unique_lock<std::mutex> lock(taskPoolMutex);
            for (auto& entry : taskPool) {
                auto task = entry.second;
                if (task->status == TaskStatus::Initialized && allPreconditionsMet(task)) {
                    task->flag = 1;
                    std::unique_lock<std::mutex> readyLock(readyQueueMutex);
                    readyQueue.push(task);
                    readyLock.unlock();
                    readyCondition.notify_one();
                }
            }
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void executeTasks() {
        while (true) {
            std::unique_lock<std::mutex> lock(readyQueueMutex);
            readyCondition.wait(lock, [this] { return!readyQueue.empty(); });
            auto task = readyQueue.top();
            readyQueue.pop();
            lock.unlock();

            std::unique_lock<std::mutex> execLock(executionQueueMutex);
            executionQueue.push(task);
            execLock.unlock();

            std::thread([this, task] {
                task->status = TaskStatus::InProgress;
                try {
                    if (task->execute) {
                        task->execute(*task);
                        task->status = TaskStatus::Success;
                    }
                } catch (...) {
                    if (task->change > 0) {
                        task->change--;
                        task->status = TaskStatus::Initialized;
                    } else {
                        task->status = TaskStatus::Failed;
                    }
                }
                std::unique_lock<std::mutex> execLock(executionQueueMutex);
                executionQueue.pop();
                if (task->status == TaskStatus::Success) {
                    // 处理输出
                    handleOutput(task);
                    // 触发后续任务
                    triggerNextTasks(task);
                } else {
                    // 处理失败
                    handleFailure(task);
                }
            }).detach();
        }
    }

private:
    bool allPreconditionsMet(const std::shared_ptr<Task>& task) {
        for (int preId : task->pre) {
            auto it = taskPool.find(preId);
            if (it == taskPool.end() || it->second->status!= TaskStatus::Success) {
                return false;
            }
        }
        return true;
    }

    void handleOutput(const std::shared_ptr<Task>& task) {
        // 处理任务输出逻辑
        std::cout << "Task " << task->taskId << " output: " << task->out << std::endl;
    }

    void triggerNextTasks(const std::shared_ptr<Task>& task) {
        std::unique_lock<std::mutex> lock(taskPoolMutex);
        for (int nextId : task->next) {
            auto it = taskPool.find(nextId);
            if (it!= taskPool.end()) {
                it->second->flag = 1;
            }
        }
    }

    void handleFailure(const std::shared_ptr<Task>& task) {
        // 处理任务失败逻辑
        std::cout << "Task " << task->taskId << " failed." << std::endl;
    }

    std::unordered_map<int, std::shared_ptr<Task>> taskPool;
    std::priority_queue<std::shared_ptr<Task>, std::vector<std::shared_ptr<Task>>, TaskComparator> readyQueue;
    std::queue<std::shared_ptr<Task>> executionQueue;
    std::mutex taskPoolMutex;
    std::mutex readyQueueMutex;
    std::mutex executionQueueMutex;
    std::condition_variable readyCondition;
};

```

以上代码提供了一个基本的任务调度框架，你可以根据实际需求进一步完善和扩展。

进一步优化设计
引入任务依赖图：

问题：当前设计中，任务依赖是通过ID数组表示的，这在处理复杂依赖关系时可能不够直观。
解决方案：引入一个任务依赖图，将任务及其依赖关系表示为图结构。这有助于更高效地管理和分析任务依赖，特别是在处理循环依赖或更复杂的依赖关系时。
动态优先级调整：

问题：Nice值用于确定任务的优先级，但它是静态的。
解决方案：实现一个动态优先级调整机制，根据任务的执行时间、资源消耗和等待时间等因素动态调整Nice值。这可以优化调度效率，确保关键任务及时执行。
增强的错误处理和重试机制：

问题：Change属性用于重试机会，但重试逻辑可能需要更细致的控制。
解决方案：引入更复杂的错误分类和重试策略，例如区分可重试错误和不可重试错误，为不同类型的错误设置不同的重试策略。
细粒度的任务状态管理：

问题：状态包括初始化、执行中、执行成功和异常失败，但可能需要更细致的状态划分。
解决方案：增加更多中间状态，如等待资源、暂停、恢复等，以更精确地描述任务的生命周期。
资源管理集成：

问题：任务调度与资源管理是分开的，可能导致资源竞争或浪费。
解决方案：将资源管理集成到调度模块中，确保任务调度时考虑资源可用性，避免资源冲突。
可扩展性和模块化：

问题：随着系统扩展，调度模块可能变得难以维护。
解决方案：设计一个插件式架构，允许在不修改核心调度器的情况下添加新任务类型或资源管理器。
监控和日志：

问题：系统可能缺乏足够的监控和日志记录。
解决方案：实现全面的监控和日志系统，记录任务的执行状态、性能指标和错误信息，以便于问题诊断和性能优化。





