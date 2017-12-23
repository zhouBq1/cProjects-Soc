
typedef struct _myError{
    char info[100] ;
    unsigned code;
} myError;


typedef struct _myArray{
    int length;
    void *content;
    void *(*addItem)(void * ,unsigned);
    void *(*removeItem)(void *);
    void *(*itemAtIndex)(unsigned);

} Array;

struct task
{
    void *(*task)(void *args); //pointer function
    taskStatus status;
    pthread_key_t key;
    void *arg; //arguments
    struct task*next;//task stack
};

typedef struct _thread_pool
{
    pthread_mutex_t lock; //mutex lockkkk
    pthread_cond_t cond;  //conditions
    struct task*task_list; //task list
    Array*tasks_at_work;

    pthread_t *tids; // thread id array

    unsigned waiting_tasks_count; //tasak_count of waiting status
    unsigned active_tasks_count; //task_count at work
    bool isShutdown; //thread_pool status of open
} thread_pool;
