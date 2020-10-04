/*  _   _                _  __
 * | |_(_)_ __ ___   ___| |/ /___  ___ _ __   ___ _ __
 * | __| | '_ ` _ \ / _ \ ' // _ \/ _ \ '_ \ / _ \ '__|
 * | |_| | | | | | |  __/ . \  __/  __/ |_) |  __/ |
 *  \__|_|_| |_| |_|\___|_|\_\___|\___| .__/ \___|_|
 *   <einKnie@gmx.at>                 |_|
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "taskCtl.h"
#include "ui.h"

taskData_t g_tasks;
int        g_taskInit = 0;

void initTasks() {

  for (int i = 0; i < MAX_IDX; i++) {
    g_tasks.task[i].id      = (i + 1);
    g_tasks.task[i].active  = 0;
    g_tasks.task[i].start   = 0;
    g_tasks.task[i].cum     = 0;
    strncpy(g_tasks.task[i].name, "\0", sizeof(g_tasks.task[i].name));
  }

  g_taskInit = 1;
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

int taskHasName(int idx) {

  if (--idx < 0) {
    return 0;
  }

  return (strlen(g_tasks.task[idx].name) > 0);
}

int setTaskName(int idx, const char *name) {
  if (--idx < 0) return 0;

  strncpy(g_tasks.task[idx].name, name, sizeof(g_tasks.task[idx].name));
  return 1;
}

int showTaskData() {
  char buf[MAX_TEXT*11];
  size_t offs = 0;

  for (int i = 0; i < MAX_IDX; i++) {
    task_t *t = &(g_tasks.task[i]);
    getTaskString(t, buf + offs, MAX_TEXT * 2);
    offs = strlen(buf);
  }

  getCumTaskTime(buf + offs, MAX_TEXT);

  notify(buf, 0);
  return 1;
}

int storeTaskData(int idx, const char *file) {
  int ret = 1;

  // open storage file in append mode
  // first line: current date/time
  // then: add task data

  int fd = -1;
  if ((fd = open(file, O_CREAT | O_APPEND | O_RDWR, 0666)) < 0) {
    printf("could not open savefile: %s\n", strerror(errno));
    return 0;
  }

  time_t now;
  char buf[MAX_TEXT*2];
  struct tm *tm_info;

  now = time(NULL);
  tm_info = localtime(&now);
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
  if (write(fd, buf, strlen(buf)) < (int)strlen(buf)) {
    printf("error: Failed to write to savefile\n");
    ret = 0;
  }

  if (idx > 0) {
    idx -= 1;
    task_t *t = &(g_tasks.task[idx]);
    getTaskString(t, buf, sizeof(buf));
    write(fd, buf, strlen(buf));
  } else {
    for(int i = 0; i < MAX_IDX; i++) {
      task_t *t = &(g_tasks.task[i]);
      getTaskString(t, buf, sizeof(buf));
      if (write(fd, buf, strlen(buf)) < (int)strlen(buf)) {
        printf("error: Failed to write to savefile\n");
        ret = 0;
      }
    }
  }

  getCumTaskTime(buf, sizeof(buf));
  if (write(fd, buf, strlen(buf)) < (int)strlen(buf)) {
    printf("error: Failed to write to savefile\n");
    ret = 0;
  }

  snprintf(buf, sizeof(buf), "\n-------------------\n");
  if (write(fd, buf, strlen(buf)) < (int)strlen(buf)) {
    printf("error: Failed to write to savefile\n");
    ret = 0;
  }
  close(fd);
  return ret;
}

void getTaskString(task_t *t, char *buf, size_t n) {
  int restart = 0;
  char active[4] = "";

  if (t->id == g_tasks.active) {
    restart = 1;
    strncpy(active, "[x]", 4);
    stopTask(t->id);
  }

  int h = (int)t->cum / 3600;
  int m = ((int)t->cum % 3600) / 60;
  int s = (int)t->cum % 60;
  snprintf(buf, n, "\nTask %d %02d:%02d:%02d '%s' %s", \
            t->id, h, m, s, t->name, active);

  if (restart) {
    startTask(t->id);
  }
}

int getCumTaskTime(char *buf, size_t n) {
  int cumtime = 0;

  for(int i = 0; i < MAX_IDX; i++) {
    int restart = 0;
    task_t *t = &(g_tasks.task[i]);

    if (t->id == g_tasks.active) {
      restart = 1;
      stopTask(t->id);
    }

    cumtime += t->cum;

    if (restart) {
      startTask(t->id);
    }
  }

  int h = cumtime / 3600;
  int m = (cumtime % 3600) / 60;
  int s = cumtime % 60;
  snprintf(buf, n, "\ntotal: %02d:%02d:%02d", h, m, s);
  return cumtime;
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
