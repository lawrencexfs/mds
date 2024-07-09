#include <czmq.h>

int main(void)
{
    // 创建一个 PUSH 套接字并绑定到端口 5555
    zsock_t *push = zsock_new_push("tcp://10.0.20.2:5551");
    assert(push);

    while (!zsys_interrupted)
    {
        // 发送消息
        const char *message = "Hello, World";
        printf("push: %s\n", message);
        zstr_send(push, message);
        zclock_sleep(1000); // 每秒发送一次消息
    }

    zsock_destroy(&push);
    return 0;
}
