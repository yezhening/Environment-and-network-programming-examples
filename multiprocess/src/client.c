// 头文件————————————————————
// #include <sys/socket.h> //socket()、sockaddr、connect()、send()、recv()
#include <stdio.h>  //（perror()）、printf()、（fgets()）
#include <stdlib.h> //exit()
// #include <netinet/in.h> //sockaddr_in、（htons()）
#include <string.h>    //bzero()、strncpy()
#include <arpa/inet.h> //inet_pton()
// #include <unistd.h>     //close()
#include <errno.h> //errno

// 全局常量————————————————————
const g_serv_port = 6666; // 服务端端口号
const g_buff_size = 64;   // 消息缓冲区大小

// 函数声明————————————————————
void handle(int sock_fd); // 处理

// 主函数————————————————————
int main(int argc, char *argv[])
{
    // 网络连接————————————————————
    int sock_fd; // 套接字文件描述符
    // 创建套接字并获取套接字文件描述符
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr; // 服务端网络信息结构体
    // 初始化服务端网络信息结构体
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(g_serv_port);
    if ((inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)) != 1)
    {
        perror("inet_pton() error");
        exit(EXIT_FAILURE);
    }

    // 与服务端建立连接
    if ((connect(sock_fd, (struct sockaddr *)(&serv_addr), sizeof(serv_addr))) == -1)
    {
        if ((close(sock_fd)) == -1)
        {
            perror("connect() close() error");
            exit(EXIT_FAILURE);
        }

        perror("connect() error");
        exit(EXIT_FAILURE);
    }

    handle(sock_fd); // 处理

    // 关闭套接字文件描述符
    if ((close(sock_fd)) == -1)
    {
        perror("close() error");
        exit(EXIT_FAILURE);
    }

    return 0;
}

// 函数定义————————————————————
// 处理
void handle(int sock_fd) // 参数：套接字文件描述符
{
    // 传输消息————————————————————
    char msg_send[g_buff_size]; // 发送到服务端的消息缓冲区
    char msg_recv[g_buff_size]; // 从服务端接收的消息缓冲区
    int recv_bytes;             // 接收的消息字节数

    while (1) // 循环发送和接收消息
    {
        bzero(&msg_send, sizeof(*msg_send));
        bzero(&msg_recv, sizeof(*msg_recv));

        printf("Send message: ");
        if ((fgets(msg_send, g_buff_size, stdin)) == NULL)
        // 从标准输入获取消息。错误或遇到文件结尾(EOF)：在客户端标准输入Ctrl+D，相当于关闭连接
        {
            printf("End of connection\n");
            return; // 函数返回后，关闭套接字文件描述符，结束进程
        }

        if ((send(sock_fd, msg_send, g_buff_size, 0)) == -1) // 发送消息
        {
            perror("send() error");
            return; // 函数返回后，关闭连接套接字文件描述符，结束进程
        }

        recv_bytes = recv(sock_fd, msg_recv, g_buff_size, 0); // 接收消息
        if (recv_bytes > 0)                                   // 有数据
        {
            printf("Received message: %s", msg_recv); // 接收的消息
        }
        else if (recv_bytes == 0) // 服务端进程提前终止，在服务端标准输入Ctrl+C中断进程
        {
            printf("Server terminated prematurely\n");
            return; // 函数返回后，关闭套接字文件描述符，结束进程
        }
        else if ((recv_bytes == -1) && (errno == EINTR)) // 信号或网络中断recv()
        {
            continue; // 继续发送和接收数据
        }
        else if (recv_bytes == -1) // 错误
        {
            perror("recv() error");
            return; // 函数返回后，关闭套接字文件描述符，结束进程
        }
    }

    return;
}