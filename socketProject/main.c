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
int main()
{
    /************/
    /****socket client and servers***/
    /************/
//a= startServer();
//printf("server result is %d",a);
//    b = startClient();
//    printf("client result is %d" ,b);

    /************/
    /****thread operations***/
    /************/
    thread_pool*threadPool=malloc(sizeof(thread_pool));
    pool_init(threadPool ,4);
    pthread_key_t key;
    pthread_key_create(&key ,NULL);
    pool_add_task(threadPool ,&task_routine,"thisIsAParameter" ,key);
    sleep(6);
    taskStatus status = pool_get_task_status(key ,threadPool);
    u_print("now the task status is %d (1)\n" ,status);
    sleep(10);
    pool_abort_task(key ,threadPool);
    status = pool_get_task_status(key ,threadPool);
    u_print("now the task status is %d (2)\n" ,status);

    return 0;
}

    /************/
    /**** log testing ***/
    /************/
void * task_routine(void *arg)
{
    printf("this is in the task routine and the arg is %s\n ,thread [%u]\n" ,(char *)arg ,pthread_self());
    sleep(5);
    printf("now the task_routine has finished on thread [%u]\n" ,pthread_self());
    return NULL;
}
