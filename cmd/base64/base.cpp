#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include <random>
#include <iomanip>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

std::string generate_uuid()
{
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, 15);

    std::stringstream ss;
    for (int i = 0; i < 36; ++i)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            ss << '-';
        }
        else
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << dist(generator);
        }
    }
    return ss.str();
}

// sudo pacman -S boost
// g++ -o demo base.cpp  -L/usr/lib -lssl -lcrypto -luuid -L/usr/local/lib -lboost_system -g

// // Base64编码
// std::string base64Encode(const std::string &input)
// {
//     using namespace boost::archive::iterators;

//     typedef base64_from_binary<transform_width<const char *, 6, 8>> base64_encoder;

//     std::string encodedString(base64_encoder(input.c_str()), base64_encoder(input.c_str() + input.size()));
//     // 处理Base64编码的填充字符
//     size_t padding = (3 - input.size() % 3) % 3;
//     encodedString.append(padding, '=');

//     return encodedString;
// }
// // Base64解码
// std::string base64Decode(const std::string &input)
// {
//     using namespace boost::archive::iterators;

//     typedef transform_width<binary_from_base64<const char *>, 8, 6> base64_decoder;

//     std::string decodedString(base64_decoder(input.c_str()), base64_decoder(input.c_str() + input.size()));
//     // 处理Base64解码的填充字符
//     decodedString.erase(decodedString.end() - (input.size() % 4), decodedString.end());

//     return decodedString;
// }

// Base64字符集
const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// 将16进制UUID转换为64进制编码
void hex_uuid_to_base64(const char *hex_uuid, char *base64_uuid)
{
    uint8_t binary_uuid[16];
    size_t i, j;

    // 将16进制UUID转换为二进制数据
    for (i = 0, j = 0; i < 32; i += 2, j++)
    {
        char hex_byte[3];
        hex_byte[0] = hex_uuid[i];
        hex_byte[1] = hex_uuid[i + 1];
        hex_byte[2] = '\0';
        binary_uuid[j] = (uint8_t)strtol(hex_byte, NULL, 16);
    }

    // 进行Base64编码
    size_t base64_len = (size_t)(((j + 2) / 3) * 4);
    size_t padding_len = (size_t)(3 - (j % 3)) % 3;
    base64_uuid[base64_len] = '\0';

    for (i = 0, j = 0; i < base64_len; i += 4, j += 3)
    {
        uint32_t combined = (binary_uuid[j] << 16) | (binary_uuid[j + 1] << 8) | binary_uuid[j + 2];
        base64_uuid[i] = base64_chars[(combined >> 18) & 0x3F];
        base64_uuid[i + 1] = base64_chars[(combined >> 12) & 0x3F];
        base64_uuid[i + 2] = base64_chars[(combined >> 6) & 0x3F];
        base64_uuid[i + 3] = base64_chars[combined & 0x3F];
    }

    // 添加Base64填充字符
    for (i = 0; i < padding_len; i++)
    {
        base64_uuid[base64_len - 1 - i] = '=';
    }
}

// Convert decimal to base 64   十进制 字符串数字  buf  len  dst len  -> int <=0 异常
int decimal_string_to_base64(const char *decimal, int len, char *base64, int base64_len)
{
    uint8_t binary[len + 1];
    size_t i, j;

    // 将16进制UUID转换为二进制数据
    for (i = 0, j = 0; i < len; i += 2, j++)
    {
        char hex_byte[3];
        hex_byte[0] = decimal[i];
        hex_byte[1] = decimal[i + 1];
        hex_byte[2] = '\0';
        binary[j] = (uint8_t)strtol(hex_byte, NULL, 10);
    }

    // 进行Base64编码
    // size_t base64_len = (size_t)(((j + 2) / 3) * 4);
    size_t padding_len = (size_t)(3 - (j % 3)) % 3;
    base64[base64_len] = '\0';

    for (i = 0, j = 0; i < base64_len; i += 4, j += 3)
    {
        uint32_t combined = (binary[j] << 16) | (binary[j + 1] << 8) | binary[j + 2];
        binary[i] = base64_chars[(combined >> 18) & 0x3F];
        binary[i + 1] = base64_chars[(combined >> 12) & 0x3F];
        binary[i + 2] = base64_chars[(combined >> 6) & 0x3F];
        binary[i + 3] = base64_chars[combined & 0x3F];
    }

    // 添加Base64填充字符
    for (i = 0; i < padding_len; i++)
    {
        base64[base64_len - 1 - i] = '=';
    }
    return padding_len;
}

