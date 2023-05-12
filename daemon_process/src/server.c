// 头文件————————————————————
#include <unistd.h>       // fork()、close()、setsid()、dup2()、chdir()、getpid()、write()
#include <stdio.h>        // perror()、setbuf()、printf()、sprintf()
#include <stdlib.h>       // exit()
#include <signal.h>       // signal()
#include <sys/resource.h> // rlimit{}、getrlimit()
#include <sys/stat.h>     // umask()
#include <fcntl.h>        // open()
#include <errno.h>        // errno
// #include <sys/socket.h> // socket()、setsockopt()、sockaddr{}、bind()、listen()、accept()、recv()、send()
// #include <netinet/in.h> // sockaddr_in{}、htons()、ntohs()
#include <string.h>    // memset()、strcat()、strcpy()
#include <arpa/inet.h> // inet_pton()、inet_ntop()
#include <sys/wait.h>  // waitpid()

// 全局常量————————————————————
const char g_serv_listen_ip[INET_ADDRSTRLEN] = "0.0.0.0"; // 服务端监听的IP地址
const uint16_t g_serv_listen_port = 6000;                 // 服务端监听的端口号
const int g_listen_max_count = 5;                         // 监听的最大连接数
const int g_buff_size = 32;                               // 消息缓冲区的大小。单位：字节

// 函数声明————————————————————
void create_daemon();     // 创建守护进程
void sigchld_handle(int); // SIGCHLD信号处理
void handle_request(int); // 处理请求

