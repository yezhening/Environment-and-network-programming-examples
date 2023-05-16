// 头文件————————————————————
#include <fcntl.h>    // open()
#include <sys/stat.h> // S_IRUSR、S_IWUSR
#include <unistd.h>   // ftruncate()、close()
#include <sys/mman.h> // mmap()、munmap()
#include <stdio.h>    // perror()
#include <stdlib.h>   // exit()
#include <string.h>   // strcpy()

// 主函数————————————————————
int main(int argc, char *argv[])
{
    char *share_file_name = "share.txt";              // 共享文件的名称
    int share_file_flag = O_CREAT | O_TRUNC | O_RDWR; // 共享文件的标志
    mode_t share_file_mode = S_IRUSR | S_IWUSR;       // 共享文件的模式
    int share_fd;                                     // 共享文件的文件描述符

    void *mmap_init_addr = NULL;              // 内存映射的起始地址
    size_t mmap_size = 7;                     // 内存映射的大小。单位：字节
    int mmap_prompt = PROT_READ | PROT_WRITE; // 内存映射的提示
    int mmap_flag = MAP_SHARED;               // 内存映射的标志
    off_t mmap_offset = 0;                    // 内存映射的偏移
    char *mmap_addr;                          // 内存映射的地址

    const char *msg = "message"; // 消息

    // 打开文件
    share_fd = open(share_file_name, share_file_flag, share_file_mode);

    // 调整文件大小
    ftruncate(share_fd, mmap_size);
    // 注意：必须调整文件大小，否则会报错：Bus error，表示访问的内存映射区不存在

    // 创建内存映射
    if ((mmap_addr = mmap(mmap_init_addr, mmap_size, mmap_prompt, mmap_flag, share_fd, mmap_offset)) == MAP_FAILED)
    {
        close(share_fd);

        perror("mmap() error");
        exit(EXIT_FAILURE);
    }

    // 向内存映射写入数据
    strcpy(mmap_addr, msg);
    printf("Write message:\n");
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
