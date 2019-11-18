# Project5 Designing a Thread Pool

## 517030910294 刘瀚文

> When thread pools are used, a task is submitted to the pool and executed by a thread from the pool. Work is submitted to the pool using a queue, and an available thread removes work from the queue. If there are no available threads, the work remains queued until one becomes available. If there is no work, threads await notification until a task becomes available. This project involves creating and managing a thread pool, and it may be completed using either Pthreds and POSIX synchronization or Java. Below we provide the details relevant to each specific technology. 

## Answer

我使用的是C语言进行编程，实现了三个文件。

`client.c`这个文件是客户端文件，实现对函数的调用和进程的整体控制。

`threadpool.c`进程池实现代码，实现需要实现的函数的功能。

`threadpool.h`头文件，呈现`function prototypes`

### threadpool.c

按照代码中的顺序，依次进行分析和展示。

`enqueue(task t)` 这个函数实现的是进入队列的操作，应用`mutex`锁保护队列中现在任务的数目。

```c
// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) {
    pthread_mutex_lock(&mutex);
    if (curTaskNum == QUEUE_SIZE) {
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    worktodo[curTaskNum++] = t;
    pthread_mutex_unlock(&mutex);
    return 0;
}
```

`*dequeue()`大同小异的实现，需要注意这里的`worktodo`列表使用的是引用的形式。

```c
// remove a task from the queue
task *dequeue() {
    pthread_mutex_lock(&mutex);
    if (curTaskNum == 0) {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    task *curTask = &worktodo[--curTaskNum];
    pthread_mutex_unlock(&mutex);
    return curTask;
}
```

`*worker(void *param)`线程池中的线程执行的函数，这里面使用了`sem_wait(&taskCnt)`;这行代码能够在线程池中没有任务的时候进行等待（为啥必须等待呢，因为后面有`dequeue()`呀）。

```c
// the worker thread in the thread pool
void *worker(void *param) {
    // execute the task
    while (TRUE) {
        sem_wait(&taskCnt); // block until there is an available task
        task *t = dequeue();
        if (t == NULL) { continue; }
        execute(t->function, t->data);
    }
}
```

`pool_submit(void (*somefunction)(void *p), void *p)` 提交任务到线程池中。

其中使用了`sem_post(&taskCnt)`;，当线程进入到线程池中的时候，给`taskCnt`这个变量`+1`。

```c
/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p) {
    task tmpTask;

    tmpTask.function = somefunction;
    tmpTask.data = p;

    int result = enqueue(tmpTask);

    if (result) {
        printf("Queue is Full! /n");
        return 1;
    }
    sem_post(&taskCnt);    // suceess -> signal the semaphore
    return 0;
}
```

`pool_init(void)`初始化线程池，初始化信号量，并且逐个将线程添加到线程池中。

```c
// initialize the thread pool
void pool_init(void) {
    pthread_mutex_init(&mutex, NULL);
    sem_init(&taskCnt, 0, 0);
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_create(&bee[i], NULL, worker, NULL);
    }
}
```

pool_shutdown(void)终止线程池的操作。

利用循环，逐个取消线程并且汇总线程的结果。销毁信号量。

```c
// shutdown the thread pool
void pool_shutdown(void) {
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_cancel(bee[i]);
        pthread_join(bee[i], NULL);
    }
    sem_destroy(&taskCnt);
    pthread_mutex_destroy(&mutex);
}

```

### client.c

按照代码中的顺序，首先是定义`add(void *param)`函数

```c
void add(void *param) {
    struct data *temp;
    temp = (struct data *) param;

    printf("I add two values %d and %d result = %d\n", temp->a, temp->b, temp->a + temp->b);
}
```

主函数中依次接受外界输入任务个数，调用函数初始化线程池，并且提交任务到队列中。睡眠一段时间，等待计算的结果，最后终止线程池，释放之前创建的空间。

```c
int main(void) {
    // create some work to do
    int n = 0;
    printf("Please input the number of tasks: ");
    scanf("%d", &n);

    // initialize the thread pool
    pool_init();

    // submit the work to the queue
    struct data *params = malloc(n * sizeof(struct data));
    for (int i = 0; i < n; i++) {
        params[i].a = i;
        params[i].b = i + 1;
        while (pool_submit(&add, &params[i]));
    }
    printf("All the tasks have been submitted.\n");

    // may be helpful 
    sleep(3);

    pool_shutdown();

    free(params);
    return 0;
}
```



## 结果展示

![result](https://s3.ap-east-1.amazonaws.com/stark99.com/blog-image/project5_thread_pool.png)

