#include"SocketServer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#define PORT 55555
#define MAXBUF 512

char recv_buf[MAXBUF+1];
char link_state=1;
void* fun(void* x)
{
   //printf("enter thread!\r\n");
   pthread_detach(pthread_self());//非阻塞
   int new_fd=*((int*)x);
   while(1)
   {
     bzero(recv_buf, MAXBUF + 1);
     int len = recv(new_fd, recv_buf, MAXBUF, 0);
     if(recv_buf[0] == 'p')
     {
        link_state = 0;
        printf("------------客户端要断开！服务器别再发了！----------\r\n");
        break;
     }
   }
   return NULL;
}

char send_buf[MAXBUF+1];
int newfd;
int inewfd=0;
int startServer()
{
    printf("run the server \n");
   int sockfd=socket(AF_INET,SOCK_STREAM,0);//第 1 步 创建套接字
   struct sockaddr_in svraddr;//第 2 步 设置地址结构体
   svraddr.sin_family=AF_INET;//使用 internet 协议
   svraddr.sin_port=htons(PORT);
   inet_aton("127.0.0.1",&svraddr.sin_addr);
   int ret=bind(sockfd,(struct sockaddr*)&svraddr,sizeof(svraddr));//第 3 步 绑定
   if(ret<0)
   {
    printf("error bind!\r\n");
    exit(-1);
   }
   listen(sockfd,2);//第 4 步 监听，最多两个client
   while(1)
   {
     newfd=accept(sockfd,NULL,NULL); //第 5 步 接收
     pthread_t ntid;
     pthread_create(&ntid,NULL,fun,&newfd);
     while(1)
     {
        if(link_state == 0)
            break;
        bzero(send_buf, MAXBUF + 1);
        strcpy(send_buf, "hello,my client!\n");
        int len = send(newfd, send_buf, strlen(send_buf), 0);//发消息给客户端
        if(len < 0)
            printf("消息发送失败！错误代码是%d，错误信息是'%s'\n", errno, strerror(errno));
         else
            printf("消息发送成功，共发送了%d个字节！\n", len);
        sleep(1);
     }
     link_state = 1;
   }
 /* 关闭连接 */
 close(sockfd);
 return 0;
}
