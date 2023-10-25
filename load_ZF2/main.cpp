#include <iostream> // string...
#include <fstream>  // ifstream...
#include <string>   // getline()...
#include <sstream>  // stringstream...
#include <vector>   // vector<>...

#include <unistd.h>   // fork()、execv()
#include <sys/wait.h> // waitpid()、WIFEXITED()、WEXITSTATUS()

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::stringstream;
using std::vector;

struct SystemFile // 系统文件
{
    string file_name;   // 文件名称
    string virtual_dev; // 挂载设备号
    string mount_dir;   // 挂载目录
};

// const string g_config_file_path("./setupvendor.conf"); // 配置文件路径
const string g_config_file_path("/usr/local/etc/setupvendor.conf"); // 配置文件路径
const string g_file_num_key("file_num");                            // 配置文件中系统文件数量的键
string g_file_num_value("");                                        // 配置文件中系统文件数量的值
int g_file_num_value2 = 0;                                          // 配置文件中系统文件数量的值2

const string g_file_name_key("file_name");     // 配置文件中系统文件名称的键
const string g_virtual_dev_key("virtual_dev"); // 配置文件中虚拟设备号的键
const string g_mount_dir_key("mount_dir");     // 配置文件中挂载目录的键
vector<struct SystemFile> g_system_file_vec{}; // 系统文件向量

// 在配置文件的一行字符串【键 = 值换行符】中，依据键获取值
void get_key_value(const string &line, const string &key, string &value)
{
    size_t pos = string::npos; // 等号出现位置的索引
    pos = line.find('=');
    // 1. 存在键 line.find(key)) == 0
    // 2. 存在等号 pos != string::npos
    // 注意：在freebsd每一行line最后会多出一个字符应该是换行符，舍去；windows不存在该问题
    if (((line.find(key)) == 0) && (pos != string::npos))
    {
        value = line.substr(0, line.size() - 1);
        value = value.substr(pos + 2); // 取值
    }

    return;
}

// 获取系统文件数量
void get_system_file_num()
{
    cout << "[Read config file]" << endl;

    // 1. 打开配置文件
    ifstream config_file(g_config_file_path.c_str()); // 配置文件文件输入流对象
    if (!config_file.is_open())
    {
        cerr << "get_system_file_num() is_open() Open config file error" << endl;
        exit(EXIT_FAILURE);
    }

    // 2. 获取系统文件数量
    string line(""); // 配置文件的一行内容
    while (getline(config_file, line))
    {
        if (config_file.fail())
        {
            cerr << "get_system_file_num() getline() Read line error" << endl;
            exit(EXIT_FAILURE);
        }

        get_key_value(line, g_file_num_key, g_file_num_value);
        if (g_file_num_value != "") // 找到键值就退出循环
        {
            break;
        }
    }

    if (g_file_num_value == "")
    {
        cerr << "get_system_file_num() Get file_num error" << endl;
        exit(EXIT_FAILURE);
    }

    // 3. 将string转换为int并检查值内容
    stringstream ss(g_file_num_value); // 字符串流对象
    ss >> g_file_num_value2;
    if (ss.fail())
    {
        cerr << "get_system_file_num() Convert file_num error" << endl;
        exit(EXIT_FAILURE);
    }
    if (g_file_num_value2 <= 0 || g_file_num_value2 >= 5)
    {
        cerr << "get_system_file_num() Invalid file_num error" << endl;
        exit(EXIT_FAILURE);
    }

    // 4. 清理资源
    config_file.close();

    cout << g_file_num_key << " : " << g_file_num_value2 << endl;
    cout << endl;

    return;
}

// 获取系统文件配置
void get_system_file_config()
{
    // 1. 打开配置文件
    ifstream config_file(g_config_file_path.c_str()); // 配置文件对象
    if (!config_file.is_open())
    {
        cerr << "get_system_file_config() is_open() Open config file error" << endl;
        exit(EXIT_FAILURE);
    }

    // 2. 依据系统文件数量，获取系统文件配置
    stringstream ss("");                   // 字符串流对象
    string i_str("");                      // 遍历号的string类型
    string file_name_key(g_file_name_key); // 临时键，每循环会根据循环数变化，如file_name0、file_name1
    string virtual_dev_key(g_virtual_dev_key);
    string mount_dir_key(g_mount_dir_key);

    string line("");               // 配置文件的一行内容
    struct SystemFile system_file; // 结构体对象

    for (int i = 0; i < g_file_num_value2; ++i)
    {
        // 2.1 构造临时键。如：file_name0、file_name1
        ss.str(""); // 清空stringstream
        ss.clear(); // 清除错误状态
        ss << i;
        if (ss.fail())
        {
            cerr << "get_system_file_config() Convert i_str error " << i << endl;
            exit(EXIT_FAILURE);
        }
        i_str = ss.str();
        file_name_key = g_file_name_key + i_str;
        virtual_dev_key = g_virtual_dev_key + i_str;
        mount_dir_key = g_mount_dir_key + i_str;
        // cout << i_str << endl;
        // cout<< file_name_key << endl;
        // cout << virtual_dev_key << endl;
        // cout << mount_dir_key << endl;

        // 2.2 循环一次获取一个系统文件的配置
        config_file.clear();  // 清除文件流的状态
        config_file.seekg(0); // 将文件位置设置为开头
        line = "";
        while (getline(config_file, line))
        {
            if (config_file.fail())
            {
                cerr << "get_system_file_config() getline() Read line error" << endl;
                exit(EXIT_FAILURE);
            }

            get_key_value(line, file_name_key, system_file.file_name);
            get_key_value(line, virtual_dev_key, system_file.virtual_dev);
            get_key_value(line, mount_dir_key, system_file.mount_dir);
        }

        if (system_file.file_name.empty() || system_file.virtual_dev.empty() || system_file.mount_dir.empty())
        {
            cerr << "get_system_file_config() Get system file config error " << i_str << endl;
            exit(EXIT_FAILURE);
        }

        // 2.3 存储到向量容器
        g_system_file_vec.push_back(system_file);
    }

    // 3. 清理资源
    config_file.close();

    cout << "file config : " << endl;
    for (int i = 0; i < g_file_num_value2; ++i)
    {
        cout << g_file_name_key << i << " : " << g_system_file_vec[i].file_name << endl;
        cout << g_virtual_dev_key << i << " : " << g_system_file_vec[i].virtual_dev << endl;
        cout << g_mount_dir_key << i << " : " << g_system_file_vec[i].mount_dir << endl;
        cout << endl;
    }

    return;
}

