#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

/*gadget library*/
#include "./include/cop.h"

char string[1000];

int server_sockfd;//服务器端套接字
int client_sockfd;//客户端套接字
int len;
struct sockaddr_in my_addr;//服务器网络地址结构
struct sockaddr_in remote_addr;//客户端网络地址结构体
int sin_size;
//char buf[BUFSIZ];//数据传送的缓冲区

void socketConnect(){

    memset(&my_addr,0,sizeof(my_addr));//数据初始化 清零
    my_addr.sin_family = AF_INET;//设置为IP通 信
    my_addr.sin_addr.s_addr = INADDR_ANY;//服务器IP地址，允许连接到所有本地地址上
    my_addr.sin_port = htons(41028);//服务器端口号需提取
    
    
    /* 创建服务器端套接字  IPV4协议,面向链接痛心，tcp*/
    if((server_sockfd = socket(PF_INET,SOCK_STREAM,0))<0){
        perror("socket");
        return 1;
    }
    
    /*将套接字绑定到服务器的网络地址上*/
    if(bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0){
        perror("bind");
        return 1;
    }
    
    /*监听链接请求  监听队列长度为5*/
    listen(server_sockfd,5);
    
    sin_size = sizeof(struct sockaddr_in);
    
    
    /*等待客户端连接请求到达*/
    if((client_sockfd = accept(server_sockfd,(struct sockaddr *)&remote_addr,&sin_size))<0){
        perror("accpet");
        return 1;
    }
    
    printf("accept client %s\n",inet_ntoa(remote_addr.sin_addr));
}

/*main function*/

//辅助函数并没有实际意义
void ppp(){
	printf("ppp\n");
}
//辅助函数并没有实际意义
void ppp1(){
	printf("ppp1\n");
}



int main() {
    char buffer[1024];
    socketConnect();
    read(client_sockfd, buffer, 1024);
		
    //在bss段创建一些变量，用于溢出劫持控制流
    static void (*pfun2)();
    static void (*pfun1)();
    static char *tmpfile, aa[16],*tmpfile1;
    static void (*pfun)();

    tmpfile = aa;
    tmpfile1 = aa;

    static char bb[16];
    static void (*pfun3)();
    pfun3 = ppp;
	

    //write(1, "Hello, World\n", 13);
    memcpy(aa,buffer,32);
    (*pfun3)();

    execute_all();
}
