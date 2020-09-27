#include "procMgr.h"
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/// Check if a given pidfile exists
/// @return 0 if no file exists, -1 on error, and file content ( == pid of daemon) on success
int checkPidFile(const char *pidfile) {
  int ret = 0;
  int fd = -1;
  char buf[60] = "\0";

  if ((fd = open(pidfile, O_RDONLY)) < 0) {
    printf("%d: %s\n", errno, strerror(errno));
  } else {
    int rd = 0;
    if ((rd = read(fd, buf, sizeof(buf))) < 0) {
      printf("Failed to read pidfile: %s\n", strerror(errno));
      ret = -1;
    } else {
      buf[rd] = '\0';
      printf("read pid: %s\n", buf);
      if ((ret = atoi(buf)) == 0) {
        ret = -1;
      }
    }
    close(fd);
  }
  return ret;
}

/// Create a pidfile
/// @return 0 on failure, 1 on success
int createPidFile(const char *pidfile) {
  int  ret     = 0;
  int  fd      = -1;
  char buf[60] = "\0";
  int  pid     = getpid();

  if ((fd = open(pidfile, O_CREAT | O_RDWR, 0644)) < 0) {
    printf("%s\n", strerror(errno));
    return ret;
  }

  sprintf(buf, "%d", pid);
  if (write(fd, buf, sizeof(buf)) < 0) {
    printf("%s\n", strerror(errno));
    ret = 0;
  } else {
    printf("Created pid file\n");
    ret = 1;
  }

  close(fd);
  return ret;
}

/// Remove a pidfile
/// @return 0 on failure, 1 on success
int cleanupPidFile(const char *pidfile) {
  if (remove(pidfile) < 0) {
    printf("error: failed to remove pid file\n");
    return 0;
  } else {
    printf("removed pid file\n");
    return 1;
  }
}
