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

#define MIN_IDX 1
#define MAX_IDX 5
#define PROCNAME "timeKeeper"

extern char g_pidfile[PATH_MAX];
extern char g_savefile[PATH_MAX];
extern char g_logfile[PATH_MAX];

extern int  g_logfd;

#endif
