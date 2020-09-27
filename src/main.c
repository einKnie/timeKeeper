/*
 * TimeKeeper v0.1
 *  base program checks if another process is running
 *  if not: it forks itself
 *  if yes: sends a message with cmd to daemon
 *
 *
*/

// params:
// -h help
// -t <int> ... start tracking task <int>
// -s <int> ... stop tracking task <int>
// -x ... stop

// daemonisation: write own pid to pid file (/var/run/bla.pid)
// - if pid file exists: a daemon is running
// - if pid file does not exist: become daemon and write own pid to file


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <linux/limits.h>
#include <signal.h>

#include "procMgr.h"


void cleanup(void);
void sigHdl(const int signum);

static const char procname[] = "timeKeeper";

char g_pidfile[PATH_MAX] = "\0";
int  g_isDaemon = 0;

int main(int argc, char **argv) {

  // set exit handlers
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_handler = sigHdl;
  sigaction(SIGINT, &act, 0);
  sigaction(SIGTERM, &act, 0);
  atexit(cleanup);

  // parse params
  int opt;
  while ((opt = getopt(argc, argv, "t:s:xh")) != -1) {
    switch(opt) {
      case 't': break;
      case 's': break;
      case 'x': break;
      case 'h': break;
      default: printf("Error: invalid parameter: %c\n", opt); exit(1);
    }
  }

  // check pidfile
  char *user = NULL;
  if ((user = getlogin()) == NULL) {
    printf("error: cannot determine current user\n");
    exit(1);
  }

  snprintf(g_pidfile, sizeof(g_pidfile), "/home/%s/.%s.pid", user, procname);
  printf("Checking pidfile at %s\n", g_pidfile);

  // check if a daemon is already running
  int pid = checkPidFile(g_pidfile);
  if (pid < 0) {
    printf("Error: Failed to read pidfile\n");
    exit(1);
  } else if (pid == 0) {
    printf("no daemon running!\n");
    if (! createPidFile(g_pidfile)) {
      printf("error: failed to create pid file\n");
      exit(1);
    }

    // this is the new daemon
    g_isDaemon = 1;

    while (1) {
      sleep(1);
    }

  } else {
    printf("got pid of daemon: %d\n", pid);
  }

  // if yes:
  // ipc to daemon process with whatever parameter was given

  // if no:
  // do whatever parameter was given, and keep running as daemon
  // i.e. wait for ipc from now on

  return 0;
}

// ----- signal handling ---

void sigHdl(const int signum) {
  exit(signum);
}

void cleanup(void) {
  printf("Cleaning up...\n");
  fflush(stdout);
  if (g_isDaemon) {
    if (! cleanupPidFile(g_pidfile)) {
      printf("error: Failed to remove pid file at: %s\n", g_pidfile);
    }
  }
}
