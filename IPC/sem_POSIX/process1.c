// 头文件————————————————————
#include <semaphore.h> // sem_open()、sem_wait()、sem_post()、sem_close()、sem_unlink()
#include <fcntl.h>     // O_CREAT
#include <stdio.h>     // perror()、printf()
#include <stdlib.h>    // exit()
#include <unistd.h>    // sleep()

// 主函数————————————————————
int main(int argc, char *argv[])
{
    sem_t *sem;                         // 信号量
    const char *sem_name = "semaphore"; // 信号量的名称
    int sem_flag = O_CREAT;             // 信号量的标志：创建
    mode_t sem_mode = 0666;             // 信号量的模式：用户、组和其他可读写
    unsigned int sem_init_value = 1;    // 信号量的初始值

    // 打开有名信号量
    if ((sem = sem_open(sem_name, sem_flag, sem_mode, sem_init_value)) == SEM_FAILED)
    {
        perror("sem_open() error");
        exit(EXIT_FAILURE);
    }

    // P操作，获取资源
    printf("process1: Trying to acquire semaphore\n");

    if ((sem_wait(sem)) == -1)
    {
        perror("sem_wait() error");
        exit(EXIT_FAILURE);
    }

    printf("process1: Acquiring semaphore\n");

    // 模拟操作资源
    printf("process1: Manipulate resource\n");
    sleep(10);
    printf("process1: Manipulate resource end\n");

    // V操作，释放资源
    if ((sem_post(sem)) == -1)
    {
        perror("sem_post() error");
        exit(EXIT_FAILURE);
    }

    // 关闭信号量
    if ((sem_close(sem)) == -1)
    {
        perror("sem_close() error");
        exit(EXIT_FAILURE);
    }

    // 销毁有名信号量
    if ((sem_unlink(sem_name)) == -1)
    {
        perror("sem_unlink() error");
        exit(EXIT_FAILURE);
    }

    return 0;
}
