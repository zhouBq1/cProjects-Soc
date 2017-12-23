
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
    pool_init(4);
    pthread_key_t key;
    pthread_key_create(&key ,NULL);
    pool_add_task(&task_routine,"thisIsAParameter" ,key);//add into the pool and do the task_routine
    taskStatus status = pool_get_task_status(key);
    u_print("now the task status is %s (0)\n" ,GET_STATUS_STRING(status));
    sleep(3);
    status = pool_get_task_status(key);
    u_print("now the task status is %s (1)\n" ,GET_STATUS_STRING(status));
    sleep(3);
    status = pool_get_task_status(key);
    u_print("now the task status is %s (2)\n" ,GET_STATUS_STRING(status));
    pool_abort_task(key);
    status = pool_get_task_status(key);
    u_print("now the task status is %s (3)\n" ,GET_STATUS_STRING(status));
    pthread_key_delete(key);

    return 0;
}

    /************/
    /**** log testing ***/
    /************/
void * task_routine(void *arg)
{
    printf("this is in the task routine and the arg is %s\n ,thread [%u]\n" ,(char *)arg ,pthread_self());
    sleep(2);
    printf("now the task_routine has finished on thread [%u]\n" ,pthread_self());
    return NULL;
}
