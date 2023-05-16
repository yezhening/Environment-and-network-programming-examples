// 头文件————————————————————
#include <unistd.h>    // fork()
#include <sys/types.h> // pid_t
#include <stdio.h>     // printf()

// 全局变量————————————————————
// 全局变量
int g_glov = 0;

// 主函数————————————————————
int main(int argc, char *argv[])
{
    pid_t p_id; // 进程的标识

    // 创建子进程
    p_id = fork();
    if (p_id > 0) // 父进程
    {
        printf("Parent process: The global variable: %d\n", g_glov);
    }
    else if (p_id == 0) // 子进程
    {
        printf("Child process: The global variable: %d\n", g_glov);
    }

    return 0;
}