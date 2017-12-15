#include "src/SocketClient.h"
#include "src/SocketServer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "src/CommonTypes.h"
#include "src/ThreadPool.h"

#include <time.h>

void * task_routine(void *arg);
int main(){
    printf("run at the main func at thread :%d\n",pthread_self());
    int a =10 ,b =10;

    /************/
    /****socket client and server***/
    /************/
//a= startServer();
//printf("server result is %d",a);
//    b = startClient();
//    printf("client result is %d" ,b);

    /************/
    /****thread operations***/
    /************/
//    thread_pool*threadPool=malloc(sizeof(thread_pool));
//    pool_init(threadPool ,4);
//    pool_add_task(threadPool ,&task_routine,"thisIsAParameter");
//    sleep(20);

    /************/
    /****time operations***/
    /************/
    time_t tmpcal_ptr;
    struct tm *tmp_ptr = NULL;

    time(&tmpcal_ptr);
        //tmpcal_ptr = time(NULL);   两种取值方法均可以
    printf("tmpcal_ptr=%d\n", tmpcal_ptr);

    tmp_ptr = gmtime(&tmpcal_ptr);
    printf("after gmtime, the time is:%d:%d:%d\n", tmp_ptr->tm_hour, tmp_ptr->tm_min, tmp_ptr->tm_sec);

    tmp_ptr = localtime(&tmpcal_ptr);
    printf ("after localtime, the time is:%d.%d.%d ", (1900+tmp_ptr->tm_year), (1+tmp_ptr->tm_mon), tmp_ptr->tm_mday);
    printf("%d:%d:%d\n", tmp_ptr->tm_hour, tmp_ptr->tm_min, tmp_ptr->tm_sec);

    return 0;

    return 0;
}

void * task_routine(void *arg){
    printf("this is in the task routine and the arg is %s\n ,thread [%u]\n" ,(char *)arg ,pthread_self());
    sleep(10);
    printf("now the task_routine has finished on thread [%u]" ,pthread_self());
    return NULL;
}
