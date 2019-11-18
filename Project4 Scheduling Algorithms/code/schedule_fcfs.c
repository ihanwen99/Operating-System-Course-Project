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
    struct node *lastNode = taskList;
    while (lastNode->next) {
        lastNode = lastNode->next;
    }
    return lastNode->task;
}

void schedule() {
    while (taskList) {
        Task *task = pickNextTask();
        run(task, task->burst);
        delete(&taskList, task);
    }
}