// 执行命令
// 输入：命令，参数数组，参数数量
// 注意execv需要char*const*参数
void exec_cmd(const char *cmd, char *const arg[], const int arg_size)
{
    for (int i = 0; i < arg_size; ++i)
    {
        cout << arg[i] << " ";
    }
    cout << endl;

    pid_t pid = fork(); // 进程号
    if (pid == -1)
    {
        perror("exec_cmd() fork() error");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        execv(cmd, arg);
    }

    // pid != 0
    int ret = -1;                   // 系统调用返回结果
    int status = 0;                 // 进程结束状态
    ret = waitpid(pid, &status, 0); // 等待子进程，结束状态，阻塞等待
    if ((ret == -1) || ((WIFEXITED(status)) && (WEXITSTATUS(status) != 0)))
    {
        perror("exec_cmd() Execute command error");
        exit(EXIT_FAILURE);
    }

    return;
}

// 挂载文件系统
void mount_file_system()
{
    cout << "[Mount file system, exec cmd]" << endl;

    const char *mdconfig_cmd = "/sbin/mdconfig"; // 1. 命令
    const int mdconfig_arg_size = 8;             // 3. 参数个数，若多出一个访问空指针会segment fault
    string virtual_dev("");                      // 虚拟磁盘分区名称，如md0a
    const char *mount_cmd = "/sbin/mount";
    const int mount_arg_size = 5;
    for (const auto &file : g_system_file_vec)
    {
        // mdconfig命令
        // 2. 参数
        const char *mdconfig_arg[] = {
            "/sbin/mdconfig",
            "-a",
            "-t",
            "vnode",
            "-f",
            file.file_name.c_str(),
            "-u",
            file.virtual_dev.c_str(),
            nullptr};
        exec_cmd(mdconfig_cmd, const_cast<char *const *>(mdconfig_arg), mdconfig_arg_size);
        // 注意execv需要char*const*参数
        // const char *[] = const char **指向的值不可变
        // char * const []=char *const *，指针不可变即指向不可变
        // const_cast用于去除const，只读变成只写不安全，在程序员明确行为不会修改数据才安全

        // mount命令
        virtual_dev = "/dev/md" + file.virtual_dev + "a";
        const char *mount_arg[] = {
            "/sbin/mount",
            "-t",
            "ufs",
            virtual_dev.c_str(),
            file.mount_dir.c_str(),
            nullptr};
        exec_cmd(mount_cmd, const_cast<char *const *>(mount_arg), mount_arg_size);
    }

    return;
}

// 卸载文件系统
void umount_file_system()
{
    cout << "[Umount file system, exec cmd]" << endl;

    const char *umount_cmd = "/sbin/umount";
    const int umount_arg_size = 2;
    const char *mdconfig_cmd = "/sbin/mdconfig";
    const int mdconfig_arg_size = 4;
    for (const auto &file : g_system_file_vec)
    {
        // umount命令
        const char *umount_arg[] = {
            "/sbin/umount",
            file.mount_dir.c_str(),
            nullptr};
        exec_cmd(umount_cmd, const_cast<char *const *>(umount_arg), umount_arg_size);

        // mdconfig命令
        const char *mdconfig_arg[] = {
            "/sbin/mdconfig",
            "-d",
            "-u",
            file.virtual_dev.c_str(),
            nullptr};
        exec_cmd(mdconfig_cmd, const_cast<char *const *>(mdconfig_arg), mdconfig_arg_size);
    }

    return;
}

int main(int argc, char *argv[])
{
    // 依据参数判断挂载或卸载文件系统
    if (argc >= 3)
    {
        cerr << "main() Argument number error" << endl;
        exit(EXIT_FAILURE);
    }
    if ((argc == 2) && (((strncmp(argv[1], "umount", 6)) != 0)))
    {
        cerr << "main() Invalid argument error" << endl;
        exit(EXIT_FAILURE);
    }

    get_system_file_num();    // 获取系统文件数量
    get_system_file_config(); // 获取系统文件配置

    if ((argc == 2) && (((strncmp(argv[1], "umount", 6)) == 0))) // 有2个参数且第二个是umount，则卸载文件系统
    {
        umount_file_system();
        return 0;
    }
    mount_file_system(); // 默认1个参数挂载文件系统
    return 0;
}