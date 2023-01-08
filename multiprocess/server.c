// 头文件————————————————————
// #include <sys/socket.h> // socket()、sockaddr、bind()、listen()、accept()、recv()、send()
#include <stdio.h>  //(perror())、printf()
#include <stdlib.h> //exit()
// #include <netinet/in.h> //sockaddr_in、（htons()）
#include <string.h>    //bzero()、strncpy()
#include <arpa/inet.h> //inet_pton()
// #include <unistd.h>     //close()、fork()
#include <errno.h> //errno

// 全局常量————————————————————
const g_serv_port = 6666;     // 服务端的端口号
const g_listen_max_count = 5; // 监听的最大连接数
const g_buff_size = 64;      // 消息缓冲区的大小

// 函数声明————————————————————
void handle_request(int connect_fd); // 处理请求

// 主函数————————————————————
int main(int arg, char *argv[])
{
    // 网络连接————————————————————
    int listen_fd; // 监听套接字文件描述符
    // 创建套接字并获取套接字文件描述符
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr; // 服务端网络信息结构体
    // 初始化服务端网络信息结构体
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(g_serv_port);
    if ((inet_pton(AF_INET, "0.0.0.0", &serv_addr.sin_addr)) != 1)
    {
        perror("inet_pton() error");
        exit(EXIT_FAILURE);
    }

    // 绑定套接字与网络信息
    if ((bind(listen_fd, (struct sockaddr *)(&serv_addr), sizeof(serv_addr))) == -1)
    {
        if ((close(listen_fd)) == -1)
        {
            perror("bind() close() error");
            exit(EXIT_FAILURE);
        }

        perror("bind() error");
        exit(EXIT_FAILURE);
    }

    // 套接字设置被动监听状态
    if ((listen(listen_fd, g_listen_max_count)) == -1)
    {
        if ((close(listen_fd)) == -1)
        {
            perror("listen() close() error");
            exit(EXIT_FAILURE);
        }

        perror("listen() error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in clie_addr; // 客户端网络信息结构体
    int clie_addr_size;           // 客户端网络信息结构体大小
    int connect_fd;               // 连接套接字文件描述符
    bzero(&clie_addr, sizeof(clie_addr));
    clie_addr_size = sizeof(struct sockaddr);
    pid_t pid; // 进程号
    // 循环监听客户端请求
    // 原则：父进程不能退出，子进程可以退出
    while (1)
    {
        // 与客户端建立连接
        if ((connect_fd = accept(listen_fd, (struct sockaddr *)(&clie_addr), &clie_addr_size)) == -1)
        {
            perror("accept() error");
            continue; // 继续监听
        }

        // 创建子进程处理请求
        pid = fork();
        if (pid == -1) // 错误
        {
            perror("fork() error");
            continue; // 继续监听
        }
        else if (pid == 0) // 子进程
        {
            if ((close(listen_fd)) == -1) // 1.关闭监听套接字文件描述符
            {
                if ((close(connect_fd)) == -1)
                {
                    perror("fork() close() connect_fd child_process error");
                    exit(EXIT_FAILURE); // 子进程退出
                }

                perror("fork() close() listen_fd child_process error");
                exit(EXIT_FAILURE); // 子进程退出
            }

            handle_request(connect_fd); // 2.处理请求

            if ((close(connect_fd)) == -1) // 3.关闭连接套接字文件描述符
            {
                perror("fork() close() connect_fd2 child_process error");
                exit(EXIT_FAILURE); // 子进程退出
            }

            exit(EXIT_SUCCESS);
        }
        else if (pid > 0) // 父进程
        {
            if ((close(connect_fd)) == -1) // 关闭连接套接字文件描述符
            {
                perror("fork() close() connect_fd parent_process error");
                continue; // 继续监听
            }
        }
    }

    if ((close(listen_fd)) == -1) // 父进程关闭监听套接字文件描述符。实际不会执行
    {
        perror("close() listen_fd error");
        exit(EXIT_FAILURE);
    }

    return 0;
}

// 函数定义————————————————————
// 处理请求
void handle_request(int connect_fd) // 参数：连接套接字文件描述符
{
    // 传输消息————————————————————
    char msg_recv[g_buff_size]; // 从客户端接收的消息缓冲区
    char msg_send[g_buff_size]; // 发送到客户端的消息缓冲区
    int recv_bytes;             // 接收的消息字节数

    while (1) // 循环接收和发送消息
    {
        bzero(&msg_recv, sizeof(*msg_recv));
        bzero(&msg_send, sizeof(*msg_send));

        recv_bytes = recv(connect_fd, msg_recv, g_buff_size, 0); // 接收消息
        if (recv_bytes > 0) // 有消息
        {
            printf("Received message: %s", msg_recv); // 接收的消息

            strncpy(msg_send, msg_recv, g_buff_size);               // 发送的消息
            if ((send(connect_fd, msg_send, g_buff_size, 0)) == -1) // 发送消息
            {
                perror("send() error");
                return; // 函数返回后，关闭连接套接字文件描述符，结束子进程
            }
        }
        else if (recv_bytes == 0) // 文件末尾EOF：在客户端标准输入Ctrl+D
        {
            printf("The process %d received the end of file\n", getpid());
            return; // 函数返回后，关闭连接套接字文件描述符，结束子进程
        }
        else if ((recv_bytes == -1) && (errno == EINTR)) // 信号或网络中断recv()
        {
            continue; // 继续接收消息
        }
        else if (recv_bytes == -1) // 错误
        {
            perror("recv() error");
            return; // 函数返回后，关闭连接套接字文件描述符，结束子进程
        }
    }

    return;
}