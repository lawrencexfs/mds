// main.cpp
#include <iostream>
#include "libadd.h" // Go生成的头文件

extern "C" void log_message(const char* msg) {
    std::cout << "Go Log: " << msg << std::endl;
}

int main() {
    int result = Add(3, 4);
    std::cout << "Result from Go: " << result << std::endl;
    return 0;
}


// # Linux/macOS
// g++ main.cpp -o main -L. -ladd -Wl,-rpath=.

// # Windows
// g++ main.cpp -o main.exe -L. -ladd
