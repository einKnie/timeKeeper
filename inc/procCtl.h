/*  _   _                _  __
 * | |_(_)_ __ ___   ___| |/ /___  ___ _ __   ___ _ __
 * | __| | '_ ` _ \ / _ \ ' // _ \/ _ \ '_ \ / _ \ '__|
 * | |_| | | | | | |  __/ . \  __/  __/ |_) |  __/ |
 *  \__|_|_| |_| |_|\___|_|\_\___|\___| .__/ \___|_|
 *   <einKnie@gmx.at>                 |_|
 */
#ifndef _PROC_MGR_H_
#define _PROC_MGR_H_

int checkPidFile(const char *pidfile);
int createPidFile(const char *pidfile);
int cleanupPidFile(const char *pidfile);

int daemonize();
int rerouteLog();

#endif
