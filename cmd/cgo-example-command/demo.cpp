#include <iostream>
#include "command.h"

extern "C" {
    char* RunCommand(char* path, char* args);
    void RunCommandWithOut(char* path, char* args, char** pstdout, char** pstderr, char** perrOut);
}

int Command(char* path, char* args) {

    char* out = nullptr;
    char* err = nullptr;
    char* errOut = nullptr;
    RunCommandWithOut(path, args, &out, &err, &errOut);
    std::cout << "run over" << std::endl << std::endl;
    // free(result); // 释放 Go 分配的内存
    if (out) {
        std::cout << "stdout: " << out << std::endl;
        free(out);
    }

    if (err) {
        std::cout << "stderr: " << err << std::endl;
        free(err);
    }

    if (errOut) {
        std::cout << "errOut: " << errOut << std::endl;
        free(errOut);
    }
    std::cout << "over ... " << std::endl << std::endl;
    return 0;
}

int main() {
    char pathx1[] = "ping";
    char args1[] = "-n 4 10.6.25.1";
    Command(pathx1, args1);
    char pathx2[] = "g++";
    char args2[] = "";
    Command(pathx2, nullptr);
}
// PS E:\space\mds\cmd\cgo-example-command> go build -buildmode=c-shared  -o command.dll  command.go
// PS E:\space\mds\cmd\cgo-example-command> g++ -o demo.exe demo.cpp -L. -lcommand