// 将64进制编码的UUID进行解码为16进制UUID
void base64_to_hex_uuid(const char *base64_uuid, char *hex_uuid)
{
    size_t base64_len = strlen(base64_uuid);
    size_t padding_len = 0;

    // 移除Base64填充字符
    while (base64_len > 0 && base64_uuid[base64_len - 1] == '=')
    {
        padding_len++;
        base64_len--;
    }

    // 计算解码后的二进制数据长度
    size_t binary_len = (size_t)((base64_len * 3) / 4);

    // 进行Base64解码
    uint8_t binary_uuid[binary_len];
    size_t i, j;

    for (i = 0, j = 0; i < base64_len; i += 4, j += 3)
    {
        uint32_t combined = 0;
        size_t k;

        for (k = 0; k < 4; k++)
        {
            combined <<= 6;
            if (base64_uuid[i + k] >= 'A' && base64_uuid[i + k] <= 'Z')
            {
                combined |= base64_uuid[i + k] - 'A';
            }
            else if (base64_uuid[i + k] >= 'a' && base64_uuid[i + k] <= 'z')
            {
                combined |= base64_uuid[i + k] - 'a' + 26;
            }
            else if (base64_uuid[i + k] >= '0' && base64_uuid[i + k] <= '9')
            {
                combined |= base64_uuid[i + k] - '0' + 52;
            }
            else if (base64_uuid[i + k] == '+')
            {
                combined |= 62;
            }
            else if (base64_uuid[i + k] == '/')
            {
                combined |= 63;
            }
        }

        binary_uuid[j] = (combined >> 16) & 0xFF;
        binary_uuid[j + 1] = (combined >> 8) & 0xFF;
        binary_uuid[j + 2] = combined & 0xFF;
    }

    // 将二进制数据转换为16进制UUID
    for (i = 0, j = 0; i < binary_len; i++, j += 2)
    {
        sprintf(hex_uuid + j, "%02X", binary_uuid[i]);
    }
}

int main()
{
    // 使用Boost的随机数生成器生成UUID
    boost::uuids::random_generator gen;
    boost::uuids::uuid uuid = gen();

    // 打印生成的UUID
    std::cout << "Generated UUID: " << uuid << std::endl;

    std::string original = boost::uuids::to_string(uuid);
    original.erase(std::remove(original.begin(), original.end(), '-'), original.end());
    std::cout << "original UUID: " << original << std::endl;
    char base64_uuid[50];
    // char decoded_uuid[33];
    // // Base64编码
    // std::string encodedData = base64Encode(original);
    // std::cout << "Encoded data: " << encodedData << std::endl;

    // // Base64解码
    // std::string decodedData = base64Decode(encodedData);
    // std::cout << "Decoded data: " << decodedData << std::endl;

    // // if (decodedData.compare(original) != 0)
    // if (strcmp(original.c_str(), decodedData.c_str()))
    // {
    //     std::cout << "Decoded err, buf disagree : " << original << std::endl;
    // }

    // 将十进制字符串转换为64进制字符串
    // std::string decimal_str = "1234567890";
    hex_uuid_to_base64(original.c_str(), base64_uuid);
    std::cout << "64进制字符串: " << base64_uuid << std::endl;
    return 0;

    printf("Source UUID : %s\n", original.c_str());

    hex_uuid_to_base64(original.c_str(), base64_uuid);
    printf("Base64 UUID : %s\n", base64_uuid);

    // hex_uuid_to_base64x(hex_uuid, base64_uuid);
    // printf("Base64 UUID: %s\n", base64_uuid);

    // base64_to_hex_uuid(base64_uuid, decoded_uuid);
    // printf("Decoded UUID: %s\n", decoded_uuid);

    return 0;
}