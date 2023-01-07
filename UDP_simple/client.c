// 头文件————————————————————
// #include <sys/socket.h> //socket()、sockaddr、sendto()、recvfrom()
#include <stdio.h>  //（perror()）、printf()
#include <stdlib.h> //exit()
// #include <netinet/in.h> //sockaddr_in、（htons()）
#include <string.h>    //bzero()、strncpy()
#include <arpa/inet.h> //inet_pton()
// #include <unistd.h>     //close()

// 全局常量————————————————————
const g_serv_port = 3333; // 服务端端口号
const g_buff_size = 16;   // 消息缓冲区大小

// 主函数————————————————————
int main(int argc, char *argv[])
{
    // 网络连接————————————————————
    int sock_fd; // 套接字文件描述符
    // 创建套接字并获取套接字文件描述符
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
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

    // 数据传输
    char msg_send[g_buff_size]; // 发送到服务端的消息缓冲区
    char msg_recv[g_buff_size]; // 从服务端接收的消息缓冲区
    bzero(&msg_recv, sizeof(*msg_recv));
    bzero(&msg_send, sizeof(*msg_send));

    strncpy(msg_send, "Hello", 6);                                                                             // 发送的消息
    if ((sendto(sock_fd, msg_send, g_buff_size, 0, (struct sockaddr *)(&serv_addr), sizeof(serv_addr))) == -1) // 发送数据
    {
        if ((close(sock_fd)) == -1)
        {
            perror("sendto() close() error");
            exit(EXIT_FAILURE);
        }

        perror("sendto() error");
        exit(EXIT_FAILURE);
    }

    if ((recvfrom(sock_fd, msg_recv, g_buff_size, 0, NULL, NULL)) == -1) // 接收数据
    {
        if ((close(sock_fd)) == -1)
        {
            perror("recvfrom() close() error");
            exit(EXIT_FAILURE);
        }

        perror("recvfrom() error");
        exit(EXIT_FAILURE);
    }
    printf("Received message: %s\n", msg_recv); // 接收的消息

    // 关闭套接字文件描述符
    if ((close(sock_fd)) == -1)
    {
        perror("close() error");
        exit(EXIT_FAILURE);
    }

    return 0;
}