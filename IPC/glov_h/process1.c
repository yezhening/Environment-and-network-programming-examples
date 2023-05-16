// 头文件————————————————————
#include <stdio.h> // printf()
#include "glov.h"  // g_glov

// 全局变量————————————————————
// 引入全局变量
extern int g_glov;

// 主函数————————————————————
int main(int argc, char *argv[])
{
    printf("Process1: The global variable: %d\n", g_glov);

    return 0;
}