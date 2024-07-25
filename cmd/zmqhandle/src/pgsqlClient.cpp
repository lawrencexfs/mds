/*
 * This file is part of the software and assets of HK ZXOUD LIMITED.
 * @copyright (c) HK ZXOUD LIMITED https://www.zxoud.com
 * Author: yushou-cell(email:2354739167@qq.com)
 * create: Do not edit
 * FilePath: /zmqhandle/src/pgsqlClient.cpp
 * Description:PostgreSQL client command exec
 */

#include "pgsqlClient.hpp"
#include <iostream>

PgsqlClient::PgsqlClient(std::string view)
    : _conn(view)
{
    _connectStr = view;
    if (_conn.is_open())
    {
        std::cout << "DB connect success! str is " << _connectStr << std::endl;
    }
    else
    {
        std::cout << "DB connect fail! str is " << _connectStr << std::endl;
    }
}

PgsqlClient::~PgsqlClient()
{
}

pqxx::result PgsqlClient::execCommandOneSql(std::string_view view)
{
    pqxx::result res;                 // 创建一个 pqxx::result 对象，用于存储查询结果
    pqxx::nontransaction work(_conn); // 创建一个非事务性的工作对象，使用给定的连接 _conn
    res = work.exec(view);            // 在工作对象上执行 SQL 查询，并将结果赋值给 res
    return res;                       // 返回查询结果
}

bool PgsqlClient::sqlExec(std::string cmd, pqxx::result &reply)
{
    bool ret = false;
    // try exec a command to pgsql, if error, throw command error and others
    try
    {
        reply = execCommandOneSql(cmd);
        ret = true;
    }
    catch (const pqxx::sql_error &e)
    {
        _err = e.what();
        std::cout << "SQL error: " << e.what() << std::endl;
        std::cout << "Query was: " << e.query() << std::endl;
        ret = false;
    }
    catch (const std::exception &e)
    {
        _err = e.what();
        std::cout << "Error: " << e.what() << std::endl;
        ret = false;
    }
    return ret;
}

std::string PgsqlClient::Error()
{
    return _err;
}

// 将 pqxx::result 转换为 JSON 字符串的函数
nlohmann::json pqxx_result_to_json(const pqxx::result &r)
{
    // 创建一个 JSON 数组来存储所有行
    nlohmann::json j_array = nlohmann::json::array();
    // 遍历结果集中的每一行
    for (const auto &row : r)
    {
        // 对于每一行，创建一个 JSON 对象
        nlohmann::json j_object = nlohmann::json::object();
        // 遍历行中的所有列
        for (const auto &field : row)
        {
            // 将列值添加到 JSON 对象中
            // 这里假设字段值是字符串，根据实际情况可能需要转换类型
            j_object[field.name()] = field.as<std::string>();
        }
        // 将 JSON 对象添加到数组中
        j_array.push_back(j_object);
    }
    return j_array;
};
