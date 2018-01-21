#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>


typedef struct String{
    
    union{
        char *buf;
        char array[16];
    } o;
    int len;
    void (*free)(struct String *ptr);
    
    
} String;


struct {
    
    int inuse;
    String *str;
    
} Strings[0x10];


void showMenu(void);

int getInt(void);

void creatStr();

void deleteStr();

void freeLong(String *str);

void freeShort(String *str);

int getInt(void){
    
    char str[11];
    char ch;
    int i;
    for (i = 0; (read(STDIN_FILENO, &ch,1),ch) != '\n' && i < 10 && ch != -1; i++) {
        
        str[i] = ch;
        
    }
    str[i] = 0;
    return atoi(str);
    
}


char string[1000];

int server_sockfd;//服务器端套接字
int client_sockfd;//客户端套接字
int len;
struct sockaddr_in my_addr;//服务器网络地址结构
struct sockaddr_in remote_addr;//客户端网络地址结构体
int sin_size;
char buf[BUFSIZ];//数据传送的缓冲区

void socketConnect(){

    memset(&my_addr,0,sizeof(my_addr));//数据初始化 清零
    my_addr.sin_family = AF_INET;//设置为IP通 信
    my_addr.sin_addr.s_addr = INADDR_ANY;//服务器IP地址，允许连接到所有本地地址上
    my_addr.sin_port = htons(8000);//服务器端口号
    
    
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
}

int main(void){
	
    socketConnect();

    //char buf[1024];
    setbuf(stdout,NULL);
    setbuf(stdin,NULL);
    setbuf(stderr,NULL);
    
    printf("+++++++++++++++++++++++++++++++\n");
    printf("So, let`s crash the world \n");
    printf("+++++++++++++++++++++++++++++++\n");
    strcpy(string,"+++++++++++++++++++++++++++++++\nSo, let`s crash the world\n+++++++++++++++++++++++++++++++\n");
    len  = send(client_sockfd,string,strlen(string),0);
    
    while (1) {
        
        showMenu();
        if (len==0) {
            return 1;
        }
        if (buf[0]=='c') {
            creatStr();
        }else if(buf[0]=='d'){
            deleteStr();
        }else if(buf[0]=='b'){
            printf("bye~\n");
            return 0;
        }else
            printf("invalid cmd\n");
        
    }
}



void freeShort(String *str){
    free(str);
}



void freeLong(String *str){
    free(str->o.buf);
    free(str);
}



void deleteStr(){
    
    int id;
    printf("pls give me the string id you want ro delete\nid:");
    strcpy(string,"pls give me the string id you want ro delete\nid:");
    len  = send(client_sockfd,string,strlen(string),0);
    memset(buf,0,BUFSIZ);
    len = recv(client_sockfd,buf,BUFSIZ,0);
    id = (int)buf[0] - 48;
    printf("%d\n",id);
    if (id <0 || id > 0x10) {
        printf("invalid id\n");
    }
    
    if (Strings[id].str) {
        Strings[id].str->free(Strings[id].str);
        printf("free plt is %p\n",free);
        Strings[id].inuse = 0;
        strcpy(string,"free Ok!\n");
        len  = send(client_sockfd,string,strlen(string),0);
    }
}



void creatStr(){
    String *mString  = malloc(sizeof(String));
    char *str = NULL;
    size_t size = 0;
    
    printf("pls give string size:");
    strcpy(string,"pls give string size:");
    len  = send(client_sockfd,string,strlen(string),0);
    memset(buf,0,BUFSIZ);
    len = recv(client_sockfd,buf,BUFSIZ,0);

    for (int i =0; i<strlen(buf); i++) {
        int a = (int)buf[i]-48;
        for (int j =strlen(buf)-i-1; j>0; j--) {
            a = a*10;
        }
        size += a;   
    }
    printf("%d\n",size);
    if (size < 0 || size > 0x1000) {
        printf("incalid size\n");
        free(mString);
        return;
    }
    
    printf("str:");
    strcpy(string,"str:");
    len  = send(client_sockfd,string,strlen(string),0);
    memset(buf,0,BUFSIZ);
    len = recv(client_sockfd,buf,BUFSIZ,0);
   
    if (size < 16) {
        strncpy(mString->o.array,buf,size);
        mString->free = freeShort;
    }else{
        str = malloc(size);
        if (str == NULL) {
            printf("malloc faild!\n");
            exit(1);
        }
        strncpy(str,buf,size);
        mString->o.buf = str;
        mString->free = freeLong;
    }

    int j = 0;
    mString->len = size;
    for (j = 0; j <0x10; j++) {
        if (Strings[j].inuse == 0) {
            Strings[j].inuse = 1;
            Strings[j].str = mString;
            printf("The string id is %d\n",j);
	    printf("the string len is %d\n",mString->len);
            printf("the string%d at %p\n",j,mString);
            break;
        }
    }
    
    if (j == 0x10) {
        printf("The string list is full\n");
        mString->free(mString);
    }
    
}


void showMenu(void){
    
    printf("1.create string\n");
    printf("2.delete string\n");
    printf("3.quit\n");
    strcpy(string,"1.create string\n2.delete string\n3.quit\n");
    len  = send(client_sockfd,string,strlen(string),0);
    memset(buf,0,BUFSIZ);
    len = recv(client_sockfd,buf,BUFSIZ,0);
}

















