#include "bigint.h"
#include <vector>
#include <algorithm> // 包含std::reverse
#include <iostream>
#include <chrono>
#include <string.h>
#include "compress.h"
#include "base.h"

int main(int argc, char **argv)
{
    std::cout << "Hello World!" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for (auto i = 0; i < 1000; ++i)
    {
        LargeInt origin("315649811562056488133654506548956103684987630025466806548946434671");
        //     std::cout << "origin 字符串: " << origin.toString() << std::endl;
        auto temp = origin.toBase64String();
        //     std::cout << "base64 字符串: " << temp << std::endl;
        temp = origin.DecodeBase64(temp);
        //     std::cout << "decode 字符串: " << temp << std::endl;
        //     if (strcmp(origin.toString().c_str(), temp.c_str()) != 0)
        //     {
        //         std::cout << "Decoded err, buf disagree : " << temp << std::endl;
        //         return 0;
        //     }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << std::endl
              << "Time taken: " << duration.count() << " milliseconds" << std::endl;
//
    return 0;
}