// 主函数————————————————————
int main(int arg, char *argv[])
{
    // 创建守护进程————————————————————
    create_daemon();
    printf("The server runs at: %s:%d. The process id is: %d\n", g_serv_listen_ip, g_serv_listen_port, getpid());

    // 网络连接————————————————————
    int listen_fd; // 监听套接字文件描述符
    //  创建套接字并获取套接字文件描述符
    if ((listen_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    // 设置套接字选项
    int sock_opt_val = 1; // 二元/标志选项，设置为非0值打开
    if ((setsockopt(listen_fd, SOL_SOCKET, SO_KEEPALIVE, &sock_opt_val, sizeof(int))) == -1)
    {
        perror("setsockopt() SO_KEEPALIVE error");
        exit(EXIT_FAILURE);
    }
    if ((setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &sock_opt_val, sizeof(int))) == -1)
    {
        perror("setsockopt() SO_REUSEADDR error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr; // 服务端网络信息结构体
    // 初始化服务端网络信息结构体
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(g_serv_listen_port);
    if ((inet_pton(AF_INET, g_serv_listen_ip, &serv_addr.sin_addr)) != 1)
    {
        perror("inet_pton() error");
        exit(EXIT_FAILURE);
    }

    // 绑定套接字与网络信息
    if ((bind(listen_fd, (struct sockaddr *)(&serv_addr), sizeof(serv_addr))) == -1)
    {
        if ((close(listen_fd)) == -1)
        {
            perror("bind() close() error");
            exit(EXIT_FAILURE);
        }

        perror("bind() error");
        exit(EXIT_FAILURE);
    }

    // 套接字设置被动监听状态
    if ((listen(listen_fd, g_listen_max_count)) == -1)
    {
        if ((close(listen_fd)) == -1)
        {
            perror("listen() close() error");
            exit(EXIT_FAILURE);
        }

        perror("listen() error");
        exit(EXIT_FAILURE);
    }

    // 注册SIGCHLD信号的处理函数
    // 当客户端关闭连接时，服务端子进程recv()接收0/EOF后退出，向父进程发送SIGCHLD信号
    // 为避免子进程成为僵尸进程，父进程接收信号后，waitpid()回收子进程
    if ((signal(SIGCHLD, sigchld_handle)) == SIG_ERR)
    {
        perror("signal() error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in clie_addr; // 客户端网络信息结构体
    int clie_addr_size;           // 客户端网络信息结构体大小
    int connect_fd;               // 连接套接字文件描述符
    memset(&clie_addr, 0, sizeof(clie_addr));
    clie_addr_size = sizeof(struct sockaddr);
    pid_t pid; // 进程标识符

    // 循环监听客户端请求
    // 原则：父进程不能退出，子进程可以退出
    while (1)
    {
        // 与客户端建立连接
        if ((connect_fd = accept(listen_fd, (struct sockaddr *)(&clie_addr), &clie_addr_size)) == -1)
        {
            // 这里的逻辑显式将部分错误和其他错误区分开
            // 父进程阻塞在accept()慢系统调用，若捕获SIGCHLD处理并返回时，该慢系统调用可能会返回EINTR错误，需要重启
            if (errno == EINTR)
            {
                continue;
            }

            // accept()返回前连接中止，在三次握手之后accept()返回之前的时序，即connect()客户端发送一个RST复位分节给服务端，是非致命错误，需要重启
            // 解决依赖于实现，POSIX要求检测errno为ECONNABORTED而不是EPROTO错误并重启
            if (errno == ECONNABORTED)
            {
                continue;
            }

            // 对于其他错误，同样是重启而不是退出进程
            perror("accept() error");
            continue; // 继续监听
        }

        // 创建子进程处理请求
        pid = fork();
        if (pid == -1) // 错误
        {
            perror("fork() error");
            continue; // 继续监听
        }
        else if (pid == 0) // 子进程
        {
            if ((close(listen_fd)) == -1) // 1.关闭监听套接字文件描述符
            {
                if ((close(connect_fd)) == -1)
                {
                    perror("fork() close() connect_fd child_process error");
                    exit(EXIT_FAILURE); // 子进程退出
                }

                perror("fork() close() listen_fd child_process error");
                exit(EXIT_FAILURE); // 子进程退出
            }

            handle_request(connect_fd); // 2.处理请求

            if ((close(connect_fd)) == -1) // 3.关闭连接套接字文件描述符
            {
                perror("fork() close() connect_fd2 child_process error");
                exit(EXIT_FAILURE); // 子进程退出
            }

            exit(EXIT_SUCCESS);
        }
        else if (pid > 0) // 父进程
        {
            if ((close(connect_fd)) == -1) // 关闭连接套接字文件描述符
            {
                perror("fork() close() connect_fd parent_process error");
                continue; // 继续监听
            }
        }
    }

    if ((close(listen_fd)) == -1) // 父进程关闭监听套接字文件描述符。实际不会执行
    {
        perror("close() listen_fd error");
        exit(EXIT_FAILURE);
    }

    pid = getpid();
    printf("The child process terminated: %d\n", pid);

    return 0;
}

// 函数定义————————————————————
// 创建守护进程
void create_daemon()
{
    pid_t child_pid;         // 子进程进程号
    pid_t grandchild_pid;    // 孙进程进程号
    struct rlimit res_limit; // 资源软硬件限制
    int fd_max_num;          // 进程可打开的最大文件描述符数量
    int log_fd;              // 日志文件描述符

    // 1.创建子进程，父进程退出
    // 原因：
    // 若父进程是作为一条简单的shell命令启动，父进程退出让shell认为命令执行完成。子进程是孤儿进程，被进程号为1的Init进程收养，成为后台进程
    // 子进程不是会话、进程组组长（会话号和进程组号可能是父进程号不等于子进程号），为第2步的条件
    child_pid = fork();
    if (child_pid == -1)
    {
        perror("create_daemon() fork() child_pid error");
        exit(EXIT_FAILURE);
    }
    else if (child_pid > 0) // 父进程退出
    {
        exit(EXIT_SUCCESS);
    }
    // child_pid == 0未退出的是子进程，继续往下运行

    // 2.子进程创建会话
    // 原因：
    // 子进程创建新会话，为会话组长
    // 创建新进程组，为进程组组长
    // 没有控制终端但还可绑定控制终端，需要第4步处理
    if (setsid() == -1)
    // setsid()在调用进程是进程组组长时会失败，在第1步已保证其不是进程组组长
    {
        perror("create_daemon() setsid() error");
        exit(EXIT_FAILURE);
    }

    // 3.注册SIGHUP信号的忽略行为
    // 原因：
    // 在第4步中，会话首进程/子进程终止时，会向会话中的所有进程/孙进程发送SIGHUP信号
    if (signal(SIGHUP, SIG_IGN) == SIG_ERR)
    {
        perror("create_daemon() signal() error");
        exit(EXIT_FAILURE);
    }

    // 4.创建孙进程，子进程退出
    // 原因：
    // 孙进程不是会话、进程组组长（会话号和进程组号是子进程号不等于孙进程号），无权限重新打开/绑定/自动获得控制终端
    grandchild_pid = fork();
    if (grandchild_pid == -1)
    {
        perror("create_daemon() fork() grandchild_pid error");
        exit(EXIT_FAILURE);
    }
    else if (grandchild_pid > 0) // 子进程退出
    {
        exit(EXIT_SUCCESS);
    }
    // grandchild_pid == 0未退出的是孙进程，继续往下运行

    // 5.获取进程可打开的最大文件描述符数量，关闭文件描述符
    // 原因：
    // 孙进程继承了打开的文件描述符，可能会导致这些文件描述符无法释放，出现内存泄漏等问题
    // 5.1获取进程可打开的最大文件描述符数量=硬限制
    if ((getrlimit(RLIMIT_NOFILE, &res_limit)) != 0)
    // RLIMIT_NOFILE：每个进程能打开的最大文件数
    // 注意：出错返回非0值
    {
        perror("create_daemon() getrlimit() error");
        exit(EXIT_FAILURE);
    }
    fd_max_num = res_limit.rlim_max;
    // res_limit.rlim_cur：软限制，res_limit.rlim_max：硬限制，软限制<=硬限制，获取最大文件描述符数量=硬限制

    // 5.2关闭不需要的文件描述符
    for (int i = 3; i < fd_max_num; ++i)
    // 注意从3开始，第6步再将012标准输入输出错误文件描述符重定向
    {
        if ((close(i) == -1))
        {
            // 注意：有的文件描述符并不存在，也进行通用关闭，close()会报错：Bad file descriptor，errno=9=EBADF，该情况不输出错误
            if (errno == EBADF)
            {
                continue;
            }

            perror("create_daemon() close() fd_max_num error");
            exit(EXIT_FAILURE);
        }
    }

    // 6.清除文件权限掩码，设置文件掩码重定向标准文件描述符
    // 原因：
    // 进程从创建它的父进程那里继承了文件创建掩码，可能不具有操作文件的权限，清除文件权限掩码允许其访问文件操作
    // 守护进程没有控制终端，需要有一种方式记录日志信息
    // 6.1清除文件权限掩码
    umask(0);

    // 6.2重定向三个标准文件描述符到日志文件
    // 6.2.1打开日志文件，不存在创建，读写打开，存在截断清空
    if ((log_fd = open("../log.txt", O_CREAT | O_RDWR | O_TRUNC)) == -1)
    {
        perror("create_daemon() open() error");
        exit(EXIT_FAILURE);
    }

    // 6.2.2复制当前的文件描述符给标准输入、输出和错误，
    for (int i = 0; i < 3; ++i)
    {
        if ((dup2(log_fd, i)) == -1)
        {
            if ((close(log_fd)) == -1)
            {
                perror("create_daemon() dup2() close() log_fd error");
                exit(EXIT_FAILURE);
            }

            perror("create_daemon() dup2() error");
            exit(EXIT_FAILURE);
        }
    }

    // 6.2.3复制完后关闭
    // 后续操作标准输入、输出和错误文件描述符即可，关闭当前文件描述符释放资源
    if ((close(log_fd)) == -1)
    {
        perror("create_daemon() close() log_fd error");
        exit(EXIT_FAILURE);
    }

    // 7.改变工作目录为根目录
    // 原因：
    // 从父进程继承的当前工作目录可能在一个挂载的文件系统中，守护进程通常在系统再引导之前一直存在，进程活动时当前文件系统无法umount卸下(比如工作目录在一个NFS中,运行一个daemon会导致umount无法成功)
    // 一般需要将工作目录改变到根目录。对于需要转储核心、写运行日志的进程将工作目录改变到特定目录如/tmp
    if ((chdir("/")) == -1)
    {
        perror("create_daemon() chdir() error");
        exit(EXIT_FAILURE);
    }

    // 设置无缓冲IO，这一项不是创建守护进程的必须条件，只是为了让输出能够即时写入日志文件
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    return;
}

// SIGCHLD信号处理
// 参数：信号值
void sigchld_handle(int signal_val)
{
    int old_errno;                                          // 在进入信号处理函数前的旧errno
    pid_t pid;                                              // 终止子进程的进程号
    pid_t pid_tmp;                                          // 临时进程号
    char pid_str[6];                                        // char[]类型的进程号
    int pid_len;                                            // 进程号的长度
    char pid_char;                                          // char类型的进程号，1位数字
    char output_str[64] = "The child process terminated: "; // 输出字符串
    int output_str_len = 30;                                // 输出字符串的长度

    // 在进入信号处理函数前的旧errno，信号处理函数中的waitpid()、write()和signal()可能会改变旧errno，信号处理后需要恢复
    old_errno = errno;

    // 如果同时有多个子进程终止并发送SIGCHLD信号，则在循环中等待并一个个处理
    while (1)
    {
        pid = waitpid(-1, NULL, WNOHANG);
        // 参数：信号触发时等待任一子进程，不获取退出状态，非阻塞调用
        if (pid == -1)
        {
            if (errno == ECHILD)
            {
                break;
            }
            // 当waitpid()返回值为-1时，错误代码为ECHILD，错误信息为：No child processes，表示没有子进程等待，退出循环

            perror("waitpid() error");
            errno = old_errno; // perror()输出后恢复旧errno
            return;
        }
        else if (pid == 0) // 当waitpid()返回值为0时，表示有子进程未终止，退出循环
        {
            break;
        }
        // pid>0，有子进程终止等待，往下执行处理

        // 注意：标准IO、字符串处理函数strcat()、strlen()等是不可重入函数（用了全局的静态结构或缓冲区），不适合在异步的信号处理函数中使用
        // 将进程号pid_t转换为char[]类型，再用write()标准输出
        pid_tmp = pid; // 如：12345
        pid_len = 0;
        while (pid_tmp > 0)
        {
            pid_str[pid_len] = pid_tmp % 10 + '0'; // 获取最低位数字，然后转换为char类型并存储。如：5 -> '5'
            ++pid_len;

            pid_tmp /= 10; // 整数除法忽略小数位，数字抹去最低位。如：1234.5 -> 1234
        }

        // pid_str从低位存储："54321"，需要逆转从高位存储："12345"
        for (int i = 0; i < pid_len / 2; ++i)
        {
            pid_char = pid_str[i];
            pid_str[i] = pid_str[pid_len - i - 1];
            pid_str[pid_len - i - 1] = pid_char;
        }

        // 拼接输出字符串
        for (int i = 0; i < pid_len; ++i)
        {
            output_str[output_str_len] = pid_str[i];
            ++output_str_len;
        }
        output_str[output_str_len] = '\n';
        ++output_str_len;

        if ((write(STDOUT_FILENO, output_str, output_str_len)) == -1)
        {
            perror("write() error");
            errno = old_errno;
            return;
        }

        // 注意：signal()只注册一次信号处理函数，调用完后会恢复默认行为SIG_DFL，需要重新安装
        if ((signal(SIGCHLD, sigchld_handle)) == SIG_ERR)
        {
            perror("signal() error");
            errno = old_errno;
            return;
        }
    }

    errno = old_errno; // 正常返回前恢复旧errno

    return;
}

// 处理请求
// 参数：连接套接字文件描述符
void handle_request(int connect_fd)
{
    // 获取子进程的pid、客户端的IP地址和端口————————————————————
    pid_t pid;
    pid = getpid(); // 未说明错误返回

    struct sockaddr_in clie_addr; // 客户端网络信息结构体
    int clie_addr_size;           // 客户端网络信息结构体大小
    memset(&clie_addr, 0, sizeof(clie_addr));
    clie_addr_size = sizeof(struct sockaddr);
    if ((getpeername(connect_fd, (struct sockaddr *)(&clie_addr), &clie_addr_size)) == -1)
    {
        perror("getpeername() error");
        return; // 函数返回后，关闭连接套接字文件描述符，结束子进程
    }

    char clie_ip[INET_ADDRSTRLEN];          // 客户端的IP地址    如：127.0.0.1
    uint16_t clie_port;                     // 客户端的端口  如：42534
    char clie_port_str[5];                  // 客户端的端口，char[]类型  如：42534
    char clie_ip_port[INET_ADDRSTRLEN + 5]; // 客户端的IP地址和端口  如：127.0.0.1:42534
    if ((inet_ntop(AF_INET, &clie_addr.sin_addr, clie_ip, sizeof(clie_ip))) == NULL)
    {
        perror("inet_ntop() error");
        return; // 函数返回后，关闭连接套接字文件描述符，结束子进程
    }
    // 注意：返回值和第3个参数值相同，区别为一个是常量一个是变量
    clie_port = ntohs(clie_addr.sin_port);
    sprintf(clie_port_str, "%d", clie_port);
    strcpy(clie_ip_port, clie_ip);
    strcat(clie_ip_port, ":");
    strcat(clie_ip_port, clie_port_str);
    printf("The process %d accept the connection: %s\n", pid, clie_ip_port);

    // 传输消息————————————————————
    char msg_recv[g_buff_size]; // 从客户端接收的消息缓冲区
    char msg_send[g_buff_size]; // 发送到客户端的消息缓冲区
    int recv_byte;              // 接收的消息字节数

    while (1) // 循环接收和发送消息
    {
        memset(&msg_recv, 0, sizeof(*msg_recv));
        memset(&msg_send, 0, sizeof(*msg_send));

        recv_byte = recv(connect_fd, msg_recv, g_buff_size, 0); // 接收消息
        if (recv_byte > 0)                                      // 有消息
        {
            printf("From %s received message: %s", clie_ip_port, msg_recv); // 接收的消息

            strcpy(msg_send, msg_recv);                             // 发送的消息
            if ((send(connect_fd, msg_send, g_buff_size, 0)) == -1) // 发送消息
            {
                perror("send() error");
                return; // 函数返回后，关闭连接套接字文件描述符，结束子进程
            }
        }
        else if (recv_byte == 0) // 文件末尾EOF：在客户端标准输入Ctrl+D或Ctrl+C
        {
            printf("From %s received the end of file\n", clie_ip_port);
            return; // 函数返回后，关闭连接套接字文件描述符，结束子进程
        }
        else if (recv_byte == -1)
        {
            if (errno == EINTR) // 信号或网络中断recv()
            {
                continue; // 继续接收消息
            }

            perror("recv() error"); // 错误
            return;                 // 函数返回后，关闭连接套接字文件描述符，结束子进程
        }
    }

    return;
}