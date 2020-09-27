#ifndef _PROC_MGR_H_
#define _PROC_MGR_H_

int checkPidFile(const char *pidfile);
int createPidFile(const char *pidfile);
int cleanupPidFile(const char *pidfile);

#endif
