/*  _   _                _  __
 * | |_(_)_ __ ___   ___| |/ /___  ___ _ __   ___ _ __
 * | __| | '_ ` _ \ / _ \ ' // _ \/ _ \ '_ \ / _ \ '__|
 * | |_| | | | | | |  __/ . \  __/  __/ |_) |  __/ |
 *  \__|_|_| |_| |_|\___|_|\_\___|\___| .__/ \___|_|
 *   <einKnie@gmx.at>                 |_|
 */
#ifndef _PROC_MGR_H_
#define _PROC_MGR_H_

/// Check if pidfile exists
/// @param [in] pidfile
/// @return 0 if no file exists, -1 on error, pid of daemon on success
int checkPidFile(const char *pidfile);

/// Create a pidfile
/// @param [in] pidfile path
/// @return 0 on failure, 1 on success
int createPidFile(const char *pidfile);

/// Remove a pidfile
/// @param [in] pidfile path
/// @return 0 on failure, 1 on success
int cleanupPidFile(const char *pidfile);

/// Daemonize process
void daemonize();

/// Reroute stdout&&stderr to logfile
/// @return 0 on failure, 1 on success
int rerouteLog();

#endif
