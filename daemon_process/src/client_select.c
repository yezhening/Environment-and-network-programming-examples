// 头文件————————————————————
// #include <sys/socket.h> // socket()、sockaddr{}、connect()、send()、recv()、shutdown()
#include <stdio.h>  // perror()、printf()、gets()
#include <stdlib.h> // exit()
// #include <netinet/in.h> // sockaddr_in{}、htons()
#include <string.h>     // memset()、strcat()
#include <arpa/inet.h>  // inet_pton()
#include <unistd.h>     // close()、STDIN_FILENO
#include <errno.h>      // errno
#include <sys/select.h> // select()

// 全局常量————————————————————
const char g_connect_serv_ip[INET_ADDRSTRLEN] = "127.0.0.1"; // 连接服务端的IP地址
const uint16_t g_connect_serv_port = 6000;                   // 连接服务端的端口号
const int g_buff_size = 32;                                  // 消息缓冲区大小。单位：字节

// 函数声明————————————————————
void handle(int); // 处理

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
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(g_connect_serv_port);
    if ((inet_pton(AF_INET, g_connect_serv_ip, &serv_addr.sin_addr)) != 1)
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
    // select()准备————————————————————
    // 监听标准输入文件描述符和套接字文件描述符的可读条件
    int maxfdp1;        // 要监听描述符的最大值+1
    fd_set read_fd_set; // 可读文件描述符集合
    if (STDIN_FILENO >= sock_fd)
    {
        maxfdp1 = STDIN_FILENO + 1;
    }
    else
    {
        maxfdp1 = sock_fd + 1;
    }
    FD_ZERO(&read_fd_set);

    // 传输消息————————————————————
    char msg_send[g_buff_size]; // 发送到服务端的消息缓冲区
    char msg_recv[g_buff_size]; // 从服务端接收的消息缓冲区
    int recv_byte;              // 接收的消息字节数

    int stdin_eof = 0; // 标准输入文件描述符读到文件末尾的标志：0未读到EOF，1读到EOF

    printf("Please input the message to be sent directly below:\n");
    while (1) // 循环发送和接收消息
    {
        // 1.select()调用
        // 因为select()返回会修改fd_set，所以每循环都要重新设置监听的fd_set
        // 当stdin_eof == 1时，写半部关闭，不需要再设置监听标准输入文件描述符
        if (stdin_eof == 0)
        {
            FD_SET(STDIN_FILENO, &read_fd_set);
        }
        FD_SET(sock_fd, &read_fd_set);
        if ((select(maxfdp1, &read_fd_set, NULL, NULL, NULL)) == -1)
        {
            perror("select() error");
            continue; // 若有错误在下个循环继续调用
        }

        memset(&msg_send, 0, sizeof(*msg_send));
        memset(&msg_recv, 0, sizeof(*msg_recv));

        // 2.select()检测
        if (FD_ISSET(STDIN_FILENO, &read_fd_set)) // 标准输入文件描述符可读
        {
            if ((fgets(msg_send, g_buff_size, stdin)) == NULL)
            // 从标准输入获取消息。错误或遇到文件结尾(EOF)：在客户端标准输入Ctrl+D或Ctrl+C，相当于关闭连接
            {
                printf("End of connection\n");

                stdin_eof = 1;                          // 设置标志
                FD_CLR(STDIN_FILENO, &read_fd_set);     // 清理fd_set
                if ((shutdown(sock_fd, SHUT_WR)) == -1) // 写半部关闭
                {
                    perror("shutdown() error");
                    return; // 函数返回后，关闭连接套接字文件描述符，结束进程
                }
                continue;
                // 不是return，因为可能还需要从网络套接字文件描述符读
                // 不需要进入下面的send()，服务端会recv()接收EOF
            }

            if ((send(sock_fd, msg_send, g_buff_size, 0)) == -1) // 发送消息
            {
                perror("send() error");
                return; // 函数返回后，关闭连接套接字文件描述符，结束进程
            }
            printf("Send message: %s", msg_send);
        }
        else if (FD_ISSET(sock_fd, &read_fd_set)) // 套接字文件描述符可读
        {
            recv_byte = recv(sock_fd, msg_recv, g_buff_size, 0); // 接收消息
            if (recv_byte > 0)                                   // 有数据
            {
                printf("Received message: %s", msg_recv); // 接收的消息
            }
            else if (recv_byte == 0) // 服务端进程提前终止，在服务端标准输入Ctrl+C中断进程
            {
                // 如果已经调用shutdown()写半部关闭，当服务端recv()EOF后调用close()时，是正常的结束连接
                // 否则，是服务端ctrl+c提前关闭连接
                if (stdin_eof == 1)
                {
                    return; // 函数返回后，关闭套接字文件描述符，结束进程
                }

                printf("Server terminated prematurely\n");
                return; // 函数返回后，关闭套接字文件描述符，结束进程
            }
            else if (recv_byte == -1)
            {
                if (errno == EINTR) // 信号或网络中断recv()
                {
                    continue; // 继续接收消息
                }

                perror("recv() error"); // 错误
                return;                 // 函数返回后，关闭连接套接字文件描述符，结束子进程
            }
        }
    }

    return;
}