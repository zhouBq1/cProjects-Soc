#include "ThreadPool.h"
#include "ThreadPoolPrivate.h"
#include <string.h>

/*
    use the thread_pool as a file_private variety ,and you will not use it outside the file through api provided,
*/
static thread_pool *pool;
static struct task* taskIsValidForKey(pthread_key_t key ,myError *err);

/*
*clean-up handler
*do the jobs while unusual-existing form current thread .
*@param arg :the arguments passed to the handler to do the cleanup jobs
*/
void handler(void *arg)
{
    pthread_mutex_t *lock = (pthread_mutex_t *)arg;
    pthread_mutex_unlock(lock);
    printf("in the function handler\n");
}

void* key_destroy_default(void *arg){
    pthread_key_t argKey = (pthread_key_t)arg;
    printf("the key is destroy with arg :%d" ,argKey);
    return NULL;
}
/*
*thread_routine -> while creating a new thread ,the routine will run to make ready of the resources for this thread.
*@param arg: the pool arguments passed to
*
*/
//void thread_routine(void *arg){
//
//}
/*
* thread function ,the main function that's do the real thread jobs
*pthread_create
*/
void* routine(void *arg)
{
    thread_pool *pool = (thread_pool*)arg;
    struct task *p;
    printf("this is in the thread with id[%u] ,function routine\n" ,pthread_self());
    while(1)
    {
        //(head->next)
        pthread_cleanup_push(handler,(void *)&pool->lock);
        pthread_mutex_lock(&pool->lock);

// to judge the node existing in the thread ,and with no node working the thread will block.

        while(pool->waiting_tasks_count == 0 && !pool->isShutdown)          //do the running-loop tog
        {
            pthread_cond_wait(&pool->cond,&pool->lock);            //让线程沉睡
        }
        if(pool->waiting_tasks_count==0&&pool->isShutdown==true)            //shutdown=true也要等任务全执行完了才能删除线程池
        {

            pthread_mutex_unlock(&pool->lock);
            pthread_exit(NULL);
        }
        p =pool->task_list->next;
        pool->task_list->next =p->next;
        pool->waiting_tasks_count--;
        while(p != NULL && p->status == kTaskStatusAborted){
            p =pool->task_list->next;
            pool->task_list->next =p->next;
            pool->waiting_tasks_count--;
        }

        //解锁
        pthread_mutex_unlock(&pool->lock);
        pthread_cleanup_pop(0);

        //处理刚才扣下来的那个节点任务
//      the thread in which the task runs won't be canceled.
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);       //禁止当前这个线程被其他线程调用pthread_cancel来删除
        printf("1key [%d] in the thread[%d] at status[%d] \n" ,p->key ,pthread_self() ,p->status);
        p->status = kTaskStatusDoing;
        struct task**tasks = pool->tasks_at_work->content;
        tasks[pool->tasks_at_work->length] = p;
        pool->tasks_at_work->length += 1;

        printf("2key [%d] in the thread[%d] at status[%d] \n" ,p->key ,pthread_self() ,p->status);
        //anything you want to pass to the key for serialization ,the value will be bind to the thread and you can get the value from the thread
        pthread_setspecific(p->key ,pthread_self());
        (p->task)(p->arg);                                       //执行任务
        p->status = kTaskStatusFinished;
        printf("3key [%d] in the thread[%d] at status[%d] \n" ,p->key ,pthread_self() ,p->status);
        pthread_key_delete(p->key);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);        //取消禁止删除效果
        //释放定义的那个节点
        free(p);
    }
    pthread_exit(NULL);//退出
}
// thread pool initializing function ,,
/*
*to initialize the thread pool ,do the following things
*   1. initialize the lock and the conditions
*   2. initialize the task_list pthread_key_tand set the head's next pointer to null
*   3. set the waiting_tasks_count to 0 ,and the active_tasks_num to threads_num
*   4. set the isShutdown to false
*   5. using the for to create the threads
*@param pool :the pointer to a thread_pool that's to initial.
*@param thraed_num :the thread_count that's created
*@return :the pool initial results
*/
bool pool_init(unsigned int threads_num)
{
    if(pool != NULL){
        pool_destroy();
    }

    pool = malloc(sizeof(thread_pool));
    printf("pool[%u] init\n" ,pthread_self());
    pthread_mutex_init(&pool->lock ,NULL);
    pthread_cond_init(&pool->cond ,NULL);

    pool->task_list=malloc(sizeof(struct task));
    pool->waiting_tasks_count = 0;
    pool->active_tasks_count = threads_num;
    pool->isShutdown = false;
    pool->tasks_at_work = malloc(sizeof(Array));
    pool->tasks_at_work->content = malloc(sizeof(struct task *) *MAX_ACTIVE_THRAED);

    pool->tids = malloc(sizeof(pthread_t) *MAX_ACTIVE_THRAED);

    if(pool->task_list == NULL || pool->tids == NULL)
    {
        printf("the task list or pool pids created fail");
        return false;
    }
    pool->task_list->next = NULL;
    pool->task_list->key = RELOLVED_PTHREAD_KEY;//pthread_key_t will start count at 0 ,so -1 as the default key.

    for(int index = 0 ; index < pool->active_tasks_count; index++)
    {
        if(pthread_create(&(pool->tids[index]),NULL ,routine,(void *)pool) != 0)
        {
            printf("create thread with index :%d failed ",index);
            return false;
        }
        else
        {
            printf("create thread success -> tid[%d]\n" ,pool->tids[index]);
        }
    }
    return true;
}

