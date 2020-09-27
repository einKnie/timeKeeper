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
#include "ipcCtl.h"

void cleanup(void);
void sigHdl(const int signum);
void printHelp();

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
  int type = ENone;
  int idx  = 0;
  char text[MAX_TEXT] = "\0";

  int opt;
  while ((opt = getopt(argc, argv, "t:s:xh")) != -1) {
    switch(opt) {
      case 't':
        type = EStartCtr;
        idx = atoi(optarg);
        break;
      case 's':
        type = EStopCtr;
        idx = atoi(optarg);
        break;
      case 'x':
        type = ESave;
        idx = 0;
        break;
      case 'h':
        printHelp();
        exit(0);
        break;
      default: printf("Error: invalid parameter: %c\n", opt); exit(1);
    }
  }

  // verify params
  if (type < ESave) {
    if ((idx < MIN_IDX) || (idx > MAX_IDX)) {
      printf("error: invalid parameter given\n");
      printHelp();
      exit(1);
    }
  }

  // get pidfile path
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
    // this is the new daemon
    g_isDaemon = 1;
    if (! createPidFile(g_pidfile)) {
      printf("error: failed to create pid file\n");
      exit(1);
    }

  } else {
    printf("got pid of daemon: %d\n", pid);
  }

  initIpc(g_pidfile, g_isDaemon);

  if (g_isDaemon) {
    // daemon loop
    struct msg message;

    while (1) {
      // daemon loop: wait for ipc here
      sleep(1);
      waitForMsg(&message);
    }
  } else {
    // client stuff
    struct msg message;
    message.type = type;
    message.idx  = idx;
    sprintf(message.text, text);

    sendMsg(message);
  }

  return 0;
}

// ----- signal handling ---

void sigHdl(const int signum) {
  exit(signum);
}

void cleanup(void) {
  printf("Cleaning up...\n");
  if (g_isDaemon) {
    if (! exitIpc()) {
      printf("error: Failed to remove message queue\n");
    }
    if (! cleanupPidFile(g_pidfile)) {
      printf("error: Failed to remove pid file at: %s\n", g_pidfile);
    }
  }
}

// ----- help -----
void printHelp() {
  printf("timeKeeper v.%d\n", VERSION);
  printf("\ttimeKeeper [-t <no> -s <no> -x -h]\n");
  printf("-t <no>\t... start tracking time of task <no>\n");
  printf("-s <no>\t... stop tracking time of task <no>\n");
  printf("-x     \t... write data to file and stop daemon\n");
  printf("-h     \t... print this help message and exit\n");
  printf("\n");
  printf("Note: <no> must be an integer in range %d to %d\n", MIN_IDX, MAX_IDX);
}
