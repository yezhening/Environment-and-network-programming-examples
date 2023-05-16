// 头文件————————————————————
#include <semaphore.h> // sem_open()、sem_wait()、sem_post()、sem_close()
#include <stdio.h>     // perror()、printf()
#include <stdlib.h>    // exit()
#include <unistd.h>    // sleep()

// 主函数————————————————————
int main(int argc, char *argv[])
{
    sem_t *sem;                         // 信号量
    const char *sem_name = "semaphore"; // 信号量的名称
    int sem_flag = 0;                   // 信号量的标志：无————注意：与进程1不同

    // 打开有名信号量
    if ((sem = sem_open(sem_name, sem_flag)) == SEM_FAILED)
    {
        perror("sem_open() error");
        exit(EXIT_FAILURE);
    }

    // P操作，获取资源
    printf("process2: Trying to acquire semaphore\n");

    if ((sem_wait(sem)) == -1)
    {
        perror("sem_wait() error");
        exit(EXIT_FAILURE);
    }

    printf("process2: Acquiring semaphore\n");

    // 模拟操作资源
    printf("process2: Manipulate resource\n");
    sleep(10);
    printf("process2: Manipulate resource end\n");

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

    return 0;
}
