/*  _   _                _  __
 * | |_(_)_ __ ___   ___| |/ /___  ___ _ __   ___ _ __
 * | __| | '_ ` _ \ / _ \ ' // _ \/ _ \ '_ \ / _ \ '__|
 * | |_| | | | | | |  __/ . \  __/  __/ |_) |  __/ |
 *  \__|_|_| |_| |_|\___|_|\_\___|\___| .__/ \___|_|
 *   <einKnie@gmx.at>                 |_|
 */
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include "timeKeeper.h"
#include "procCtl.h"

int checkPidFile(const char *pidfile) {
  int ret = 0;
  int fd = -1;
  char buf[60] = "\0";

  if ((fd = open(pidfile, O_RDONLY)) < 0) {
    if (errno != ENOENT) printf("%d: %s\n", errno, strerror(errno));
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

int cleanupPidFile(const char *pidfile) {
  if (remove(pidfile) < 0) {
    printf("error: failed to remove pid file\n");
    return 0;
  } else {
    printf("removed pid file\n");
    return 1;
  }
}

void daemonize() {

  pid_t pid = fork();
  switch(pid) {
    case -1:
      printf("Failed to fork: %s\n", strerror(errno));
      exit(1);
    case 0: break;     // child
    default: exit(0);  // parent
  }

  printf("Rerouting all output to logfile at %s\n", g_logfile);
  if (! rerouteLog()) {
    // close all file descriptors
    printf("Failed to reroute stdin&&stdout\n logging disabled\n");
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
      close(x);
    }
  }

  if (setsid() < 0) {
    printf("Failed to create new session: %s\n", strerror(errno));
    exit(1);
  }

  umask(022);
  if ((umask(022) & 022) != 022) {
    printf("Failed to set umask\n");
    exit(1);
  }

  if (chdir("/") != 0) {
    printf("Failed to change directory: %s\n", strerror(errno));
    exit(1);
  }

  printf("daemon initialized\n");
}

int rerouteLog() {
  if (g_logfd < 0) {
    if ((g_logfd = open(g_logfile, O_CREAT | O_APPEND | O_WRONLY, 0644)) < 0) {
      return 0;
    }

    dup2(g_logfd, STDOUT_FILENO);
    dup2(g_logfd, STDERR_FILENO);
    return 1;
  }
  return 0;
}
