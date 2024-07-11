#include <iostream>
#include <string>
#include <lz4.h>
#include <zlib.h>
#include <string.h>
#include <chrono>
#include "compress.h"

std::string CompressLZ4::compressData(const std::string &input)
{
    // return std::string("ok");
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

std::string CompressLZ4::decompressData(const std::string &input)
{
    // return std::string("ok");
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

std::string CompressGZIP::compressData(const std::string &input)
{
    z_stream zs; // zlib流结构
    memset(&zs, 0, sizeof(zs));

    if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK)
        return "";

    zs.next_in = (Bytef *)input.data();
    zs.avail_in = input.size();

    int ret;
    char outbuffer[32768];
    std::string compressedString;

    do
    {
        zs.next_out = reinterpret_cast<Bytef *>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (compressedString.size() < zs.total_out)
        {
            compressedString.append(outbuffer, zs.total_out - compressedString.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END)
    {
        return "";
    }

    return compressedString;
    // return std::string("ok");
}

std::string CompressGZIP::decompressData(const std::string &input)
{
    z_stream zs; // zlib流结构
    memset(&zs, 0, sizeof(zs));

    if (inflateInit2(&zs, 15 + 16) != Z_OK)
        return "";

    zs.next_in = (Bytef *)input.data();
    zs.avail_in = input.size();

    int ret;
    char outbuffer[32768];
    std::string decompressedString;

    do
    {
        zs.next_out = reinterpret_cast<Bytef *>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (decompressedString.size() < zs.total_out)
        {
            decompressedString.append(outbuffer, zs.total_out - decompressedString.size());
        }
    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END)
    {
        return "";
    }

    return decompressedString;
    // return std::string("ok");
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
    CompressBase *pCompress = new CompressGZIP();
    auto start = std::chrono::high_resolution_clock::now();

    // for (auto i = 0; i < 1000000; ++i)
    {
        // 压缩数据
        std::string compressedData = pCompress->compressData(originalData);
        std::cout << "Compressed data: " << compressedData << std::endl;
        std::cout << "Compressed size: " << compressedData.size() << std::endl;

        // 解压缩数据
        std::string decompressedData = pCompress->decompressData(compressedData);
        std::cout << "Decompressed data: " << decompressedData << std::endl;
        std::cout << "Decompressed size: " << decompressedData.size() << std::endl;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << std::endl
              << "Time taken: " << duration.count() << " milliseconds" << std::endl;
    delete pCompress;
    return 0;
}
