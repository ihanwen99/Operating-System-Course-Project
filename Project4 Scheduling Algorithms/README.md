# Scheduling Algorithms

517030910294 刘瀚文

## Question

> This project involves implementing several different process scheduling algorithms. The scheduler will be assigned a predefined set of tasks and will schedule the tasks based on the selected scheduling algorithm. Each task is assigned a priority and CPU burst. The following scheduling algorithms will be implemented:
>
> 1. First-come, first-served (FCFS), which schedules tasks in the order in which they request the CPU.
> 2. Shortest-job-first (SJF), which schedules tasks in order of the length of the tasks’ next CPU burst.
> 3. Priority scheduling, which schedules tasks based on priority.
> 4. Round-robin (RR) scheduling, where each task is run for a time quantum (or for the remainder of its CPU burst).
> 5. Priority with round-robin, which schedules tasks in order of priority and uses round-robin scheduling for tasks with equal priority.

## Background

题目中已经实现的有：

- The file `driver.c` reads in the schedule of tasks, inserts each task into a linked list, and invokes the process scheduler by calling the `schedule() `function. The `schedule()` function executes each task according to the specified scheduling algorithm.
- Tasks selected for execution on the CPU are determined by the `pick-NextTask()` function and are executed by invoking the` run()` function defined in the `CPU.c` file.
- A Makefile is used to determine the specific scheduling algorithm that will be invoked by driver.

运行的Demo指令：

```bash
make fcfs
./fcfs schedule.txt
```

## Answer

在实现中共通的地方:

我们需要自己实现`add()`,`pickNextTask()`,`schedule()`这几个函数。其中`add()`函数是可以共通使用的。

```c
void add(char *name, int priority, int burst) {
    Task *newTask = malloc(sizeof(Task));
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    insert(&taskList, newTask);
}
```

这个函数中，在链表中插入我们的`Task`，其中最新插入的在链表的尾端，最后插入的是链表的头部（这部分在`list.h`中实现）。

我们对于不同任务实际的操作就是修改不同的选择顺序pickNextTask()，并且特意化调度函数schedule()。

### First Come First Service(FCFS)

下一个要执行的任务是***最先插入链表的任务***。

```c
Task *pickNextTask() {
    struct node *lastNode = taskList;
    while (lastNode->next) {
        lastNode = lastNode->next;
    }
    return lastNode->task;
}
```
```c
void schedule() {
    while (taskList) {
        Task *task = pickNextTask();
        run(task, task->burst);
        delete(&taskList, task);
    }
}
```

### Shortest Job First(sjf)

下一个要执行的任务是***任务链表中任务执行时间最短的任务***。

```c
Task *pickNextTask() {
    struct node *curNode = taskList;
    struct node *minNode = curNode;
    while (curNode->next) {
        if (curNode->task->burst < minNode->task->burst) { minNode = curNode; }
        curNode = curNode->next;
    }
    if (curNode->task->burst < minNode->task->burst) { minNode = curNode; }
    return minNode->task;
}
```

### Priority

下一个要执行的任务是***任务列表中优先级最高的任务***。

```c
Task *pickNextTask() {
    struct node *curNode = taskList;
    struct node *priNode = curNode;
    while (curNode->next) {
        if (curNode->task->priority > priNode->task->priority) { priNode = curNode; }
        curNode = curNode->next;
    }
    if (curNode->task->priority > priNode->task->priority) { priNode = curNode; }
    return priNode->task;
}
```

### rr

对于轮询来说，下一个要执行的任务就很简单了，就是按照顺序的下一个任务就好。

```c
Task *pickNextTask() {
    Task *curTask = nextNode->task;
    nextNode = (nextNode->next) ? nextNode->next : taskList;
    return curTask;
}
```

调度函数：需要注意任务执行的时间，取轮询时间和任务剩余执行时间的小，配置任务的执行。

```c
void schedule() {
    nextNode = taskList;
    while (taskList) {
        Task *task = pickNextTask();
        int slice = min(rr, task->burst);
        run(task, slice);
        task->burst -= slice;
        if (task->burst == 0) { delete(&taskList, task); }
    }
}
```

### priority_rr

与简单的轮询相比，需要增加的是优先级的属性。

在代码中，我通过相同优先级的在同一个链表中，将所有优先级的放到原先的`taskList`中。

```c
void add(char *name, int priority, int burst) {
    Task *newTask = malloc(sizeof(Task));
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    insert(&taskList[priority], newTask);
}
```

调度函数：从最高优先级执行到最低优先级，同一优先级的任务按照原先的轮询方式执行。

```c
void schedule() {
    for (size_t p = MAX_PRIORITY; p >= MIN_PRIORITY; p--) {
        nextNode = taskList[p];
        while (taskList[p]) {
            Task *task = pickNextTask(taskList[p]);
            int slice = min(rr, task->burst);
            run(task, slice);
            task->burst -= slice;
            if (task->burst == 0) { delete(&taskList[p], task); }
        }
    }
}
```

## 结果展示

### First Come First Service(FCFS)

![975d7cabad9d65812c6a22847a4d766](project4_)

### Shortest Job First(sjf)

![150c1cc1e58a886341c5d59a561dad6]( https://s3.ap-east-1.amazonaws.com/stark99.com/blog-image/project4_sjf.png )

### Priority

![66f8896ff66749982f769d80c9a9e07]( https://s3.ap-east-1.amazonaws.com/stark99.com/blog-image/project4_priority.png )

### rr

![fdbfa82d7ae8a01c99a0af1b1ce0d40]( https://s3.ap-east-1.amazonaws.com/stark99.com/blog-image/project4_rr.png )

### priority_rr

![98fd294cc13d183d7d11b380b65e0a0]( https://s3.ap-east-1.amazonaws.com/stark99.com/blog-image/project4_priority_rr.png )

