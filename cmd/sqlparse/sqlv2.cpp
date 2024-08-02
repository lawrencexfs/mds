#include <iostream>
#include "hsql/SQLParser.h"
#include "hsql/util/sqlhelper.h"
// g++ sqlv2.cpp -o demo -g -lsqlparser

int main()
{
    // SQL 查询语句
    // std::string sql = "SELECT * FROM users WHERE age > 30";
    std::string sql = "SELECT id, name, x1, x2, x3 FROM users WHERE age > 18;select b.id,b.title,u.name from blog b left join  user u on u.id=b.user_id;";
    // std::string sql = "SELECT * FROM users WHERE age > 18;";
    // std::string sql = "insert into personi(id,name,age,phone,address) values(1, \'yang\', 22, \'123232323\', \'中国上海\');";
    // std::string sql = "update personu set address=\'浙江杭州\'; ";
    // std::string sql = "delete FROM persond where id = 1;";
    // std::string sql = "select b.id,b.title,u.name from blog b left join  user u on u.id=b.user_id;";

    // 解析 SQL 语句
    // parse a given query
    hsql::SQLParserResult result;
    hsql::SQLParser::parse(sql, &result);

    // check whether the parsing was successful

    if (result.isValid())
    {
        printf("Parsed successfully!\n");
        printf("Number of statements: %lu\n", result.size());

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
                    break;

                case hsql::kTableJoin:
                {
                    auto joinleft = xx->fromTable->join->left;
                    std::cout << "joinleft : " << joinleft->name << std::endl;
                    auto joinright = xx->fromTable->join->right;
                    std::cout << "joinright : " << joinright->name << std::endl;
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
        return 0;
    }
    else
    {
        fprintf(stderr, "Given string is not a valid SQL query.\n");
        fprintf(stderr, "%s (L%d:%d)\n",
                result.errorMsg(),
                result.errorLine(),
                result.errorColumn());
        return -1;
    }
    return 0;
}
