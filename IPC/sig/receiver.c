// 头文件————————————————————
#include <signal.h>    // signal()
#include <string.h>    // strsignal()
#include <stdio.h>     // printf()、perror()、fopen()、fprintf()、fclose()
#include <stdlib.h>    // exit()
#include <unistd.h>    // getpid()
#include <sys/types.h> // pid_t

// 函数声明————————————————————
// 信号处理函数
// 参数：信号
void sig_handler(int);

// 主函数————————————————————
int main(int argc, char *argv[])
{
    pid_t p_id;      // 进程的标识
    FILE *p_id_file; // 记录进程标识的文件

    // 注册信号处理函数
    if ((signal(SIGTERM, sig_handler)) == SIG_ERR)
    {
        perror("signal() error");
        exit(EXIT_FAILURE);
    }

    // 获取进程的标识
    p_id = getpid();

    // 将进程的标识写入文件
    p_id_file = fopen("p_id.txt", "w");
    fprintf(p_id_file, "%d", p_id);
    fclose(p_id_file);

    // 等待信号
    while (1)
    {
    }

    return 0;
}

// 函数定义————————————————————
// 信号处理函数
// 参数：信号
void sig_handler(int sig)
{
    char *sig_str; // 信号的字符串

    // 获取信号的字符串
    sig_str = strsignal(sig);

    // 退出进程
    printf("Reveiver: Received signal: %s\n", sig_str);
    exit(EXIT_SUCCESS);

    return;
}