#ifndef _ORDER_H
#define _ORDER_H

class Singleton
{
public:
    // 禁止拷贝和赋值
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    // 提供全局访问点
    static Singleton &getInstance()
    {
        static Singleton instance; // 局部静态变量
        return instance;
    }

public:
    // 其他公共成员函数
    void doSomething()
    {
        // 实现
    }

private:
    // 私有构造函数，防止外部实例化
    Singleton()
    {
        // 初始化
    }

    // 私有析构函数，防止外部删除实例
    ~Singleton()
    {
        // 清理
    }
};
#endif

// 使用
// Singleton &singleton = Singleton::getInstance();