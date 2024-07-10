#include <iostream>
#include <string>
#include <zlib.h>
#include <string.h>
#include <chrono>
// g++ -o demo gzip.cpp  -L/usr/lib -lssl -lcrypto -luuid -L/usr/local/lib -ggdb -lz
// g++ -o demo gzip.cpp  -L/usr/lib -lssl -lcrypto -luuid -L/usr/local/lib -lz -O2

// 压缩数据
std::string compressData(const std::string &input)
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
}

// 解压缩数据
std::string decompressData(const std::string &input)
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
