/*  _   _                _  __
 * | |_(_)_ __ ___   ___| |/ /___  ___ _ __   ___ _ __
 * | __| | '_ ` _ \ / _ \ ' // _ \/ _ \ '_ \ / _ \ '__|
 * | |_| | | | | | |  __/ . \  __/  __/ |_) |  __/ |
 *  \__|_|_| |_| |_|\___|_|\_\___|\___| .__/ \___|_|
 *   <einKnie@gmx.at>                 |_|
 */
#ifndef _TIME_KEEPER_H_
#define _TIME_KEEPER_H_

#include <linux/limits.h>

#define VERSION 1
#define MAX_TEXT 100

#define MIN_IDX 1                 ///< min task index
#define MAX_IDX 5                 ///< max task index [ increase this to add more tasks ]
#define PROCNAME "timeKeeper"     ///< name used for all files (pid, save, log)

extern char g_pidfile[PATH_MAX];  ///< pidfile path
extern char g_savefile[PATH_MAX]; ///< savefile path
extern char g_logfile[PATH_MAX];  ///< logfile path

extern int  g_logfd;              ///< logfile file descriptor

#endif
