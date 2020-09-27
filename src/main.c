/*  _   _                _  __
 * | |_(_)_ __ ___   ___| |/ /___  ___ _ __   ___ _ __
 * | __| | '_ ` _ \ / _ \ ' // _ \/ _ \ '_ \ / _ \ '__|
 * | |_| | | | | | |  __/ . \  __/  __/ |_) |  __/ |
 *  \__|_|_| |_| |_|\___|_|\_\___|\___| .__/ \___|_|
 *   <einKnie@gmx.at>                 |_|
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "timeKeeper.h"
#include "procCtl.h"
#include "ipcCtl.h"
#include "taskCtl.h"

void cleanup(void);
void sigHdl(const int signum);
void printHelp();

static const char procname[] = "timeKeeper";

// declared as extern in timeKeeper.h
char g_pidfile[PATH_MAX] = "\0";
char g_savefile[PATH_MAX] = "\0";
int  g_logfd = -1;

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
  int  type = ENone;
  int  idx  = 0;
  char text[MAX_TEXT] = "\0";

  int opt;
  while ((opt = getopt(argc, argv, "t:n:sevxqh")) != -1) {
    switch(opt) {
      case 't':
        idx = atoi(optarg);
        break;
      case 's':
        type = EStartCtr;
        break;
      case 'e':
        type = EEndCtr;
        break;
      case 'v':
        type = EShowInfo;
        break;
      case 'n':
        type = ESetName;
        strncpy(text, optarg, sizeof(text));
        break;
      case 'x':
        type = ESave;
        idx = 0;
        break;
      case 'q':
        type = EQuit;
        break;
      case 'h':
        printHelp();
        exit(0);
        break;
      default: printf("Error: invalid parameter: %c\n", opt); exit(1);
    }
  }

  int err = 0;
  if ((idx != 0) && (type == ENone)) {
    // no action
    printf("error: no action specified for task %d\n", idx);
    err++;
  }
  if (((type == EStartCtr) || (type == ESetName)) && (idx == 0)) {
    // action w/o task
    printf("error: no task specified for action\n");
    err++;
  }

  if (err) {
    printf("invalid configuration received\n");
    printHelp();
    exit(1);
  }

  // get pidfile path
  char *user = NULL;
  if ((user = getlogin()) == NULL) {
    printf("error: cannot determine current user\n");
    exit(1);
  }

  snprintf(g_pidfile, sizeof(g_pidfile), "/home/%s/.%s.pid", user, procname);
  snprintf(g_savefile, sizeof(g_savefile), "/home/%s/.%s.dat", user, procname);
  printf("Checking pidfile at %s\n", g_pidfile);

  // check if a daemon is already running
  int pid = -1;
  if ((pid = checkPidFile(g_pidfile)) < 0) {
    printf("Error: Failed to read pidfile\n");
    exit(1);
  }

  if (pid != 0) {
    printf("got pid of daemon: %d\n", pid);
  } else {
    printf("no daemon found, becoming the new daemon!\n");
    daemonize();
    // we are a daemon now
    g_isDaemon = 1;
    if (! createPidFile(g_pidfile)) {
      printf("error: failed to create pid file\n");
      exit(1);
    }
  }

  initIpc(g_pidfile, g_isDaemon);

  if (g_isDaemon) {
    initTasks();
    struct msg message;

    while (1) {
      // daemon loop: wait for ipc here
      waitForMsg(&message);
      handleMsg(message);
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
  if (g_isDaemon) {
    // cleanup tasks
    switchToTask(0);
    storeTaskData(0, g_savefile);

    if (! exitIpc()) {
      printf("error: Failed to remove message queue\n");
    }
    if (! cleanupPidFile(g_pidfile)) {
      printf("error: Failed to remove pid file at: %s\n", g_pidfile);
    }
  }

  if (g_logfd > -1) {
    printf("process %d exiting\n", getpid());
    close(g_logfd);
  }
}

// ----- help -----

void printHelp() {
  printf("timeKeeper v.%d\n", VERSION);
  printf("\ttimeKeeper [-t <no> -n -s -e -v -x -q -h]\n");
  printf("-t <no> \t... select a task <no>\n");
  printf("-n <str>\t... set name <str> for selected task\n");
  printf("-s      \t... start counter for selected task\n");
  printf("-e      \t... stop any running task counter\n");
  printf("-v      \t... show current task data as notification\n");
  printf("-x      \t... write current task data to file\n");
  printf("-q      \t... stop daemon (writes data to file automatically)\n");
  printf("-h      \t... print this help message and exit\n");
  printf("\n");
  printf("Note: -s and -n require the task being set with -t also\n");
  printf("Note: <no> must be an integer in range %d to %d\n", MIN_IDX, MAX_IDX);
  printf("Note: data is written to file '.%s.dat' in caller's home\n", procname);
  printf("Last note: A logfile of the daemon process is kept at '/tmp/timeKeeper.log'\n");
}
