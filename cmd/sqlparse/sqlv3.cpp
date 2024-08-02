#include <stdio.h>
#include <pg_query.h>
// g++ sqlv3.cpp -o demo -g -lsqlparser -lpg_query -lm

int main()
{
    // 初始化库
    pg_query_init();

    // 定义一个 SQL 查询
    const char *sql = "select b.id,b.title,u.name FROM blog b left join  user u on u.id=b.user_id;";

    // 解析 SQL 查询
    PgQueryParseResult result = pg_query_parse(sql);

    // 检查解析结果
    if (result.error)
    {
        // 打印错误信息
        printf("Parsing failed: %s\n", result.error->message);
    }
    else
    {
        // 打印解析树
        printf("Parsed successfully:\n %s\n", result.parse_tree);
    }

    // 释放解析结果
    pg_query_free_parse_result(result);

    // 清理库
    pg_query_exit();

    return 0;
}
