#include "bigint.h"
#include <vector>
#include <algorithm> // 包含std::reverse
#include <iostream>
#include <chrono>

// using namespace std;

#ifndef MIN
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline bool isDigit(const char ch)
{
    return ch >= '0' && ch <= '9';
}

LargeInt::LargeInt() {}

LargeInt::LargeInt(uint val)
{
    this->_data.push_back(val % MAX_VAL);
    if (val >= MAX_VAL)
        this->_data.push_back(val / MAX_VAL);
}

// 字符串合法性检查，只允许出现字符 0~9
bool LargeInt::checkValStr(const std::string &valStr)
{
    for (auto it = valStr.begin(); it != valStr.end(); ++it)
    {
        if (!isDigit(*it))
            return false;
    }
    return true;
}

// 字符串构造
//  valStr：十进制字符串
LargeInt::LargeInt(const std::string &valStr)
{
    if (checkValStr(valStr))
    {
        int len = valStr.length();
        // 按长度9截取子串
        while (len >= VAL_LEN)
        {
            std::string s = valStr.substr(len - VAL_LEN, VAL_LEN);
            this->_data.push_back(std::stoi(s));
            len -= VAL_LEN;
        }
        // 残留子串
        if (len > 0)
        {
            std::string s = valStr.substr(0, len);
            this->_data.push_back(std::stoi(s));
        }
    }

    this->arrange(); // 去零
}

// 去零操作，避免整数队列的高位存在多余的零
void LargeInt::arrange()
{
    int idx = this->_data.size();

    // 注意，如果队列中全为0，要保留最低位的0
    while (--idx >= 1)
    {
        if (this->_data[idx] > 0)
            break;

        this->_data.pop_back();
    }
}

// 比较函数，0 相等，1 大于，-1 小于
int LargeInt::compare(const LargeInt &li) const
{
    int len1 = this->_data.size();
    int len2 = li._data.size();

    // step1: 比较长度
    if (len1 != len2)
        return (len1 > len2) ? 1 : -1;

    // step2: 由高位到低位比较值
    for (int idx = len1 - 1; idx >= 0; --idx)
    {
        if (this->_data[idx] == li._data[idx])
            continue;
        return this->_data[idx] > li._data[idx] ? 1 : -1;
    }

    return 0;
}

bool LargeInt::operator==(const LargeInt &li) const
{
    return compare(li) == 0;
}

bool LargeInt::operator!=(const LargeInt &li) const
{
    return compare(li) != 0;
}

bool LargeInt::operator<(const LargeInt &li) const
{
    return compare(li) < 0;
}

bool LargeInt::operator>(const LargeInt &li) const
{
    return compare(li) > 0;
}

bool LargeInt::operator<=(const LargeInt &li) const
{
    return compare(li) <= 0;
}

bool LargeInt::operator>=(const LargeInt &li) const
{
    return compare(li) >= 0;
}

LargeInt LargeInt::operator+(const LargeInt &li) const
{
    int len1 = this->_data.size();
    int len2 = li._data.size();

    int minLen = MIN(len1, len2);
    int maxLen = MAX(len1, len2);
    const LargeInt &extraLi = (len1 > len2) ? (*this) : li;

    uint value = 0; // 和，不超过最大值的2倍
    uint carry = 0; // 进位
    LargeInt retVal;

    for (int idx = 0; idx < minLen; ++idx)
    {
        value = this->_data[idx] + li._data[idx] + carry;

        if (value >= MAX_VAL)
        {
            retVal._data.push_back(value - MAX_VAL);
            carry = 1;
        }
        else
        {
            retVal._data.push_back(value);
            carry = 0;
        }
    }

    for (int idx = minLen; idx < maxLen; ++idx)
    {
        value = extraLi._data[idx] + carry;

        if (value >= MAX_VAL)
        {
            retVal._data.push_back(value - MAX_VAL);
            carry = 1;
        }
        else
        {
            retVal._data.push_back(value);
            carry = 0;
        }
    }

    if (carry > 0)
        retVal._data.push_back(carry);

    // retVal.arrange(); // 去0操作
    return retVal;
}

