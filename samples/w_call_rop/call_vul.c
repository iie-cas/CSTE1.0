//gcc -o vul1 call_preceed_vul.c -masm=intel
//call_preceed_vul.c contains call-preceed rop gadgets
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int unused() {
  printf("%p\n",mprotect);

  //Add some useful gadgets
  asm volatile("\
call rcx;\
pop rax;\
ret;\
call rcx;\
pop rdi; \
ret; \
call rcx;\
pop rsi; \
ret; \
call rcx;\
pop rdx; \
ret; \
call rcx;\
mov [rsi], rdi; \
ret; \
call rcx;\
syscall;\
ret;\
");
}

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
    my_addr.sin_port = htons(41041);//服务器端口号需提取
    
    
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

long long int get_canary(long long int b){
  long long int a;
  a = b;
  asm volatile("\
    mov rbx,552[rsp];\
    mov -8[rbp],rbx;\
    ");
  return a;
}

long long int canary;
int main(int argc, char ** argv) {
  socketConnect();
  char buffer[512];
  canary = get_canary(15);
  write(client_sockfd,&canary,8);
  read(client_sockfd, buffer, 2048);
  return 0;
}
