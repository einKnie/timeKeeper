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
#include "timekeeper.h"

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
/// @param [in] idx index of next task. if 0, stop all tasks
/// @note: any previously active task is stopped
void switchToTask(int idx);

/// Check if task \a idx has a name
/// @param [in] idx index of task, must be >0
/// @return length of name (0 if no name), <0 on invalid index
int taskHasName(int idx);

/// Set a name for task \a idx
/// @param [in] idx index of task, must be >0
/// @param [in] name
/// @return 0 on success, -1 on invalid idx
int setTaskName(int idx, const char *name);

/// Show a notification containing the current task data
/// @param [in] idx index of task, or 0 to show data of all tasks
void showTaskData(int idx);

/// Write task data to string \a buf from index
/// @param [in] idx index of task, or 0 for data of all tasks
/// @param [in,out] buf holds collected task data after successful call
/// @param [in] n size of provided buffer \a buf
void getStringFromIndex(int idx, char *buf, size_t n);

/// Write task data to \a buf from struct
/// @param [in] t task struct
/// @param [in,out] buf holds collected task data after successful call
/// @param [in] n size of provided buffer \a buf
void getStringFromTask(task_t *t, char *buf, size_t n);

/// Write data of task \idx to file
/// @param idx index of task, or 0 to write all tasks
/// @return 0 on success, >0 on errors
int storeTaskData(int idx, const char *file);

/// Get the cumulative time spent on all tasks
/// @param [in,out] buf holds human-readable time string after successful call
/// @param [in] n size of \a buf
/// @return cumulative time in seconds
int getCumTaskTime(char *buf, size_t n);

/// Stop a running task
/// @param idx index of task
/// @return 0 on success, 1 if process already stopped, -1 on invalid index
int stopTask(int idx);

// Start a task
/// @param idx index of task
/// @return 0 on success, 1 if process already running, -1 on invalid index
int startTask(int idx);

#endif
