// 头文件————————————————————
#include <sys/ipc.h> // ftok()
#include <stdio.h>   // perror()、printf()
#include <stdlib.h>  // exit()
#include <sys/msg.h> // msgget()、msgrcv()、msgctl()

// 结构体————————————————————
// 消息
struct Message
{
    long mtype;    // 类型
    char mtext[8]; // 内容
};

// 主函数————————————————————
int main(int argc, char *argv[])
{
    key_t mq_key;            // 消息队列的键（外部名）
    int mq_id;               // 消息队列的标识（内部名）
    int mq_flag = 0666;      // 消息队列的标志：用户、组和其他可读写————注意：与发送端不同
    struct Message msg;      // 消息
    const long msg_type = 1; // 消息的类型

    // 创建消息队列的键（外部名）
    if ((mq_key = ftok(".", 'k')) == ((key_t)-1))
    {
        perror("ftok() error");
        exit(EXIT_FAILURE);
    }

    // 获取消息队列
    if ((mq_id = msgget(mq_key, mq_flag)) == -1)
    {
        perror("msgget() error");
        exit(EXIT_FAILURE);
    }

    // 接收消息
    if ((msgrcv(mq_id, &msg, sizeof(msg.mtext), msg_type, 0)) == -1)
    {
        perror("msgrcv() error");
        exit(EXIT_FAILURE);
    }

    printf("Receive message:\n");
    // 将消息写入标准输出
    printf("%s\n", msg.mtext);

    // 删除消息队列
    if ((msgctl(mq_id, IPC_RMID, NULL)) == -1)
    {
        perror("msgctl() error");
        exit(EXIT_FAILURE);
    }

    return 0;
}