#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 512

int main(){
    // 创建套接字
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // IP 和 PORT
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  // 结构体中每个字节都用 0 填充
	serv_addr.sin_family = AF_INET;  // 使用 IPv4 地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // 设置具体的 IP 地址
    serv_addr.sin_port = htons(1234);  // 设置具体的端口号
    // 将套接字和特定的 IP 与端口号绑定
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    // 进入监听状态，等待用户发起请求，消息队列长度为 20
    listen(serv_sock, 20);
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    char WELCOME[] = "Welcome! I wish you a success!\n";  // 定义服务器端欢迎信息
    char BYE[] = "Bye!";
    char EXIT[] = "exit";
    char buffer[BUF_SIZE] = {0};  // 定义缓冲区
    while(1){  // 不限制连接次数
        /*
            accept() 会阻塞程序执行，直到有新的请求到来
            accept() 返回一个新的套接字来和客户端进行通信，serv_sock 是服务器端的套接字，clnt_addr 保存了客户端的 IP 地址和端口号
            后面和客户端进行通信时，使用的是 accept() 返回的 clnt_sock，而不是原来服务器端的套接字
        */
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        write(clnt_sock, WELCOME, sizeof(WELCOME) - 1);  // 发送欢迎信息
        int str_len = read(clnt_sock, buffer, sizeof(buffer) - 1);  // 读取客户端传来的信息
        while(strcmp(buffer, EXIT) != 0){  // 判断客户端是否退出
            write(clnt_sock, buffer, str_len);  // 原样返回收到的数据
            memset(buffer, 0, BUF_SIZE);  //重置缓冲区
            str_len = read(clnt_sock, buffer, sizeof(buffer) - 1);  // 读取客户端传来的信息
        }
        write(clnt_sock, BYE, sizeof(BYE) - 1);
        memset(buffer, 0, BUF_SIZE);  //重置缓冲区
        close(clnt_sock);  // 关闭套接字
    }
    //关闭套接字
    close(serv_sock);
    return 0;
}
