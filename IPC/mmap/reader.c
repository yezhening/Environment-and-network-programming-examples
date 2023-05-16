// 头文件————————————————————
#include <fcntl.h>    // open()
#include <sys/mman.h> // mmap()、munmap()
#include <unistd.h>   // close()
#include <stdio.h>    // perror()
#include <stdlib.h>   // exit()
#include <string.h>   // strcpy()

// 主函数————————————————————
int main(int argc, char *argv[])
{
    char *share_file_name = "share.txt"; // 共享文件的名称
    int share_file_flag = O_RDONLY;      // 共享文件的标志————注意：与写端不同的地方
    int share_fd;                        // 共享文件的文件描述符

    void *mmap_init_addr = NULL; // 内存映射的起始地址
    size_t mmap_size = 7;        // 内存映射的大小。单位：字节
    int mmap_prompt = PROT_READ; // 内存映射的提示————注意：与写端不同的地方
    int mmap_flag = MAP_SHARED;  // 内存映射的标志
    off_t mmap_offset = 0;       // 内存映射的偏移
    char *mmap_addr;             // 内存映射的地址

    char msg[7]; // 消息
    // 注意：需要初始化大小以读取内存映射的消息，否则报错：Segmentation fault

    // 打开文件
    share_fd = open(share_file_name, share_file_flag);

    // 创建内存映射
    if ((mmap_addr = mmap(mmap_init_addr, mmap_size, mmap_prompt, mmap_flag, share_fd, mmap_offset)) == MAP_FAILED)
    {
        close(share_fd);

        perror("mmap() error");
        exit(EXIT_FAILURE);
    }

    // 从内存映射读取数据
    strcpy(msg, mmap_addr);
    printf("Read message:\n");
    printf("%s\n", msg);

    // 解除内存映射
    if ((munmap(mmap_addr, mmap_size)) == -1)
    {
        perror("munmap() error");
        exit(EXIT_FAILURE);
    }

    // 关闭文件
    close(share_fd);

    return 0;
}