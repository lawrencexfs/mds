#ifndef _COMPRESS_H
#define _COMPRESS_H

#include <string>

class CompressBase
{
public:
    CompressBase() = default;  // Allow default constructor
    ~CompressBase() = default; // Allow default destructor

protected:
    CompressBase(const CompressBase &) = delete;            // Disallow copy constructor
    CompressBase &operator=(const CompressBase &) = delete; // Disallow assignment operator

    CompressBase(CompressBase &&) = delete;            // Disallow move constructor
    CompressBase &operator=(CompressBase &&) = delete; // Disallow move assignment operator

public:
    /**
     * @description: compress string
     * @param {string} &input original string
     * @return {*}
     */
    virtual std::string compressData(const std::string &input) = 0;
    /**
     * @description: decompress string
     * @param {string} &input original string
     * @return {*}
     */
    virtual std::string decompressData(const std::string &input) = 0;
};

class CompressLZ4 : public CompressBase
{
public:
    /**
     * @description: compress string
     * @param {string} &input original string
     * @return {*}
     */
    std::string compressData(const std::string &input);
    /**
     * @description: decompress string
     * @param {string} &input original string
     * @return {*}
     */
    std::string decompressData(const std::string &input);
};

class CompressGZIP : public CompressBase
{
public:
    /**
     * @description: compress string
     * @param {string} &input original string
     * @return {*}
     */
    std::string compressData(const std::string &input);
    /**
     * @description: decompress string
     * @param {string} &input original string
     * @return {*}
     */
    std::string decompressData(const std::string &input);
};

#endif