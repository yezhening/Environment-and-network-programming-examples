// 头文件————————————————————
#include <stdio.h>  // fopen()、fprintf()、fflush()、printf()、fseek()、fscanf()、fclose()
#include <unistd.h> // sleep()

// 主函数————————————————————
int main(int argc, char *argv[])
{
    // 注意：
    // 1.读取和写入前使用fseek()定位到文件开头以定位“信号量”值
    // 2.写入后使用fflush()刷新缓冲区以即时更新文件

    FILE *sem_file; // 记录“信号量”的文件
    int sem = 1;  // “信号量”

    // 将“信号量”写入文件
    sem_file = fopen("sem.txt", "w+");
    fprintf(sem_file, "%d", sem);
    fflush(sem_file);

    // P操作，获取资源
    printf("process1: Trying to acquire semaphore\n");

    fseek(sem_file, 0, SEEK_SET); // 获取“信号量”
    fscanf(sem_file, "%d", &sem);

    while (sem <= 0)
    {
        fseek(sem_file, 0, SEEK_SET);
        fscanf(sem_file, "%d", &sem);
    }

    --sem; // 修改“信号量”
    fseek(sem_file, 0, SEEK_SET);
    fprintf(sem_file, "%d", sem);
    fflush(sem_file);

    printf("process1: Acquiring semaphore\n");

    // 模拟操作资源
    printf("process1: Manipulate resource\n");
    sleep(10);
    printf("process1: Manipulate resource end\n");

    // V操作，释放资源
    fseek(sem_file, 0, SEEK_SET); // 获取“信号量”
    fscanf(sem_file, "%d", &sem);

    ++sem; // 修改“信号量”
    fseek(sem_file, 0, SEEK_SET);
    fprintf(sem_file, "%d", sem);
    fflush(sem_file);

    // 关闭文件
    fclose(sem_file);

    return 0;
}