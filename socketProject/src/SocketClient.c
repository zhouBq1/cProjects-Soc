#include "SocketClient.h"

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

char send_buf[MAXBUF+1];
void *thread(void *x)
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

int startClient()
{
    printf("run the client\n");
    int sockfd=socket(AF_INET,SOCK_STREAM,0);//第 1 步 创建一个体套接字
    struct sockaddr_in svraddr;//第 2 步 设置 addr 结构体
    svraddr.sin_family=AF_INET;//使用 internet 协议
    svraddr.sin_port=htons(PORT);
    inet_aton("127.0.0.1",&svraddr.sin_addr);
    connect(sockfd,(struct sockaddr*)&svraddr,sizeof(svraddr));//第 3 步 连接服务器

    pthread_t ntid;
    pthread_create(&ntid,NULL,thread,&sockfd);

    while(1)
    {
        bzero(recv_buf, MAXBUF + 1);/* 接收对方发过来的消息，最多接收 MAXBUF 个字节 */
        int len = recv(sockfd, recv_buf, MAXBUF, 0);/* 接收服务器来的消息 */
        if(len > 0)
            printf("接收消息成功:'%s'，共%d个字节的数据\n", recv_buf, len);
        else
            printf("消息接收失败！错误代码是%d，错误信息是'%s'\n", errno, strerror(errno));
    }
    /* 关闭连接 */
    close(sockfd);
    return 0;
}
