/*
 * This file is part of the software and assets of HK ZXOUD LIMITED.
 * @copyright (c) HK ZXOUD LIMITED https://www.zxoud.com
 * Author: yushou-cell(email:2354739167@qq.com)
 * create: 20240620
 * FilePath: /zmqhandle/head/system.hpp
 * Description: some system tools
 */
#pragma once
#include <pqxx/pqxx>
#include <czmq.h>
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>
#include <libunwind.h>
#include <hiredis/hiredis.h>
#include <format>
#include <unordered_map>
#include <utility>
#include <variant>
#include <mimalloc-override.h>
#include "nlohmann/json.hpp"
#define WORKER_READY "\001"

class NonCopyable
{
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable &operator=(const NonCopyable &) = delete;
    NonCopyable(NonCopyable &&) = delete;
    NonCopyable &operator=(NonCopyable &&) = delete;
};

struct RedisReplyWrap : NonCopyable
{
    redisReply *_reply = nullptr;
    inline ~RedisReplyWrap()
    {
        if (_reply != nullptr)
        {
            freeReplyObject(_reply);
            _reply = nullptr;
        }
    }
    inline RedisReplyWrap(RedisReplyWrap &&temp)
    {
        _reply = temp._reply;
        temp._reply = nullptr;
    }
    inline RedisReplyWrap(redisReply *reply)
        : _reply(reply)
    {
    }
    inline RedisReplyWrap()
    {
    }
};