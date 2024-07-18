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
    pqxx::result res;
    pqxx::nontransaction work(_conn);
    res = work.exec(view);
    return res;
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
        std::cout << "SQL error: " << e.what() << std::endl;
        std::cout << "Query was: " << e.query() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
    return ret;
}