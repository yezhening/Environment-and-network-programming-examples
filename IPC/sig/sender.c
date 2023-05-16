// 头文件————————————————————
#include <stdio.h>     // fopen()、fscanf()、fclose()
#include <sys/types.h> // pid_t
#include <signal.h>    // kill()
#include <stdio.h>     // perror()
#include <stdlib.h>    // exit()

// 主函数————————————————————
int main(int argc, char *argv[])
{
    pid_t p_id;      // 进程的标识
    FILE *p_id_file; // 记录进程标识的文件

    // 从文件中读取进程的标识
    p_id_file = fopen("p_id.txt", "r");
    fscanf(p_id_file, "%d", &p_id);
    fclose(p_id_file);

    // 向进程发送信号
    if ((kill(p_id, SIGTERM)) == -1)
    {
        perror("kill() error");
        exit(EXIT_FAILURE);
    }

    return 0;
}
