#ifndef _TASK_CTL_H_
#define _TASK_CTL_H_

#include <time.h>
#include "timeKeeper.h"

typedef struct task {
  int     id;             ///< task id
  int     active;         ///< is task running
  char    name[MAX_TEXT]; ///< task name
  time_t  start;          ///< current task start time (if running)
  time_t  cum;            ///< cumulated task runtime
} task_t; ///< data of a single task

typedef struct taskData {
  int    active;          ///< index of active task
  task_t task[MAX_IDX];   ///< tasks
} taskData_t; ///< collective task data



/// Initialize task structs
int initTasks();

/// Switch to task \a idx
/// @param idx index of next task. if 0, stop all tasks
/// @return 1 on success, 0 on failure
/// @note: any previously active task is stopped
int switchToTask(int idx);

int setTaskName(int idx, const char *name);

/// Return the cumulative time from task \a idx
/// @todo: think about this: do we need it?
int getTaskTime(int idx);

/// Write data of task \idx to file
/// @param idx index of task, if 0, write all tasks
/// @return 1 on success, 0 on failure
int storeTaskData(int idx, const char *file);

void writeTask(int fd, task_t *t);

/// Stop a running task
int stopTask(int idx);

// Start a task
int startTask(int idx);


// start a task
// switch task
// end a task (?)
// save task data
// get current cum of task

#endif
