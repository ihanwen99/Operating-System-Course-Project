#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

struct node *taskList = NULL;

void add(char *name, int priority, int burst) {
    Task *newTask = malloc(sizeof(Task));
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    insert(&taskList, newTask);
}

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

void schedule() {
    while (taskList) {
        Task *task = pickNextTask();
        run(task, task->burst);
        delete(&taskList, task);
    }
}