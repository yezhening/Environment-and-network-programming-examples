// 头文件————————————————————
#include <unistd.h>    // pipe()、fork()、close()、write()、read()
#include <stdio.h>     // perror()、printf()
#include <stdlib.h>    // exit()
#include <sys/types.h> // pid_t

// 主函数————————————————————
int main(int argc, char *argv[])
{
    int pipe_fd[2];              // 管道的文件描述符
    pid_t p_id;                  // 进程的标识
    char msg[8] = "message\n";   // 消息
    const int msg_buf_size = 10; // 消息缓冲区的大小。单位：字节
    // 忘记初始化了...导致子进程读取不到消息
    char read_msg_buf[msg_buf_size]; // 读取消息的缓冲区
    ssize_t read_msg_size;           // 读取消息的大小。单位：字节

    // 创建管道
    if ((pipe(pipe_fd)) == -1)
    {
        perror("pipe() error");
        exit(EXIT_FAILURE);
    }

    // 派生子进程
    p_id = fork();
    if (p_id > 0) // 父进程
    {
        // 关闭读端文件描述符
        close(pipe_fd[0]);

        printf("Write message:\n");
        printf("%s\n", msg);
        // 向子进程写入消息
        write(pipe_fd[1], msg, sizeof(msg));

        // 正常终止
        exit(EXIT_SUCCESS);
    }
    else if (p_id == 0) // 子进程
    {
        // 关闭写端文件描述符
        close(pipe_fd[1]);

        // 从父进程读取消息
        read_msg_size = read(pipe_fd[0], read_msg_buf, msg_buf_size);

        printf("Read message:\n");
        // 将消息写入标准输出
        write(STDOUT_FILENO, read_msg_buf, read_msg_size);

        // 正常终止
        exit(EXIT_SUCCESS);
    }

    return 0;
}