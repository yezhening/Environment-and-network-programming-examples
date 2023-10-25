【系统文件路径】
/usr/local/share/ZF2VendorImage/
ZF2VendorImage0.bin
ZF2VendorImage1.bin
ZF2VendorImage2.bin

【配置文件路径】
/usr/local/etc/
setupvendor.conf

【启动程序路径】
/root/start_ZF2/
main

【执行程序】
cd /root/start_ZF2
c++ -o main main.cpp
./main————挂载文件系统
./main umount————卸载文件系统

【错误处理】
main() Argument number error————参数>=3数量错误
main() Invalid argument error————参数==2，但不合法不是"umount"

get_system_file_num() is_open() Open config file error————配置文件路径错误；不存在配置文件
get_system_file_num() getline() Read line error————getline()读取配置文件时发生错误
get_system_file_num() Get file_num error————配置文件不存在file_num的键或值；无等号、换行符等格式不规范问题
get_system_file_num() Convert file_num error————stringstream转换file_num string到int失败
get_system_file_num() Invalid file_num error————file_num<=0 或 >=5，值不合法

get_system_file_config() is_open() Open config file error————配置文件路径错误；不存在配置文件
get_system_file_config() Convert i_str error 0—————stringstream转换遍历号int到string失败，最后数字指出第几个文件出问题
get_system_file_config() getline() Read line error————getline()读取配置文件时发生错误
get_system_file_config() Get system file config error 0————配置文件中缺少指定序号系统文件的某个键或值；无等号、换行符等格式不规范问题，最后数字指出第几个文件出问题
未检测值所代表的目录、设备号是否合法，若不合法在进一步执行shell命令时会报错

exec_cmd() fork() error————fork()子进程失败，见上上一行执行的具体命令和上一行打印的错误原因
exec_cmd() Execute command error————execv()执行命令或waitpid()失败，见上上一行执行的具体命令和上一行打印的错误原因

mdconfig可能的原因：检查参数（配置文件值）是否合法；bin文件是否存在；设备号是否冲突
mdconfig: realpath: No such file or directory：bin文件是否存在；名称是否正确；设备号是否正确
mdconfig: ioctl(/dev/mdctl): Device busy：设备号冲突

mount可能的原因：检查参数（配置文件值）是否合法；挂载目录是否存在；设备号正确
mount: /usr/local/www/tm/vendor: No such file or directory：需要存在指定目录，需手动创建
mount: /dev/md9: No such file or directory：参数应该使用md9a而不是md9

【用到的Shell命令】
/sbin/mdconfig -a -t vnode -f /usr/local/share/ZF2VendorImage/ZF2VendorImage0.bin -u 0
操作成功：bin文件 映射到 虚拟设备/dev/md0的一个分区/dev/md0a

/sbin/mount -t ufs /dev/md9a /usr/local/www/tm/vendor
操作前需要存在指定目录
操作成功：
虚拟设备的文件系统 挂载到 目录
该目录成为虚拟设备中文件系统的根目录
可以通过这个目录来访问和操作zf2.bin文件中的内容，就像操作一个磁盘分区一样
可以浏览、读取和写入zf2.bin文件的内容，就像在一个真实的文件系统上一样
使用这种方法可以创建一个临时的虚拟文件系统，适用于需要临时操作文件内容而不想影响实际文件系统的情况，或者在某些场景下需要模拟文件系统行为的情况

/sbin/mdconfig -d -u 0
删除文件到虚拟设备的映射

/sbin/umount /usr/local/www/tm/vendor
卸载文件系统