LargeInt LargeInt::operator-(const LargeInt &li) const
{
    if (*this <= li)
    {
        return LargeInt(0);
    }

    int len1 = this->_data.size();
    int len2 = li._data.size();

    uint value = 0; // 差
    uint carry = 0; // 借位
    LargeInt retVal;

    for (int idx = 0; idx < len2; ++idx)
    {
        if (this->_data[idx] < li._data[idx] + carry) // 注意细节，carry放在右侧，避免出现差值为负数的情况
        {
            value = this->_data[idx] + MAX_VAL - carry - li._data[idx];
            carry = 1;
        }
        else
        {
            value = this->_data[idx] - carry - li._data[idx];
            carry = 0;
        }

        retVal._data.push_back(value);
    }

    for (int idx = len2; idx < len1; ++idx)
    {
        if (this->_data[idx] < carry)
        {
            value = this->_data[idx] + MAX_VAL - carry;
            carry = 1;
        }
        else
        {
            value = this->_data[idx] - carry;
            carry = 0;
        }
        retVal._data.push_back(value);
    }

    retVal.arrange();
    return retVal;
}

LargeInt LargeInt::operator*(const LargeInt &li) const
{
    int len1 = this->_data.size();
    int len2 = li._data.size();

    if (len1 < len2)
        return li.operator*(*this); // 优化，保证被乘数位数大于乘数

    uint64 value;     // 积
    uint64 carry = 0; // 进位
    LargeInt retVal(0);
    LargeInt mulTemp;

    for (int idx2 = 0; idx2 < len2; ++idx2)
    {
        mulTemp._data.clear();
        carry = 0;

        // 补零
        for (int tmpIdx = 0; tmpIdx < idx2; ++tmpIdx)
            mulTemp._data.push_back(0);

        for (int idx1 = 0; idx1 < len1; ++idx1)
        {
            value = (uint64)(li._data[idx2]) * (uint64)(this->_data[idx1]) + carry;

            mulTemp._data.push_back((uint)(value % MAX_VAL));
            carry = value / MAX_VAL;
        }

        if (carry)
            mulTemp._data.push_back((uint)carry);

        retVal = retVal + mulTemp;
    }

    return retVal;
}

LargeInt LargeInt::operator/(const LargeInt &li) const
{
    if (li._data.empty() || li == 0)
        return LargeInt("");
    if (*this < li)
        return LargeInt(0);

    int len1 = this->_data.size();
    int len2 = li._data.size();

    uint value;
    LargeInt retVal;
    LargeInt divTemp;

    for (int idx = len1 - len2 + 1; idx < len1; ++idx)
    {
        divTemp._data.push_back(this->_data[idx]);
    }

    // len1 >= len2
    for (int idx = len1 - len2; idx >= 0; --idx)
    {
        divTemp._data.insert(divTemp._data.begin(), this->_data[idx]);
        divTemp.arrange();

        value = getMaxCycle(divTemp, li); // 商

        divTemp = divTemp - li * value; // 余数

        retVal._data.insert(retVal._data.begin(), value); // 除法是由高位向低位进行，所以插入位置在begin
    }

    retVal.arrange();
    return retVal;
}

std::string LargeInt::toString() const
{
    int len = this->_data.size();
    int shift = 0;
    char *buff = new char[len * VAL_LEN + 1];

    if (len > 0)
        shift += sprintf(buff + shift, "%d", this->_data[len - 1]);

    for (int idx = len - 2; idx >= 0; --idx)
    {
        shift += sprintf(buff + shift, FORMAT_STR, this->_data[idx]);
    }
    buff[shift] = '\0';

    std::string retStr(buff);
    delete[] buff;

    return retStr;
}

int LargeInt::toInt() const
{
    return std::stoi(this->toString());
}

std::string LargeInt::toBase64String() const
{
    // std::vector<char> dst;
    std::string dst;

    LargeInt src = *this;
    do
    {
        LargeInt tp = src / 64;
        LargeInt div = src - tp * 64;
        char value = base64_chars[div.toInt()];
        dst.push_back(value);
        src = tp;
    } while (src > 0);
    std::reverse(dst.begin(), dst.end());
    return dst;
}

