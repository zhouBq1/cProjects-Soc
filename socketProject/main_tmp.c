#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#define PORT 8082
#define LISTEN_PORT 55555
#define MAXBUF 512
#define SERV_ADDR "192.168.1.100"

char send_buf[MAXBUF+1];
void *threadWrite(void *x)
{
    int new_fd = *((int*)x);
    while(1)
    {
        bzero(send_buf, MAXBUF + 1);
        scanf("%s",send_buf);
        send(new_fd, send_buf, strlen(send_buf), 0); //第 4 步 向套接字中写入字符串
    }
    return NULL;
}

char recv_buf[MAXBUF+1];
int main()
{
   int sockfd=socket(AF_INET,SOCK_STREAM,0);//第 1 步 创建一个体套接字
   struct sockaddr_in svraddr;//第 2 步 设置 addr 结构体
   svraddr.sin_family=AF_INET;//使用 internet 协议
   svraddr.sin_port=htons(PORT);
   inet_aton(SERV_ADDR,&svraddr.sin_addr);
   connect(sockfd,(struct sockaddr*)&svraddr,sizeof(svraddr));//第 3 步 连接服务器

   pthread_t ntid;
   pthread_create(&ntid,NULL,threadWrite,&sockfd);

   while(1)
   {
     bzero(recv_buf, MAXBUF + 1);/* 接收对方发过来的消息，最多接收 MAXBUF 个字节 */
     int len = recv(sockfd, recv_buf, MAXBUF, 0);/* 接收服务器来的消息 */
     if(len > 0)
        printf("接收消息成功:'%s'，共%d个字节的数据\n", recv_buf, len);
     else
        printf("消息接收失败！错误代码是%d，错误信息是'%s'\n", errno, strerror(errno));
   }
 /*  */
 close(sockfd);
 return 0;
}


bool createClient(){
    int sockfd = socket(AF_INET ,SOCK_STREAM ,0);
    if(-1 == sockfd){
        printf("socket created error ,get -1\n");
        return false;
    }
    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htonl(PORT);
    inet_aton(SERV_ADDR ,(struct sockaddr *)&cliaddr.sin_addr);
    pthr

    return true;
}

bool createServer(){
    int listenFD = socket(AF_INET ,SOCK_STREAM ,IPPROTO_TCP);
    memset(&listenAddr, 0, sizeof(listenAddr));
    if (-1 == listenFD){
        printf("socket server create error get -1\n");
        return false;
    }
    int recvSocket;
    struct sockaddr_in listenAdinet_atondr;
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = htonl(LISTEN_PORT);
    listenAddr.sin_addr.s_addr= htonl(INADDR_ANY);

    if(-1 == bind(listenFD ,(struct sockaddr *)&listenAddr ,sizeof(sockaddr))){
        printf("socket server bind error \n");
        return false;
    }

    if(-1 == listen(listenFD ,4)){
        printf("socket server listen error \n");
        return false;
    }


    pthread_t nid;
    int threadResult = pthread_create(&nid ,NULL ,threadWrite ,&listenFD);
    printf("now it is in the thread with id :%u\n ",pthread_self());
    while(1){
        int connectedFD = accept(listenFD ,NULL ,NULL);
        if(0 > connectedFD){
            printf("socket server accept error\n");
            return false;
        }
        static int clientCount = 0;
        printf("now you got a connection from the client with index :%d",clientCount);
        clientCount ++;
    }
    close(listenFD);
    pthread_exit(NULL);

    return true;
}
