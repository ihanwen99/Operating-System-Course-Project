# Project2 UNIX Shell Programming

## 问题

This project consists of designing a C program to serve as a shell interface that accepts user commands and then executes each command in a separate process. Your implementation will support input and output redirection, as well as pipes as a form of IPC between a pair of commands. Completing this project will involve using the UNIX `fork()`, `exec()`, `wait()`, `dup2()`, and `pipe()` system calls and can be completed on any Linux, UNIX, or macOS system.
​	This project is organized into several parts:

1. Creating the child process and executing the command in the child
2. Providing a history feature
3. Adding support of input and output redirection
4. Allowing the parent and child processes to communicate via a pipe

## 实现

### 读取命令

```c
void getInput(char*);
```

首先定义一个获取输入的函数，传入的参数是`buffer`的地址。

我储存字符串的方式是C语言中标准的格式 `ls -l\n\0`，和我们的输入对应到计算机中的字符一致，并且在最后添加上C语言的字符串结束标志`\0`。

按顺序进行如下操作：

1. 逐个获取(显式)字符并检测长度

   ```c
   /*while loop getchar()*/
   while (len < 256 && ch != '\n') {
       buf[len++] = ch;
       ch = getchar();
   }
   /*check length*/
   if (len == 256) {
       printf("Command is too long \n");
       exit(-1);
   }
   ```

2. 检查命令中是否存在`!!`

   - 之前没有输入过命令`call_recent=0`，就输出提示
   - 之前输入过，就把`recent` 中的命令字符串拷贝到`buf`中

   ```c
   /*check !!*/
   /*from recent copy to buffer*/
   if (buf[0] == '!' && buf[1] == '!') {
       if (call_recent == 0) {
           printf("No command in history.\n");
       }
       memset(buf, 0, sizeof(buf));
       int i = 0;
       for (; i < strlen(recent); i++) {
           buf[i] = recent[i];
       }
       buf[i++] = '\n';
       buf[i] = '\0';
       call_recent = 1;
   }
   ```

3. 正常的情况，完善字符串格式，复制一份给`buf`

   ```c
   /*nomal case*/
   /*deal with string in buffer*/
   else {
       memset(recent, 0, sizeof(recent));
       for (int i = 0; i < strlen(buf); i++) {
           recent[i] = buf[i];
       }
       buf[len++] = '\n';
       buf[len] = '\0';
   }
   ```

### 指令分割

```c
void instructionSegment(char*, int*, char a[][256]);
```

将指令依据空格分割到命令列表`arglist`中，`\n`作为结束的标志。

初始化`char* p = buf; char* q;`

```c
while (1) {
    if (p[0] == '\n') { break; }
    if (p[0] == ' ') { p++; }
    /*p->head;q-move->end;p=q;*/
    else {
        q = p;
        number = 0;
        /*segment*/
        while ((q[0] != ' ') && (q[0] != '\n')) {
            number++;
            q++;
        }
        /*number+1 -> for \0*/
        /*arg have no \n*/
        strncpy(arglist[*argcount], p, number + 1);
        arglist[*argcount][number] = '\0';
        *argcount = *argcount + 1;
        p = q;
    }
}
```

利用`strncpy`函数将特定的值拷贝到`arglist`中。

【拓展】`arglist`格式，已经更新在我的博客中。

【拓展】'\0'和`NULL`的区别，同样更新到我的博客中。

### 执行命令

初始化中重要的参数

- `flag`表示我们判断命令格式正确与否的表示
- `background`是判断是否接受到`&``
- `op` 是指令类型的分割

```c
#define out_redirect 1
#define in_redirect 2
#define have_pip 3

