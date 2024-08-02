#include <iostream>
#include <string>
#include <regex>
#include "hsql/SQLParser.h"
#include "hsql/util/sqlhelper.h"
// g++ sql.cpp -o demo -g -lsqlparser -lpg_query -lm

struct SQLParserObject
{
    std::string _action;
    std::vector<std::string> _tablename;
};

// select 命令必须小写传入
void parse_sql(const std::string &sql, SQLParserObject &obj)
{
    std::regex action_regex(R"(\b(\w+)\s+)");
    std::regex table_regex(R"(\bFROM\s+(\w+))");
    std::regex table_regex_insert(R"(\binto\s+(\w+))");
    std::regex table_regex_update(R"(\bupdate\s+(\w+))");

    std::regex field_regex(R"(\bSELECT\s+(.*?)\s+FROM)");

    std::smatch matches;

    if (std::regex_search(sql, matches, action_regex))
    {
        std::string fields_str = matches[1];
        obj._action = fields_str;
        std::cout << "Action: " << matches[1] << std::endl;
        if (fields_str.find("select") != std::string::npos)
        {
            hsql::SQLParserResult result;
            hsql::SQLParser::parse(sql, &result);

            // check whether the parsing was successful
            if (result.isValid())
            {
                // printf("Parsed successfully!\n");
                // printf("Number of statements: %lu\n", result.size());

                for (auto i = 0u; i < result.size(); ++i)
                {
                    // Print a statement summary.
                    // hsql::printStatementInfo(result.getStatement(i));
                    auto stmt = result.getStatement(i);
                    switch (result.getStatement(i)->type())
                    {
                    case hsql::kStmtSelect:
                    {
                        // hsql::printSelectStatementInfo((const hsql::SelectStatement *)stmt, 0);
                        auto xx = (hsql::SelectStatement *)stmt;
                        switch (xx->fromTable->type)
                        {
                        case hsql::kTableName:
                            std::cout << "table : " << xx->fromTable->name << std::endl;
                            obj._tablename.push_back(std::string(xx->fromTable->name));
                            break;

                        case hsql::kTableJoin:
                        {
                            auto joinleft = xx->fromTable->join->left;
                            std::cout << "joinleft : " << joinleft->name << std::endl;
                            obj._tablename.push_back(std::string(joinleft->name));

                            auto joinright = xx->fromTable->join->right;
                            std::cout << "joinright : " << joinright->name << std::endl;
                            obj._tablename.push_back(std::string(joinright->name));
                        }
                        // printTableRefInfo(table->join->left, num_indent + 2);
                        // printTableRefInfo(table->join->right, num_indent + 2);
                        break;
                        default:
                            break;
                        }
                    }

                    break;
                    case hsql::kStmtInsert:
                        hsql::printInsertStatementInfo((const hsql::InsertStatement *)stmt, 0);
                        break;
                    case hsql::kStmtCreate:
                        hsql::printCreateStatementInfo((const hsql::CreateStatement *)stmt, 0);
                        break;
                    case hsql::kStmtImport:
                        hsql::printImportStatementInfo((const hsql::ImportStatement *)stmt, 0);
                        break;
                    case hsql::kStmtExport:
                        hsql::printExportStatementInfo((const hsql::ExportStatement *)stmt, 0);
                        break;
                    case hsql::kStmtTransaction:
                        hsql::printTransactionStatementInfo((const hsql::TransactionStatement *)stmt, 0);
                        break;
                    default:
                        break;
                    }
                }
                return;
            }
            else
            {
                // fprintf(stderr, "Given string is not a valid SQL query.\n");
                // fprintf(stderr, "%s (L%d:%d)\n",
                //         result.errorMsg(),
                //         result.errorLine(),
                //         result.errorColumn());
                return;
            }
        }
        else if (fields_str.find("insert") != std::string::npos)
        {
            table_regex = table_regex_insert;
        }
        else if (fields_str.find("update") != std::string::npos)
        {
            table_regex = table_regex_update;
        }
        if (std::regex_search(sql, matches, table_regex))
        {
            std::string fields_str = matches[1];
            obj._tablename.push_back(fields_str);
            std::cout << "Table: " << matches[1] << std::endl;
        }
    }
}

int main()
{
    SQLParserObject obj;
    std::string sql = "select id, name, x1, x2, x3 FROM users WHERE age > 18;";
    // std::string sql = "select * FROM users WHERE age > 18;";
    // std::string sql = "insert into personi(id,name,age,phone,address) values(1, \'yang\', 22, \'123232323\', \'中国上海\');";
    // std::string sql = "update personu set address=\'浙江杭州\'; ";
    // std::string sql = "delete FROM persond where id = 1;";
    // std::string sql = "select b.id,b.title,u.name FROM blog b left join  user u on u.id=b.user_id;";
    // std::string sql = "select id, name, x1, x2, x3 FROM users WHERE age > 18;select b.id,b.title,u.name from blog b left join  user u on u.id=b.user_id;";

    parse_sql(sql, obj);
    return 0;
}