// 头文件————————————————————
#include <sys/ipc.h> // ftok()
#include <sys/sem.h> // semget()、semop()
#include <stdio.h>   // perror()、printf()
#include <stdlib.h>  // exit()
#include <unistd.h>  // sleep()

// 主函数————————————————————
int main(int argc, char *argv[])
{
    key_t sem_key;         // 信号量的键（外部名）
    int sem_num = 1;       // 信号量的数量
    int sem_flag = 0666;   // 信号量的标志：用户、组和其他可读写————注意：与进程1不同
    int sem_id;            // 信号量的标识（内部名）
    struct sembuf sem_buf; // 信号量的操作

    // 创建信号量的键（外部名）
    if ((sem_key = ftok(".", 'k')) == ((key_t)-1))
    {
        perror("ftok() error");
        exit(EXIT_FAILURE);
    }

    // 获取信号量集合
    if ((sem_id = semget(sem_key, sem_num, sem_flag)) == -1)
    {
        perror("semget() error");
        exit(EXIT_FAILURE);
    }

    // P操作，获取资源
    printf("process2: Trying to acquire semaphore\n");

    sem_buf.sem_num = 0;                    // 对索引为0的信号量
    sem_buf.sem_op = -1;                    // 获取1个信号量
    sem_buf.sem_flg = SEM_UNDO;             // 若进程异常终止，则撤销操作
    if ((semop(sem_id, &sem_buf, 1)) == -1) // 操作1个信号量
    {
        perror("semop() error");
        exit(EXIT_FAILURE);
    }

    printf("process2: Acquiring semaphore\n");

    // 模拟操作资源
    printf("process2: Manipulate resource\n");
    sleep(10);
    printf("process2: Manipulate resource end\n");

    // V操作，释放资源
    sem_buf.sem_op = 1;
    if ((semop(sem_id, &sem_buf, 1)) == -1) // 操作1个信号量
    {
        perror("semop() V error");
        exit(EXIT_FAILURE);
    }

    return 0;
}