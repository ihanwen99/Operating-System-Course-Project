#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

int min(int a, int b) { return a < b ? a : b; }

struct node *taskList = NULL;
struct node *nextNode = NULL;
int rr = 20;

void add(char *name, int priority, int burst) {
    Task *newTask = malloc(sizeof(Task));
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    insert(&taskList, newTask);
}

Task *pickNextTask() {
    Task *curTask = nextNode->task;
    nextNode = (nextNode->next) ? nextNode->next : taskList;
    return curTask;
}

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