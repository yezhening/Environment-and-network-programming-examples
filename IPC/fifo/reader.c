// 头文件————————————————————
#include <fcntl.h>  // open()
#include <unistd.h> // read()
#include <stdio.h>  // printf()

// 主函数————————————————————
int main(int argc, char *argv[])
{
    const char *fifo_file_path = "fifo_file"; // 命名管道文件的路径：在当前目录下
    int fifo_fd;                              // 命名管道文件的文件描述符
    char read_msg[8];                         // 读取的消息
    ssize_t read_msg_size;                    // 读取消息的大小。单位：字节

    // 打开命名管道，以读方式
    fifo_fd = open(fifo_file_path, O_RDONLY);

    // 从命名管道读取数据
    read_msg_size = read(fifo_fd, read_msg, sizeof(read_msg));

    printf("Read message:\n");
    // 将消息写入标准输出
    write(STDOUT_FILENO, read_msg, read_msg_size);

    // 关闭命名管道
    close(fifo_fd);

    return 0;
}
