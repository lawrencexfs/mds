#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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

// 将16进制UUID转换为64进制编码
void hex_uuid_to_base64x(const char *hex_uuid, char *base64_uuid)
{
    uint8_t binary_uuid[32];
    size_t i, j;

    // 将16进制UUID转换为二进制数据
    for (i = 0, j = 0; i < 64; i += 2, j++)
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

#include <stdio.h>
#include <limits.h>

int main()
{
    int xx = -1;
    printf("int 最大值: %d\n", INT_MAX);
    printf("int sizeof长度: %d\n", sizeof(xx));
    return 0;
    const char *hex_uuid = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
    char base64_uuid[50];
    // char decoded_uuid[33];

    printf("Source UUID: %s\n", hex_uuid);

    // hex_uuid_to_base64(hex_uuid, base64_uuid);
    // printf("Base64 UUID: %s\n", base64_uuid);

    // hex_uuid_to_base64x(hex_uuid, base64_uuid);
    // printf("Base64 UUID: %s\n", base64_uuid);

    // base64_to_hex_uuid(base64_uuid, decoded_uuid);
    // printf("Decoded UUID: %s\n", decoded_uuid);

    return 0;
}