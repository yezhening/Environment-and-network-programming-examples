// 头文件————————————————————
#include <sys/stat.h> // mkfifo()
#include <stdio.h>    // perror()、printf()
#include <stdlib.h>   // exit()
#include <fcntl.h>    // open()
#include <unistd.h>   // write()、close()、unlink()

// 主函数————————————————————
int main(int argc, char *argv[])
{
    const char *fifo_file_path = "fifo_file";                                          // 命名管道文件的路径：在当前目录下
    mode_t fifo_file_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // 命名管道文件的模式：用户、组和其他可读写
    int fifo_fd;                                                                       // 命名管道文件的文件描述符
    char msg[9] = "message\n";                                                         // 消息

    // 创建命名管道
    if ((mkfifo(fifo_file_path, fifo_file_mode)) == -1)
    {
        perror("mkfifo() error");
        exit(EXIT_FAILURE);
    }

    // 打开命名管道，以写方式
    fifo_fd = open(fifo_file_path, O_WRONLY);

    printf("Write message:\n");
    printf("%s", msg);
    // 向命名管道写入消息
    write(fifo_fd, msg, sizeof(msg));

    // 关闭命名管道->引用计数-1
    close(fifo_fd);

    // 删除命名管道文件的链接->链接计数为0时删除文件
    unlink(fifo_file_path);

    return 0;
}