int flag = 0;//judge command 
int background = 0;//seperate process
int op = 0;
```

1. 首先把命令从原本的`arglist`装载到`arg`中，这步是为了后面命令的执行和分离的方便。

   ```c
   /* Copy Command. */
   for (i = 0; i < argcount; i++) {
       arg[i] = (char*)arglist[i];
   }
   arg[argcount] = NULL;
   ```

2. 对`&`的处理

   ```c
   /* Find & -> seperate process */
   for (i = 0; i < argcount; i++) {
       if (strncmp(arg[i], "&", 1) == 0) {
           /* last arg */
           if (i == argcount - 1) {
               background = 1;
               arg[argcount - 1] = NULL;
               break;
           }
           else {
               printf("Wrong Command\n");
           }
       }
   }
   ```

3. 判断不同的操作，用`op`来代表

   同时对于输入输出操作获取文件名。

   ```c
   /* Different Operations. */
   for (i = 0; arg[i] != NULL; i++) {
       if (strcmp(arg[i], ">") == 0) {
           flag++;
           op = out_redirect;
           if (arg[i + 1] == NULL) {
               flag++;//no output files
           }
           file = arg[i + 1];
           arg[i] = NULL;
       }
       if (strcmp(arg[i], "<") == 0) {
           flag++;
           op = in_redirect;
           if (i == 0) {
               flag++;//no input files
           }
           file = arg[i + 1];
           arg[i] = NULL;
       }
       if (strcmp(arg[i], "|") == 0) {
           flag++;
           op = have_pip;
           if (arg[i + 1] == NULL) { flag++; }
           if (i == 0) { flag++; }//wrong position
       }
   }
   ```

   利用flag来判断命令输入正确与否问题。

   ```c
   * Flag -> Wrong Command. */
   if (flag > 1) { printf("Wrong Command\n"); return; }
   ```

	```c
/* pipe & copy instruction. */
if (op == have_pip) {
    for (i = 0; arg[i] != NULL; i++) {
        if (strcmp(arg[i], "|") == 0) {
            arg[i] = NULL;
            int j;
            for (j = i + 1; arg[j] != NULL; j++) {
                argnext[j - i - 1] = arg[j];
            }
            argnext[j - i - 1] = arg[j];
            break;
        }
    }
}
	```

4. 利用子进程开始进入执行。

   对于基础的命令，调用`execvp(arg[0], arg);`进行执行；

   对于输入的操作，利用`dup2(fd, 1);`将标准输出写入到文件；

   对于输出的操作，利用`dup2(fd, 0);`将文件内容导入到标准输入；

   ```c
   /* Classic Fork Error.*/
   if ((pid = fork()) < 0) {
       printf("Fork Error\n");
       return;
   }
   ```
   
   
	```c
   switch (op) {
       case 0:
           if (pid == 0) {
               execvp(arg[0], arg);
               exit(0);
           }
           break;
       case 1:
           if (pid == 0) {
               fd = open(file, O_RDWR | O_CREAT | O_TRUNC, 0644);
               dup2(fd, 1);
               execvp(arg[0], arg);
               exit(0);
           }
           break;
       case 2:
           if (pid == 0) {
               fd = open(file, O_RDONLY);
               dup2(fd, 0);
               execvp(arg[0], arg);
               exit(0);
           }
           break;
   ```
5. 管道的操作

   管道的操作真的很优美

   初始化：管道`int fd2[2];pipe(fd2);`；新的子子进程`pid2=fork();`

   子子进程(childpid[代码中是pid2])执行应该先执行的命令`arg`，将命令的结果（标准输出）传递到管道的写端（关闭读端就是显然的了），最后关闭管道写端，退出进程。

   子进程(原来的pid执行进程)，执行应该后执行的命令`argnext`，从管道的读端获取标准化输入，最后关闭读端。

   ```c
   case 3:
   if(pid==0){
       pid_t pid2;
       int fd2[2];
       pipe(fd2);
   
   	pid2=fork();
       if(pid2<0){
           fprintf(stderr, "Fork Failed");
       }
   
       if(pid2==0){//child 
           dup2(fd2[WRITE_END],1);
           execvp(arg[0], arg);
           close(fd2[READ_END]);
           close(fd2[WRITE_END]);
           exit(0);
       }
       else{//parent
   		dup2(fd2[READ_END],0);
           execvp (argnext[0],argnext);
           close(fd2[WRITE_END]);
           close(fd2[READ_END]);
           exit(0);
       }
   }
   break;
   ```

6. 处理并发Concurrency

   这部分的并发效果与Ubuntu16.04中的terminal效果一致。

   ```c
   /*Parent Process*/
   if(background){
       printf("[Concurrency process id %d]\n", pid);
       return;
   }
   
   /*background==0(no &)*/
   /*wait*/
   if (waitpid(pid, &status, 0) == -1){
       printf("Wait for child process error.\n");
   }
   ```

   之前检测到的输入`&`反映在`background==1`，表现出返回原来主函数的操作。

   进入到后面代码的前提`background!=0`，利用`waitpid(pid, &status, 0)`等待执行命令的子进程执行完毕。