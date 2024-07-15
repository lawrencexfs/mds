#include "bigint.h"
#include <vector>
#include <algorithm> // 包含std::reverse
#include <iostream>
#include <chrono>
#include <string.h>
#include "compress.h"
#include "base.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

int main(int argc, char **argv)
{
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

    boost::uuids::random_generator gen;
    boost::uuids::uuid uuid = gen();
    // 打印生成的UUID
    std::cout << "Generated UUID: " << uuid << std::endl;

    std::string original = boost::uuids::to_string(uuid);
    // 在调用std::remove之后
    original.erase(std::remove(original.begin(), original.end(), '-'), original.end());
    std::cout << "original UUID: " << original << std::endl;
    char base64_uuid[50];
    hex_uuid_to_base64(original.c_str(), base64_uuid);
    std::cout << "64进制字符串: " << base64_uuid << std::endl;

    CompressBase *pCompress = new CompressGZIP();
    start = std::chrono::high_resolution_clock::now();

    // for (auto i = 0; i < 1000000; ++i)
    {
        // 压缩数据
        std::string compressedData = pCompress->compressData(original);
        std::cout << "Compressed uuid: " << compressedData << std::endl;
        std::cout << "Compressed size: " << compressedData.size() << std::endl;

        // // 解压缩数据
        // std::string decompressedData = pCompress->decompressData(compressedData);
        // std::cout << "Decompressed data: " << decompressedData << std::endl;
        // std::cout << "Decompressed size: " << decompressedData.size() << std::endl;
    }
    {
        // 压缩数据
        std::string compressedData = pCompress->compressData(base64_uuid);
        std::cout << "Compressed base64: " << compressedData << std::endl;
        std::cout << "Compressed size: " << compressedData.size() << std::endl;
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << std::endl
              << "Time taken: " << duration.count() << " milliseconds" << std::endl;
    delete pCompress;

    return 0;
}
