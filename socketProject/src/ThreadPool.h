
#ifndef THREADPOOL_H_INCLUDED
#define THREADPOOL_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "CommonTypes.h"

#define MAX_ACTIVE_THRAED 20
#define MYERROR_INFO_MAX_LENGTH 100
#define RELOLVED_PTHREAD_KEY 1000
typedef enum
{
    kTaskStatusReady = 0,
    kTaskStatusDoing ,
    kTaskStatusAborted ,
    kTaskStatusFinished ,
    kTaskStatusAbnormal ,
} taskStatus;

#define GET_STATUS_STRING(status) get_status_string(status)

static char * get_status_string(taskStatus status){
    char *statusString = malloc(sizeof(char) *20);
    switch(status){
        case kTaskStatusReady:sprintf(statusString ,"%s" ,"taskIsReady");break;
        case kTaskStatusDoing:sprintf(statusString ,"%s" ,"taskIsDoing");break;
        case kTaskStatusAborted:sprintf(statusString ,"%s" ,"taskIsaborted");break;
        case kTaskStatusFinished:sprintf(statusString ,"%s" ,"taskIsFinished");break;
        case kTaskStatusAbnormal:sprintf(statusString ,"%s" ,"taskIsAbnormal");break;
        default:sprintf(statusString ,"%s" ,"taskStatusNotDefined");
        return statusString;
    }
    return statusString;
}

/**
 * @brief initialize the thread pool
 *
 * @param[in] pool: the pool given to initial ,wont do anything when passed null
 * @param[in] threads_num: the max-thread-number
 * @return 0 on success, -1 when params invalid
 * @note use this function to do the thread pool initializing-jobs
 */
bool pool_init(unsigned int threads_num);
//bool pool_add_task(thread_pool* pool , void* (*task)(void *arg),void *arg);
/**
 add a tesk to the thread

 @param pool the thread_pool to which the task will add .
 @param task the real task to run on the thread.
 @param arg the argument passed to the task
 @param key the key set for the task
 @return successfully add the task to the thread_pool
 */
bool pool_add_task(void* (*task)(void *arg),void *arg,pthread_key_t key);
int pool_add_thread(unsigned additional_threads);
int pool_remove_thread(int removing_threads);
bool pool_destroy(void);

/**
 abort the task with the key of key(in the pool_add_task ,the key was set)

 @param key the pthread_key_t type of key to identifier
 @return successfully abort the task or not
 */
bool pool_abort_task(pthread_key_t key);

/**
 get the task status of the task with the identifier of key in the thread

 @param key the identifier set for the task of the thread
 @return the task status now of the thread in the thread-pool
 */
taskStatus pool_get_task_status(pthread_key_t key);

#endif // THREADPOOL_H_INCLUDED
