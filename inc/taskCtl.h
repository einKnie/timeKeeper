/*  _   _                _  __
 * | |_(_)_ __ ___   ___| |/ /___  ___ _ __   ___ _ __
 * | __| | '_ ` _ \ / _ \ ' // _ \/ _ \ '_ \ / _ \ '__|
 * | |_| | | | | | |  __/ . \  __/  __/ |_) |  __/ |
 *  \__|_|_| |_| |_|\___|_|\_\___|\___| .__/ \___|_|
 *   <einKnie@gmx.at>                 |_|
 */
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
  int    active;          ///< id of active task
  task_t task[MAX_IDX];   ///< tasks
} taskData_t; ///< collective task data


/// Initialize task structs
void initTasks();

/// Switch to task \a idx
/// @param idx index of next task. if 0, stop all tasks
/// @return 1 on success, 0 on failure
/// @note: any previously active task is stopped
int switchToTask(int idx);

/// Check if task \a idx has a name
/// @param [in] idx index of task
/// @return 0 if no name, 1 if name
int taskHasName(int idx);

/// Set a name for task \a idx
/// @param [in] idx index of task
/// @param [in] name
/// @return 0 on failure, 1 on success
int setTaskName(int idx, const char *name);

/// Write data of task \idx to file
/// @param idx index of task, if 0, write all tasks
/// @return 1 on success, 0 on failure
int storeTaskData(int idx, const char *file);

/// Show a notification containing the current task data
int showTaskData();

/// Write task data to \a buf
/// @param [in] t task
/// @param [in,out] buf
/// @param [in] n size of buffer
void getTaskString(task_t *t, char *buf, size_t n);

/// Return the cumulative time spent on all tasks
/// @param [in,out] buf holds stringified time after successful call
/// @param [in] n size of \a buf
/// @returns cumulative time in seconds
int getCumTaskTime(char *buf, size_t n);

/// Stop a running task
int stopTask(int idx);

// Start a task
int startTask(int idx);

#endif
