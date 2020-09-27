#include "taskCtl.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

taskData_t g_tasks;
int        g_taskInit = 0;

int initTasks() {
  int ret = 0;

  for (int i = 0; i < MAX_IDX; i++) {
    g_tasks.task[i].id      = (i + 1);
    g_tasks.task[i].active  = 0;
    g_tasks.task[i].start   = 0;
    g_tasks.task[i].cum     = 0;
    strncpy(g_tasks.task[i].name, "\0", sizeof(g_tasks.task[i].name));
  }

  g_taskInit = 1;
  return ret;
}

int switchToTask(int idx) {
  int ret = 0;

  // stop active task
  if (g_tasks.active != 0) {
    stopTask(g_tasks.active);
  }

  if (idx > 0) {
    // start a specific task
    startTask(idx);
  }

  return ret;
}

int setTaskName(int idx, const char *name) {
  if (--idx < 0) return 0;

  strncpy(g_tasks.task[idx].name, name, sizeof(g_tasks.task[idx].name));
  return 1;
}

int storeTaskData(int idx, const char *file) {
  int ret = 0;

  // open storage file in append mode
  // first line: current date/time
  // then: add task data

  int fd = -1;
  if ((fd = open(file, O_CREAT | O_APPEND | O_RDWR, 0666)) < 0) {
    printf("could not open savefile: %s\n", strerror(errno));
    return 0;
  }

  time_t now;
  char buf[MAX_TEXT];
  struct tm *tm_info;

  now = time(NULL);
  tm_info = localtime(&now);
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
  write(fd, buf, strlen(buf));

  if (idx > 0) {
    idx -= 1;
    task_t *t = &(g_tasks.task[idx]);
    writeTask(fd, t);
  } else {
    for(int i = 0; i < MAX_IDX; i++) {
      task_t *t = &(g_tasks.task[i]);
      writeTask(fd, t);
    }
  }

  write(fd, "\n------------\n", 14); // todo: improve this
  close(fd);
  return ret;
}

void writeTask(int fd, task_t *t) {
  int restart = 0;
  char active[4] = "";
  char buf[MAX_TEXT*2];

  if (t->id == g_tasks.active) {
    restart = 1;
    strncpy(active, "[x]", 4);
    stopTask(t->id);
  }

  int h = (int)t->cum / 3600;
  int m = ((int)t->cum % 3600) / 60;
  int s = (int)t->cum % 60;
  snprintf(buf, sizeof(buf), "\nTask %d;\"%s\";%02d:%02d:%02d %s", t->id, t->name, h, m, s, active);

  write(fd, buf, strlen(buf));
  if (restart) {
    startTask(t->id);
  }
}

int stopTask(int idx) {

  printf("stopping task %d\n", idx);

  if (idx != g_tasks.active) {
    printf("not stopping an already stopped task\n");
    return 1;
  }

  // task array starts at 0
  idx -= 1;

  time_t now = time(NULL);
  g_tasks.task[idx].cum += (now - g_tasks.task[idx].start);
  g_tasks.task[idx].active = 0;

  g_tasks.active = 0;

  return 1;
}

int startTask(int idx) {

  printf("starting task %d\n", idx);

  if (idx == g_tasks.active) {
    printf("not starting active task again\n");
    return 1;
  }

  // task array starts at 0
  idx -= 1;

  g_tasks.task[idx].start  = time(NULL);
  g_tasks.task[idx].active = 1;

  g_tasks.active = g_tasks.task[idx].id;

  return 1;
}
