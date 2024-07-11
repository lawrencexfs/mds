#include <iostream>
#include <string>
#include <lz4.h>
#include <chrono>

// g++ -o demo lz4.cpp  -L/usr/lib -lssl -lcrypto -luuid -L/usr/local/lib -ggdb -llz4
// g++ -o demo lz4.cpp  -L/usr/lib -lssl -lcrypto -luuid -L/usr/local/lib -llz4 -O2

// 压缩数据
std::string compressData(const std::string &input)
{
    // 计算压缩后的最大长度
    int maxCompressedSize = LZ4_compressBound(input.size());

    // 分配压缩后的缓冲区
    char *compressedData = new char[maxCompressedSize];

    // 压缩数据
    int compressedSize = LZ4_compress_default(input.data(), compressedData, input.size(), maxCompressedSize);

    // 构造压缩后的字符串
    std::string compressedString(compressedData, compressedSize);

    // 释放缓冲区
    delete[] compressedData;

    return compressedString;
}

// 解压缩数据
std::string decompressData(const std::string &input)
{
    // 获取解压缩后的最大长度
    int maxDecompressedSize = input.size() * 10; // 估算一个足够大的解压缩后的长度

    // 分配解压缩后的缓冲区
    char *decompressedData = new char[maxDecompressedSize];

    // 解压缩数据
    int decompressedSize = LZ4_decompress_safe(input.data(), decompressedData, input.size(), maxDecompressedSize);
    if (decompressedSize <= 0)
    {
        std::cout << "Decompressed buff size not long enough. " << maxDecompressedSize << std::endl;
        return std::string();
    }

    // 构造解压缩后的字符串
    std::string decompressedString(decompressedData, decompressedSize);

    // 释放缓冲区
    delete[] decompressedData;

    return decompressedString;
}

int main()
{
    std::string originalData = ("{ \
    \"sender\":\"web\", \
    \"receive\":\"redisService\",\
    \"level\":\"session\",\
    \"email\":\"\", \
    \"uuid\":\"\",\
    \"login\":0, \
    \"ExpirationTime\":1200,\
    \"EnableDataBaseType\":0,\
    \"sender\":\"web\", \
    \"receive\":\"redisService\",\
    \"level\":\"session\",\
    \"uuid\":\"\",\
    \"login\":0, \
    \"ExpirationTime\":1200,\
    \"EnableDataBaseType\":0,\
    \"order\":\"CreateSession\"\
                }");
    std::cout << "Original data: " << originalData << std::endl;
    std::cout << "Original size: " << originalData.size() << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    // for (auto i = 0; i < 1000000; ++i)
    {
        // 压缩数据
        std::string compressedData = compressData(originalData);
        // std::cout << "Compressed data: " << compressedData << std::endl;
        std::cout << "Compressed size: " << compressedData.size() << std::endl;

        // 解压缩数据
        std::string decompressedData = decompressData(compressedData);
        // std::cout << "Decompressed data: " << decompressedData << std::endl;
        // std::cout << "Decompressed size: " << decompressedData.size() << std::endl;

        // if (decompressedData.compare(originalData) != 0)
        // {
        //     std::cout << "not eq: " << std::endl;
        //     return -1;
        // }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << std::endl
              << "Time taken: " << duration.count() << " milliseconds" << std::endl;

    return 0;
}
