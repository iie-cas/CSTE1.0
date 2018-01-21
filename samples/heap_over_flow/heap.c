//gcc -g heap.c -o heap

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
uint64_t *a[2010];
int i=2000;
void Malloc(void);
void Free(void);
void Edit(void);
void Print(void);



char string[1000];

int server_sockfd;//服务器端套接字
int client_sockfd;//客户端套接字
int len;
struct sockaddr_in my_addr;//服务器网络地址结构
struct sockaddr_in remote_addr;//客户端网络地址结构体
int sin_size;
char buf[BUFSIZ];//数据传送的缓冲区


int main(void)
{
   
    memset(&my_addr,0,sizeof(my_addr));//数据初始化 清零
    my_addr.sin_family = AF_INET;//设置为IP通 信
    my_addr.sin_addr.s_addr = INADDR_ANY;//服务器IP地址，允许连接到所有本地地址上
    my_addr.sin_port = htons(41022);//服务器端口号需提取
    
    
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
    strcpy(string,"welcome to my server\n");
    len  = send(client_sockfd,string,strlen(string),0);//发送欢迎信息
    
    
    
    setbuf(stdout,NULL);
    while (1)
    {
        
        printf("What is your choice?(m/f/e/p)\n");
	strcpy(string,"What is your choice?(m/f/e/p)\n");
        len  = send(client_sockfd,string,strlen(string),0);
        memset(buf,0,BUFSIZ);
        len = recv(client_sockfd,buf,BUFSIZ,0);
        printf("%s\n",buf);

        if(buf[0] == 'm')
        {
            Malloc();
        }
        else if(buf[0] == 'f')
        {
            Free();
        }
        else if(buf[0] == 'e')
        {
            Edit();
        }
        else if(buf[0] == 'p')
        {
            Print();
        }
        else
        {
            break;
        }
    }
    printf("end\n");
}

void Malloc(void)
{
    printf("malloc a heap\n");
    printf("enter the size of chunk\n");
    //准备发送数据
    strcpy(string,"malloc a heap\nenter the size of chunk\n");
    //发送
    len  = send(client_sockfd,string,strlen(string),0);
    //清空buf
    memset(buf,0,BUFSIZ);
    //接收
    len = recv(client_sockfd,buf,BUFSIZ,0);
    //由于是ascii所以处理一下
    int q = 0;
    for (int i =0; i<strlen(buf); i++) {
        int a = (int)buf[i]-48;
        for (int j =strlen(buf)-i-1; j>0; j--) {
            a = a*10;
        }
        q += a;
        printf("%d\n",q);
    }
    a[i] = (uint64_t *)malloc(q);
    printf("enter your code\n");
    strcpy(string,"enter your code\n");
    len  = send(client_sockfd,string,strlen(string),0);
    memset(a[i],0,sizeof(a[i]));
    len = recv(client_sockfd,a[i],BUFSIZ,0);     
    printf("%s\n",a[i]);
    printf("ptr adress is %p\n",a[i]);
    i++;
    printf("new OK!\n");
    strcpy(string,"new Ok!\n");
    len  = send(client_sockfd,string,strlen(string),0);
}

void Free(void)
{
  
    printf("which heap do you want to free?\n");
    strcpy(string,"which heap do you want to free?\n");
    len  = send(client_sockfd,string,strlen(string),0);
    memset(buf,0,BUFSIZ);
    len = recv(client_sockfd,buf,BUFSIZ,0);
    int j = (int)buf[0] - 48;
    printf("%d\n",j);
    free(a[j+2000]);
    printf("free Ok!\n");
    strcpy(string,"free Ok!\n");
    len  = send(client_sockfd,string,strlen(string),0);
}

void Edit(void)
{
    printf("which heap do you want to edit?\n");
    strcpy(string,"which heap do you want to edit?\n");
    len  = send(client_sockfd,string,strlen(string),0);
    memset(buf,0,BUFSIZ);
    len = recv(client_sockfd,buf,BUFSIZ,0);
    int j = (int)*buf - 48;//ASCII
    printf("%d\n",j);
    printf("str:\n");
    strcpy(string,"str:\n");
    len  = send(client_sockfd,string,strlen(string),0);
    memset(a[j+2000],0,sizeof(a[j+2000]));
    len = recv(client_sockfd,a[j+2000],BUFSIZ,0);
    printf("edit Ok!\n");
    strcpy(string,"edit Ok!\n");
    len  = send(client_sockfd,string,strlen(string),0);
}
void Print(void)
{
    printf("which heap do you want to show?\n");
    strcpy(string,"which heap do you want to show?\n");
    len  = send(client_sockfd,string,strlen(string),0);
    memset(buf,0,BUFSIZ);
    len = recv(client_sockfd,buf,BUFSIZ,0);
    int j = (int)buf[0] - 48;
    printf("%d\n",j);
    printf("content is %s \n",a[j+2000]);
    printf("print end\n");
    strcpy(string,"content is");
    /*以长整形获取指针中所存内容（free函数的地址），并转换为字符串存于strbuf
      因为c语言字符串处理二进制会出现问题例如\x00c语言会以为是空的然后截断，所以改成长整形*/
    char strbuf[20];
    sprintf(strbuf,"%ld",*a[j+2000]);
    //拼接两个字符串
    strcat(string,strbuf);
    strcat(string,"\nprint end\n");
    printf("%s\n",string);
    len  = send(client_sockfd,string,strlen(string),0);
}




