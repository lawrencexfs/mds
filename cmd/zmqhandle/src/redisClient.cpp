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
        redisFree(_client);
        _client = nullptr;
    }
}

bool RedisClient::connect()
{
    _client = redisConnect(_connectIP.c_str(), _port);
    if (_client == nullptr || _client->err)
    {
        std::cout << "redis Connect err! func stack is " << CUtil::printTrace();
        if (_client->err)
        {
            std::cout << "redis Connect err is " << _client->errstr;
        }
        return false;
    }
    std::cout << "redis connect success! addr is " << _connectIP << ":" << _port << std::endl;
    return true;
}

void RedisClient::exeCommand(RedisReplyWrap &reply, const std::string &order)
{
    auto *replyTemp = static_cast<redisReply *>(redisCommand(_client, order.c_str()));
    if (replyTemp == nullptr)
    {
        std::cout << "Failed to execute command: " << _client->errstr << " func stack is " << CUtil::printTrace();
        return;
    }
    reply._reply = replyTemp;
}

void RedisClient::exeSetCommand(RedisReplyWrap &reply, const std::string &key, std::string value)
{
    auto *replyTemp = static_cast<redisReply *>(redisCommand(_client, "SET %b %b", key.c_str(), key.length(), value.c_str(), value.length()));
    if (replyTemp == nullptr)
    {
        std::cout << "Failed to execute command: " << _client->errstr << " func stack is " << CUtil::printTrace();
        return;
    }
    reply._reply = replyTemp;

    if (replyTemp->type == REDIS_REPLY_STRING)
    {
        // 解析字符串类型的结果
        std::string result(replyTemp->str, replyTemp->len);
        // 使用 result 进行后续操作
    }
    else if (replyTemp->type == REDIS_REPLY_INTEGER)
    {
        // 解析整数类型的结果
        int result = replyTemp->integer;
        // 使用 result 进行后续操作
    }
    else if (replyTemp->type == REDIS_REPLY_ARRAY)
    {
        // 解析数组类型的结果
        for (size_t i = 0; i < replyTemp->elements; i++)
        {
            redisReply *element = replyTemp->element[i];
            // 根据 element 的类型进行进一步解析
        }
    }
    else
    {
        // 处理其他类型的结果
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
    if (replyTemp._reply && strcmp(replyTemp._reply->str, "OK") == 0)
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
    if (replyTemp._reply && strcmp(replyTemp._reply->str, "OK") == 0)
    {
        // ok
    }
    else
    {
        std::cout << "redis err is " << std::endl;
    }
}

void RedisClient::startTransaction()
{
    _transactionOrderNum = 0;
    redisAppendCommand(_client, "MULTI");
}

void RedisClient::addCommandToTransaction(const std::string &order)
{
    redisAppendCommand(_client, order.c_str());
    ++_transactionOrderNum;
}

void RedisClient::exeTransaction(std::vector<RedisReplyWrap> &replyArry)
{
    redisAppendCommand(_client, "EXEC");
    RedisReplyWrap temp;
    for (decltype(_transactionOrderNum) i = 0; i < _transactionOrderNum; i++)
    {
        redisGetReply(_client, (void **)(&temp._reply));
        replyArry.emplace_back(temp._reply);
    }
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