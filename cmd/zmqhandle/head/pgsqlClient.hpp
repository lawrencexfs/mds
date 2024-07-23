/*
 * This file is part of the software and assets of HK ZXOUD LIMITED.
 * @copyright (c) HK ZXOUD LIMITED https://www.zxoud.com
 * Author: yushou-cell(email:2354739167@qq.com)
 * create: 20240626
 * FilePath: /zmqhandle/head/pgsqlClient.hpp
 * Description: PostgreSQL client command exec
 */
#pragma once
#include "system.hpp"

class PgsqlClient
{
private:
    std::string _connectStr;
    pqxx::connection _conn;

public:
    PgsqlClient(std::string view);
    ~PgsqlClient();
    /**
     * @description: exec one SQL command to PostgreSQL server
     * @param {string_view} view: one SQL statements
     * @return {pqxx::result} a row data of PostgreSQL reply
     */
    pqxx::result execCommandOneSql(std::string_view view);
    /**
     * @description:
     * @param {string} cmd
     * @param {result} &reply
     * @return {*}
     */
    bool sqlExec(std::string cmd, pqxx::result &reply);
};

nlohmann::json pqxx_result_to_json(const pqxx::result &r);
