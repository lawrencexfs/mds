/*
 * This file is part of the software and assets of HK ZXOUD LIMITED.
 * @copyright (c) HK ZXOUD LIMITED https://www.zxoud.com
 * Author: yushou-cell(email:2354739167@qq.com)
 * create: 20240620
 * FilePath: /zmqhandle/src/redisClient.cpp
 * Description: redis client command exec
 */
#include "redisClient.hpp"
#include <iostream>

RedisClient::RedisClient(const nlohmann::json &value)
{
    if (value.find("MemoryDataBase") == value.end())
    {
        std::cout << "config.json not config MemoryDataBase info" << std::endl;
    }
    auto value2 = value.find("MemoryDataBase").value();
    if (value2.find("Redis") == value2.end())
    {
        std::cout << "config.json not config redis info" << std::endl;
    }
    auto value3 = value2.find("Redis").value();

    _connectIP = value3.find("RedisConnectIP").value().get<std::string>();
    _port = value3.find("RedisConnectPort").value().get<int>();
    connect();
}

RedisClient::RedisClient(std::string connectIP, int port) : _connectIP(connectIP), _port(port)
{
    connect();
}

RedisClient::~RedisClient()
{
    if (_client != nullptr)
    {
        // redisFree(_client);
        delete _client;
        _client = nullptr;
    }
}

bool RedisClient::connect()
{
    sw::redis::ConnectionOptions connection_options;
    connection_options.host = _connectIP; // Required.
    connection_options.port = _port;      // Optional. The default port is 6379.
    // connection_options.password = "auth";   // Optional. No password by default.
    connection_options.db = 0; // Optional. Use the 0th database by default.

    sw::redis::ConnectionPoolOptions pool_options;
    pool_options.size = 10; // Pool size, i.e. max number of connections.
    pool_options.wait_timeout = std::chrono::milliseconds(100);

    // sw::redis::RedisCluster *redisofDB1 = NULL;
    try
    {
        _client = new sw::redis::AsyncRedisCluster(connection_options, pool_options);
    }
    catch (const sw::redis::ReplyError &err)
    {
        printf("RedisHandler-- ReplyError：%s \n", err.what());
        return false;
    }
    catch (const sw::redis::TimeoutError &err)
    {
        printf("RedisHandler-- TimeoutError%s \n", err.what());
        return false;
    }
    catch (const sw::redis::ClosedError &err)
    {
        printf("RedisHandler-- ClosedError%s \n", err.what());
        return false;
    }
    catch (const sw::redis::IoError &err)
    {
        printf("RedisHandler-- IoError%s \n", err.what());
        return false;
    }
    catch (const sw::redis::Error &err)
    {
        printf("RedisHandler-- other%s \n", err.what());
        return false;
    }
    std::cout << "redis connect success! addr is " << _connectIP << ":" << _port << std::endl;
    return true;
}

void RedisClient::exeSetCommand(RedisReplyWrap &reply, const std::string &key, std::string value)
{
    try
    {
        // auto ret = _client->set(key, value);
        // reply._ok = true;
        // std::future<bool> ret = _client->set(key, value);
        // bool setResult = ret.get();
        // Async interface with callback.
        _client->set(key, value,
                     [](std::future<bool> &&fut)
                     {
                         try
                         {
                             auto set_res = fut.get();
                         }
                         catch (const std::exception &err)
                         {
                             // handle error
                         }
                     });

        reply._ok = true;
    }
    catch (const sw::redis::ReplyError &err)
    {
        // WRONGTYPE Operation against a key holding the wrong kind of value
        std::cout << err.what() << std::endl;
    }
    catch (const sw::redis::TimeoutError &err)
    {
        // reading or writing timeout
        std::cout << err.what() << std::endl;
    }
    catch (const sw::redis::ClosedError &err)
    {
        // the connection has been closed.
        std::cout << err.what() << std::endl;
    }
    catch (const sw::redis::IoError &err)
    {
        // there's an IO error on the connection.
        std::cout << err.what() << std::endl;
    }
    catch (const sw::redis::Error &err)
    {
        // other errors
        std::cout << err.what() << std::endl;
    }
    catch (const std::exception &err)
    {
        // 如果以上错误类型都不捕捉，直接捕捉 excepion
        std::cout << "std::exception : " << err.what() << std::endl;
    }
}

void RedisClient::handleSelect(const std::string &order, std::string object)
{
    RedisReplyWrap replyTemp;
    // std::regex re(" ");
    // auto s = std::regex_replace(str, re, "");
    // std::string cmd = std::format("set '{0}' '{1}' ", s, object);
    // std::string cmd = std::format("set %b %b ", order, object);
    exeSetCommand(replyTemp, order, object);
    if (replyTemp._ok)
    {
        // ok
    }
    else
    {
        std::cout << "redis err is " << std::endl;
    }
}
void RedisClient::handleNoSelect(const std::string &order)
{
    RedisReplyWrap replyTemp;
    // std::string cmd = std::format("set '{0}' {1}", order, 1);
    // exeCommand(replyTemp, cmd);
    exeSetCommand(replyTemp, order, "1");
    if (replyTemp._ok)
    {
        // ok
    }
    else
    {
        std::cout << "redis err is " << std::endl;
    }
}

void RedisClient::execSqlFail(const std::string &order, std::string err)
{
    RedisReplyWrap replyTemp;
    exeSetCommand(replyTemp, order, err);
    if (replyTemp._ok)
    {
        // ok
    }
    else
    {
        std::cout << "redis err is " << std::endl;
    }
}

std::string RedisClient::get(const std::string &key)
{
    std::string result;
    try
    {
        std::future<sw::redis::OptionalString> ret = _client->get(key);
        auto value = ret.get();
        if (value)
        {
            result = *value;
        }
        else
        {
            std::cout << "Key not found or error occurred" << std::endl;
        }
    }
    catch (const std::exception &err)
    {
        // 如果以上错误类型都不捕捉，直接捕捉 excepion
        std::cout << "std::exception : " << err.what() << std::endl;
    }
    return result;
}

std::string CUtil::printTrace()
{
    unw_cursor_t cursor;
    unw_context_t context;
    std::string temp;
    // init context
    unw_getcontext(&context);
    unw_init_local(&cursor, &context);
    // traverse the call stack
    while (unw_step(&cursor) > 0)
    {
        unw_word_t offset, pc;
        char fname[64];

        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        fname[0] = '\0';
        unw_get_proc_name(&cursor, fname, sizeof(fname), &offset);
        temp.append(fname);
        temp.append("() ");
        temp.append("+0x");
        temp.append(std::to_string(offset));
        temp.append("\n");
    }
    return temp;
}