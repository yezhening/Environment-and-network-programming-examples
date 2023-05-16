// 头文件————————————————————
#include <sys/ipc.h> // ftok()
#include <stdio.h>   // perror()、printf()
#include <stdlib.h>  // exit()
#include <sys/shm.h> // shmget()、shmat()、shmdt()
#include <string.h>  // strcpy()

// 主函数————————————————————
int main(int argc, char *argv[])
{
    key_t shm_key;                    // 共享内存的键（外部名）
    int shm_size = 7;                 // 共享内存的大小。单位：字节
    int shm_flag = IPC_CREAT | 0666; // 共享内存的标志：创建，用户、组和其他可读写
    int shm_id;                       // 共享内存的标识（内部名）
    const void *shm_at_addr = NULL;   // 共享内存连接的地址
    int shm_at_flag = 0;              // 共享内存连接的标志
    char *shm_addr;                   // 共享内存的地址
    char *msg = "message";        // 消息

    // 创建共享内存的键（外部名）
    if ((shm_key = ftok(".", 'k')) == ((key_t)-1))
    {
        perror("ftok() error");
        exit(EXIT_FAILURE);
    }

    // 获取共享内存
    if ((shm_id = shmget(shm_key, shm_size, shm_flag)) == -1)
    {
        perror("shmget() error");
        exit(EXIT_FAILURE);
    }

    // 连接共享内存
    shm_addr = (char *)shmat(shm_id, shm_at_addr, shm_at_flag);
    if (shm_addr == (char *)-1)
    {
        perror("shmat() error");
        exit(EXIT_FAILURE);
    }

    // 向共享内存中写入消息
    strcpy(shm_addr, msg);
    printf("Write message:\n");
    printf("%s\n", msg);

    // 分离共享内存
    if ((shmdt(shm_addr)) == -1)
    {
        perror("shmdt() error");
        exit(EXIT_FAILURE);
    }

    return 0;
}
