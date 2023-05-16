// 头文件————————————————————
#include <sys/ipc.h> // ftok()
#include <stdio.h>   // perror()、printf()
#include <stdlib.h>  // exit()
#include <sys/msg.h> // msgget()、msgsnd()
#include <string.h>  // strcpy()

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
    key_t mq_key;                   // 消息队列的键（外部名）
    int mq_id;                      // 消息队列的标识（内部名）
    int mq_flag = IPC_CREAT | 0666; // 消息队列的标志：创建，用户、组和其他可读写
    struct Message msg;             // 消息
    const long msg_type = 1;        // 消息的类型

    // 创建消息队列的键（外部名）
    if ((mq_key = ftok(".", 'k')) == ((key_t)-1))
    {
        perror("ftok() error");
        exit(EXIT_FAILURE);
    }

    // 创建消息队列
    if ((mq_id = msgget(mq_key, mq_flag)) == -1)
    {
        perror("msgget() error");
        exit(EXIT_FAILURE);
    }

    // 初始化消息
    msg.mtype = msg_type;
    strcpy(msg.mtext, "message");

    printf("Send message:\n");
    printf("%s\n", msg.mtext);
    // 发送消息
    if ((msgsnd(mq_id, &msg, sizeof(msg.mtext), 0)) == -1)
    {
        perror("msgsnd() error");
        exit(EXIT_FAILURE);
    }

    return 0;
}