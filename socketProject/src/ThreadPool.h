#ifndef THREADPOOL_H_INCLUDED
#define THREADPOOL_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "CommonTypes.h"

typedef struct _taskStatus
{
    kTaskStatusReady ,
    kTaskStatusDoing ,
    kTaskStatusAbort ,
    kTaskStatusFinished
} taskStatus;

struct task
{
    void *(*task)(void *args); //pointer function
    taskStatus status;
    void *arg; //arguments
    struct task*next;//task stack
};

typedef struct _thread_pool
{
    pthread_mutex_t lock; //mutex lockkkk
    pthread_cond_t cond;  //conditions
    struct task*task_list; //task list

    pthread_t *tids; // thread id array

    unsigned waiting_tasks_count; //task_count of waiting status
    unsigned active_tasks_count; //task_count at work
    bool isShutdown; //thread_pool status of open
} thread_pool;

bool pool_init(thread_pool *pool ,unsigned int threads_num);
bool pool_add_task(thread_pool* pool , void* (*task)(void *arg),void *arg);
/**
 add a tesk to the thread

 @param pool the thread_pool to which the task will add .
 @param task the real task to run on the thread.
 @param arg the argument passed to the task
 @param key the key set for the task
 @return successfully add the task to the thread_pool
 */
bool pool_add_task(thread_pool* pool , void* (*task)(void *arg),void *arg,pthread_key_t *key);
int pool_add_thread(thread_pool *pool ,unsigned additional_threads);
int pool_remove_thread(thread_pool *pool ,int removing_threads);
bool pool_destroy(thread_pool *pool);

/**
 abort the task with the key of key(in the pool_add_task ,the key was set)

 @param key the pthread_key_t type of key to identifier
 @return successfully abort the task or not
 */
bool pool_abort_task(pthread_key_t *key);

#endif // THREADPOOL_H_INCLUDED
