# Project2 Linux Kernel Module for Task Information

## 问题

> In this project, you will write a Linux kernel module that uses the `/proc` file system for displaying a task’s information based on its process identifier value pid. Before beginning this project, be sure you have completed the Linux kernel module programming project in Chapter 2, which involves creating an entry in the `/proc` file system. This project will involve writing a process identifier to the file `/proc/pid`. Once a pid has been written to the `/proc` file, subsequent reads from /proc/pid will report (1) the command the task is running, (2) the value of the task’s pid, and (3) the current state of the task. An example of how your kernel module will be accessed once loaded into the system is as follows:
>
> ```shell
> echo "1395" > /proc/pid
> cat /proc/pid
> command = [bash] pid = [1395] state = [1]
> ```
>
> The echo command writes the characters "1395" to the `/proc/pid` file. Your kernel module will read this value and store its integer equivalent as it represents a process identifier. The cat command reads from `/proc/pid`, where your kernel module will retrieve the three fields from the task struct associated with the task whose pid value is 1395.

## 实现

这个`Project`写的是操作系统的内核模块，需要使用`Makefile`文件使用`make`编译。

在上次的`/proc/second`实验的基础上，这次添加了`proc_write`的操作。

### proc_write()

```c
static ssize_t proc_write(struct file* file, const char __user* usr_buf, size_t count, loff_t* pos);
```

执行顺序中先进行的是写操作，这个函数在每次写的时候都会调用。

根据注释我们调用`sscanf()`函数。

```c
char* k_mem;
sscanf(k_mem, "%ld", &l_pid);
```



### proc_read()

```c
static ssize_t proc_read(struct file* file, char* buf, size_t count, loff_t* pos);
```

关键操作是通过`pid_task()`函数来获取进程的信息，获取了之后就正常输出就实现Project

```c
struct task_struct* tsk = NULL;
tsk = pid_task(find_vpid(l_pid), PIDTYPE_PID);

if (tsk == NULL) {
		rv = sprintf(buffer, "command = [No such task] pid = [%ld] state = [No such task]\n", l_pid);
	}
	else {
		rv = sprintf(buffer, "command = [%s] pid = [%ld] state = [%ld]\n", tsk->comm, l_pid, tsk->state);
	}
```