/*
*add a new task to the pool ,the task passed is a fun that's to be added as a task structure's task
*1. create a new task node ,and set the next to null
*2. pass the pointer-function and the argument to the new task node.
*3. lock the mutex while sharing the resource (the operations for the pool ,consisting of counting ,list-operations)
*4. add the new-task to the pool's task-list ,and tasks-waiting-count +1.
*5. unlock the mutex.
*@param pool :the pool to add the task to
*@param task :the task that's really doing the work
*@param arg :arg that's to passed to the task structure.
*@return successfully add a new task to the pool.
*/
static bool _pool_add_task( void *(*task)(void *arg),void *arg)
{
    printf("pool[%u] add_task\n",pthread_self());
    struct task *new_task = malloc(sizeof(struct task));
    if(new_task == NULL)
    {
        printf("new task allocate memory failed\n");
        return false;
    }
    new_task->task = task;
    new_task->arg = arg;
    new_task->next = NULL;

    pthread_mutex_lock(&pool->lock);
    if(pool->waiting_tasks_count >= MAX_ACTIVE_THRAED)
    {
        pthread_mutex_unlock(&pool->lock);
        printf("too many threads are created and waiting\n");
        free(new_task);
        return false;
    }

    struct task *tmp = pool->task_list;
    while(tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = new_task;
    pool->waiting_tasks_count ++;
    pthread_mutex_unlock(&pool->lock);
    //waken a new blocked thread and continue. this is the same as the semphore of objective-c ,resources will be blocked at a place until get a signal ,and unblock part of resources
    pthread_cond_signal(&pool->cond);
    return true;
}

/*
*add a new task to the pool ,the task passed is a fun that's to be added as a task structure's task
*1. create a new task node ,and set the next to null
*2. pass the pointer-function and the argument to the new task node.
*3. lock the mutex while sharing the resource (the operations for the pool ,consisting of counting ,list-operations)
*4. add the new-task to the pool's task-list ,and tasks-waiting-count +1.
*5. unlock the mutex.
*@param pool :the pool to add the task to
*@param task :the task that's really doing the work
*@param arg :arg that's to passed to the task structure.
*@return successfully add a new task to the pool.
*/
bool pool_add_task( void *(*task)(void *arg),void *arg ,pthread_key_t key)
{
    printf("pool[%u] add_task\n",pthread_self());
    struct task *new_task = malloc(sizeof(struct task));
    if(new_task == NULL)
    {
        printf("new task allocate memory failed\n");
        return false;
    }
    new_task->task = task;
    new_task->arg = arg;
    new_task->next = NULL;
    new_task->key = key;
    new_task->status = kTaskStatusReady;

    pthread_mutex_lock(&pool->lock);
    if(pool->waiting_tasks_count >= MAX_ACTIVE_THRAED)
    {
        pthread_mutex_unlock(&pool->lock);
        printf("too many threads are created and waiting\n");
        free(new_task);
        return false;
    }

    struct task *tmp = pool->task_list;
    while(tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = new_task;
    pool->waiting_tasks_count ++;
    pthread_mutex_unlock(&pool->lock);
    //waken a new blocked thread and continue. this is the same as the semphore of objective-c ,resources will be blocked at a place until get a signal ,and unblock part of resources
    pthread_cond_signal(&pool->cond);
    return true;
}

taskStatus pool_get_task_status(pthread_key_t key ){
    struct task *task;
    myError *err;
    err = malloc(sizeof(myError));
    task = taskIsValidForKey(key ,err);
    if(err != NULL && task == NULL){
        printf("=====%s" ,err->info);
        return kTaskStatusAbnormal;
    }
    free(err);
    return task->status;
}

bool pool_abort_task(pthread_key_t key){
    struct task *task;
    myError *err;
    err = malloc(sizeof(myError));
    task = taskIsValidForKey(key  ,err);
    if( task == NULL){
        return false;
    }
    if(err != NULL && task == NULL){
        printf("%s" ,err->info);
        return false;
    }
    task->status = kTaskStatusAborted;
    free(err);
    return true;
}

static struct task* taskIsValidForKey(pthread_key_t key ,myError *err){
    /**< search from the thread_pool ,while task_list->key is equal to the key passed ,return the task and judge the task status .
        while the task is running ,cannot stop the task ,in the task-list (mean that the task is to do sometime ,and can be aborted)
        do the aborting ,and remove the task.
     */

    struct task *task = NULL;
    struct task*tmptask = pool->task_list;
    while(tmptask ->next!= NULL && tmptask->key != key){
        tmptask = tmptask->next;
    }
    if(tmptask->key == key){
        task = tmptask;
    }else{
        pthread_mutex_lock(&pool->lock);
        if(pool->tasks_at_work != NULL && pool->tasks_at_work->length != 0){

            for(int index = 0; index < pool->tasks_at_work->length ;index ++){
//          the task at working ,the task can be get but not be abort.
            struct task ** tasks = (struct task **)pool->tasks_at_work->content;
            struct task *tTask = tasks[index];
            if(tTask->key == key){
                task = tTask;
            }
        }
        pthread_mutex_unlock(&pool->lock);
        }
    }

    if(task == NULL){
        strcpy(err->info ,"key is not set ,set the key\n");
        return NULL;
    }

    err = NULL;
    return task;
}

/*
* add threads to the pool
*
*
*@param pool:
*@param additional_threads :new thread-count to add to the pool
*return the new added thread-count
*/
int pool_add_thread(unsigned additional_threads)
{
    printf("pool[%u] add_thread\n" ,pthread_self());
    if(additional_threads <= 0)
    {
        return 0;
    }

    unsigned total_threads = pool->active_tasks_count + additional_threads;
    //actual_threads is the actual threads added to the pool.
    int index ,actual_increase = 0;
    for(index = pool->active_tasks_count; index<total_threads&&index<MAX_ACTIVE_THRAED; index++)
    {
        if(pthread_create(&pool->tids[index],NULL,routine,(void *)pool) != 0)
        {
            printf("ADD THREAD ERROR \n");

            if(actual_increase == 0)
                return -1;
        }
        break;
        actual_increase++;
    }
    pool->active_tasks_count += actual_increase;
    return actual_increase;
}

/*
*delete threads from the pool
*
*
*@param pool:thread pool to operate the threads
*@param removing_threads:threads count to remove
*@return the remaining-thread count that's in the pool.
*/
int pool_remove_thread(int removing_threads)
{
    printf("pool[%u] remove_thread\n" ,pthread_self());
    if(removing_threads <= 0)
    {
        return 0;
    }

    unsigned remaining_threads = pool->active_tasks_count- removing_threads;
    remaining_threads = remaining_threads > 0?remaining_threads:0;

    int index = 0;
    for(index = pool->active_tasks_count - 1; index > remaining_threads -1  ; index--)
    {
//the pool will delete the thread with the index they are created
        errno = pthread_cancel(pool->tids[index]);
        if(errno != 0)
        {
//when thread cancel execute failed ,then break ,run the next stop.
            break;
        }
    }
//didn't run the cancel option.
    if(index == pool->active_tasks_count-1)
    {
        return -1;
    }
    else
    {
        pool->active_tasks_count+=1;
        return index+1;
    }
}

/*
* destroy the pool ,and free the memory that's cost.
*@param pool:the pool to destroy
*@return destroy result.
*/
bool pool_destroy(void)
{
    printf("pool[%u] destroy\n" ,pthread_self());
    pool->isShutdown = true;
//    waken the threads that's blocked to collect the memory allocated to the threads.
    pthread_cond_broadcast(&pool->cond);

    int index = 0;
    for(index = 0; index < pool->active_tasks_count; index++)
    {
        errno = pthread_join(pool->tids[index] ,NULL);
        if(errno != 0)
        {
            printf("join tids[%u] error:%s\n" ,index ,strerror(errno));
        }
        else
        {
            printf("tids [%u] has joint\n" ,index);
        }
    }
    free(pool->tasks_at_work);
    free(pool->task_list);
    free(pool->tids);
    free(pool);
    return true;
}

