#ifndef _BIGINT_H
#define _BIGINT_H

#include <iostream>
#include <vector>
#include <string>

typedef unsigned int uint;
typedef unsigned long long uint64;

#define MAX_VAL 1000000000 // 10亿
#define VAL_LEN 9
#define FORMAT_STR "%09d"

class LargeInt
{
private:
    std::vector<uint> _data;

    bool checkValStr(const std::string &valStr);
    int compare(const LargeInt &li) const;
    void arrange();

    static uint estimateQuotient(const LargeInt &liA, const LargeInt &liB);
    static uint getMaxCycle(const LargeInt &liA, const LargeInt &liB);

public:
    LargeInt();
    LargeInt(uint val);
    LargeInt(const std::string &valStr);

    // 四则运算符重载
    LargeInt operator+(const LargeInt &li) const;
    LargeInt operator-(const LargeInt &li) const;
    LargeInt operator*(const LargeInt &li) const;
    LargeInt operator/(const LargeInt &li) const;

    // 比较运算符重载
    bool operator==(const LargeInt &li) const;
    bool operator!=(const LargeInt &li) const;
    bool operator<(const LargeInt &li) const;
    bool operator>(const LargeInt &li) const;
    bool operator<=(const LargeInt &li) const;
    bool operator>=(const LargeInt &li) const;

    // 整形值输出
    int toInt() const;
    // 字符串格式化输出
    std::string toString() const;

    // BASE64字符串格式化输出
    std::string toBase64String() const;
};

#endif