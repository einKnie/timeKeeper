#ifndef _TIME_KEEPER_H_
#define _TIME_KEEPER_H_

#include <linux/limits.h>

#define VERSION 1
#define MAX_TEXT 100

#define MIN_IDX 1
#define MAX_IDX 5

extern char g_pidfile[PATH_MAX];
extern char g_savefile[PATH_MAX];

extern int  g_logfd;

#endif
