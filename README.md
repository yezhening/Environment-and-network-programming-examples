# 环境和网络编程实例（Environment-and-network-programming-examples）

## 一、说明

- 均使用Linux操作系统环境
- 均使用网际协议版本4（IPv4）

## 二、内容

### 1.迭代改进内容

（1）传输控制协议（TCP）简单版：TCP_simple

- 使用传输控制协议（TCP）

- 一个服务端连接一个客户端

- 一次自动通信

（2）多进程版：multiprocess

- 使用传输控制协议（TCP）

- 一个服务端可连接多个客户端

- 多次手动通信

（3）IO复用版：IO_multiplexing

- 使用传输控制协议（TCP）
- 服务端多进程，一个服务端可连接多个客户端
- 用户在客户端终端输入，可多次手动通信
- 在上一份代码实例：多进程版的基础上，服务端增加获取客户端地址的逻辑；更新部分函数使用、错误处理、注释和Makefile文件；为保证代码简洁，部分输入输出和字符串处理函数未进行错误检测
- 3个客户端代码实例分别使用IO复用的select、poll和epoll技术，同时监听用户输入和网络接收，可即时接收服务端进程终止和服务端主机关机消息
- 客户端使用shutdown()而不是close()关闭连接，当客户端主动关闭写半部连接后，服务端仍能够接收而不是丢弃批量输入的缓冲区数据

### 2.其他内容

（1）用户数据报协议（UDP）简单版：UDP_simple

- 使用用户数据报协议（UDP）

- 一个服务端连接一个客户端

- 一次自动通信

## 三、主要参考资料

- 《UNIX环境高级编程（第3版）》作者：W.Richard Stevens，Stephen A.Rago

- 《UNIX网络编程（第3版）》作者：W.Richard Stevens，Bill Fenner，Andrew M.Rudoff
