//gcc -o vul dop.c -ldl
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

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
    my_addr.sin_port = htons(41037);//服务器端口号需提取
    
    
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

typedef struct _mystruct
{
	void (*foo)();
}mystruct;

void f1(){
        printf("f1\n");
	strcpy(string,"hello from f1\n");
	write(client_sockfd,string,strlen(string));
}

void f2(){
        printf("f2\n");
	strcpy(string,"hello from f2\n");
	write(client_sockfd,string,strlen(string));
}

void set_string(char a[]){
	char b[2];
	b[0] = a[0];
	//printf("set\n");
}

void systemaddr(){
	void* handle = dlopen("libc.so.6", RTLD_LAZY);
	long long int sysaddr = (long long int)dlsym(handle,"system");
	write(client_sockfd,&sysaddr,8);
}

mystruct mf1;
mystruct mf2;
mystruct mf3;
mystruct *pmf1 = &mf1;
mystruct *pmf2 = &mf2;
mystruct *pmf3 = &mf3;
long long int old_addr;

int main(int argc,char * argv[]){
	socketConnect();
	//printf("sizeof %d\n",sizeof(int));
	int old_value, new_value;
	char buf;
	int *p = &old_value;
	int *q = &new_value;
	int limit = 2;
	int choose = 0;
	pmf1->foo = f1;
	pmf2->foo = f2;
	pmf3->foo = systemaddr;
	old_addr = (long long int)&old_value;
	write(client_sockfd,&old_addr,8);
	while(limit--){
                printf("limit is %d\n",limit);
		choose = limit + 1;
		read(client_sockfd, &buf, 512);
		if(choose == 1)
			pmf1->foo();
		else if(choose == 2)
			pmf2->foo();
		else
			set_string(&buf);
		*p = *q;
	}
	pmf2->foo();
}