// 计算商值
uint LargeInt::getMaxCycle(const LargeInt &liA, const LargeInt &liB)
{
    LargeInt tempA = liA;
    const LargeInt &tempB = liB;
    uint tempC;
    uint res = 0;
    bool flag = true;

    while (tempA >= tempB)
    {
        tempC = estimateQuotient(tempA, tempB);
        tempA = tempB * tempC - tempA;

        res = flag ? (res + tempC) : (res - tempC);
        flag = !flag;
    }

    // 微调
    while (res > 0 && liB * res > liA)
        res--;

    return res;
}

// 估值
uint LargeInt::estimateQuotient(const LargeInt &liA, const LargeInt &liB)
{
    int lenA = liA._data.size();
    int lenB = liB._data.size();
    uint64 valA, valB;

    if (lenA == lenB)
    {
        if (lenA > 1)
        {
            valA = (uint64)liA._data[lenA - 1] * MAX_VAL + liA._data[lenA - 2];
            valB = (uint64)liB._data[lenB - 1] * MAX_VAL + liB._data[lenB - 2];
        }
        else
        {
            valA = (uint64)liA._data[lenA - 1];
            valB = (uint64)liB._data[lenB - 1];
        }
    }
    else
    {
        valA = (uint64)liA._data[lenA - 1] * MAX_VAL + liA._data[lenA - 2];
        valB = (uint64)liB._data[lenB - 1];
    }

    return (uint)(valA / valB);
}
#define ASSERT_TRUE(condition)                                        \
    if (!(condition))                                                 \
    {                                                                 \
        std::cerr << "Assertion failed: " << #condition << std::endl; \
        std::exit(1);                                                 \
    }

int main(int argc, char **argv)
{
    // ASSERT_TRUE(LargeInt().toString() == "");
    // ASSERT_TRUE(LargeInt(123456789).toString() == "123456789");
    // ASSERT_TRUE(LargeInt(1234567890).toString() == "1234567890");
    // ASSERT_TRUE(LargeInt("").toString() == "");
    // ASSERT_TRUE(LargeInt("123456789").toString() == "123456789");
    // // ASSERT_TRUE(LargeInt("01234567890123456789").toString() == "1234567890123456781");

    // ASSERT_TRUE(LargeInt(1) * LargeInt(2) == LargeInt(2));
    // ASSERT_TRUE(LargeInt(1234567890) * LargeInt(987654) == LargeInt("1219325914830060"));
    // ASSERT_TRUE(LargeInt(987654) * LargeInt(1234567890) == LargeInt("1219325914830060"));

    // ASSERT_TRUE(LargeInt("1219325914830060") / LargeInt(1234567890) == LargeInt(987654));
    // ASSERT_TRUE(LargeInt("1219325914830060") / LargeInt(987654) == LargeInt(1234567890));
    // ASSERT_TRUE(LargeInt("1219325914830060") / LargeInt("1") == LargeInt("1219325914830060"));
    // ASSERT_TRUE(LargeInt("315649816498164761562056488065489561036849876300254661") / LargeInt("0") == LargeInt(""));
    // ASSERT_TRUE(LargeInt("315649816498164761562056488065489561036849876300254661") / LargeInt("1") == LargeInt("315649816498164761562056488065489561036849876300254661"));
    // ASSERT_TRUE(LargeInt("") / LargeInt("") == LargeInt(""));
    // ASSERT_TRUE(LargeInt("56") / LargeInt("64") == LargeInt("0"));

    // // LargeInt origin("315649816498164761562056488065489561036849876300254661");
    // LargeInt origin("315649816498164761562056488065489561036849876300254661");
    // std::cout << "origin 字符串: " << origin.toString() << std::endl;
    // std::cout << "base64 字符串: " << origin.toBase64String() << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for (auto i = 0; i < 100000; ++i)
    {
        LargeInt origin("315649816498164761562056488065489561036849876300254661");
        // std::cout << "origin 字符串: " << origin.toString() << std::endl;
        // std::cout << "base64 字符串: " << origin.toBase64String() << std::endl;
        auto temp = origin.toBase64String();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << std::endl
              << "Time taken: " << duration.count() << " milliseconds" << std::endl;

    return 0;
}
