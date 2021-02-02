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
#include "timekeeper.h"
#include "proc_ctl.h"
#include "ipc_ctl.h"
#include "task_ctl.h"
#include "ui.h"

void cleanup(void);
void sigHdl(const int signum);
void printHelp();
int  validateIdx(int idx, int optional);

// declared as extern in timekeeper.h
char g_pidfile[PATH_MAX]  = "\0";
char g_savefile[PATH_MAX] = "\0";
char g_logfile[PATH_MAX]  = "\0";
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

  logLevel_e log_level = DEFAULT_LOGLEVEL;
  log_init(log_level, DEFAULT_LOGSTYLE, NULL);

  // parse params
  int  type = ENone;
  int  idx  = 0;
  char text[MAX_TEXT] = "\0";

  int opt;
  while ((opt = getopt(argc, argv, "t:n:sevxqhl:")) != -1) {
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
        break;
      case 'q':
        type = EQuit;
        break;
      case 'l':
        log_level = (logLevel_e)atoi(optarg);
        break;
      case 'h':
        printHelp();
        exit(0);
        break;
      default: log_error("invalid parameter: %c\n", opt); exit(1);
    }
  }

  int err = 0;
  switch (type) {
    case EStartCtr:
    case ESetName:
      if (!validateIdx(idx, 1)) {
        err++;
        log_error("task must be supplied in range %d-%d\n", MIN_IDX, MAX_IDX);
      }
      break;
    case ESave:
    case EShowInfo:
      if (!validateIdx(idx, 0)) {
        err++;
      }
      break;
    case EQuit:
    case EEndCtr:
      if (!validateIdx(idx, -1)) {
        err++;
        log_error("Invalid option -t in combination with other option\n");
      }
      break;
    case ENone:
      if (!validateIdx(idx, -1)) {
        err++;
        log_error("no action specified for task %d\n", idx);
      }
      break;
    default:
      log_error("Unrecognized action given\n");
      type = ENone;
      err++;
      break;
  }

  if (err) {
    log_error("invalid configuration received\n\n");
    printHelp();
    exit(1);
  }

  // set updated loglevel
  if (log_level != DEFAULT_LOGLEVEL) {
    log_reinit(log_level, DEFAULT_LOGSTYLE);
  }

  // prepare message
  struct msg message;
  message.type = type;
  message.idx  = idx;
  strncpy(message.text, text, sizeof(message.text));

  // get filepaths
  char *user = NULL;
  if ((user = getlogin()) == NULL) {
    log_error("cannot determine current user\n");
    exit(1);
  }

  snprintf(g_pidfile,  sizeof(g_pidfile),  "/home/%s/.%s.pid", user, PROCNAME);
  snprintf(g_savefile, sizeof(g_savefile), "/home/%s/%s.dat", user, PROCNAME);
  snprintf(g_logfile,  sizeof(g_logfile),  "/tmp/%s.log", PROCNAME);
  log_notice("Checking pidfile at %s\n", g_pidfile);

  // check if a daemon is already running
  int pid = -1;

  do {
    if ((pid = checkPidFile(g_pidfile)) < 0) {
      log_error("failed to read pidfile\n");
      exit(1);
    }

    if (pid == 0) {
      log_notice("no daemon found, becoming the new daemon!\n");
      daemonize();
      g_isDaemon = 1;
      if (!createPidFile(g_pidfile)) {
        log_error("failed to create pid file\n");
        exit(1);
      }
    } else {
      log_notice("got pid of daemon: %d\n", pid);
      if (!checkProcess(pid)) {
        log_notice("daemon not found: cleaning up\n");
        if (!cleanupPidFile(g_pidfile)) {
          log_error("failed to remove pidfile\n");
          exit(1);
        }
        continue;
      }
    }
    break;
  } while (1);

  if (!initIpc(g_isDaemon)) {
    log_error("failed to initialize message queue\n");
    exit(1);
  }

  if (g_isDaemon) {
    notify("Daemon running", 5);
    initTasks();

    // in case daemon was started w/ args, process them
    handleMsg(message);

    while (1) {
      // daemon loop: wait for ipc here
      if (waitForMsg(&message)) {
        handleMsg(message);
      }
    }

  } else {
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

    notify("Shutting down", 5);

    // cleanup tasks
    switchToTask(0);
    storeTaskData(0, g_savefile);

    if (!exitIpc()) {
      log_error("failed to remove message queue\n");
    }
    if (!cleanupPidFile(g_pidfile)) {
      log_error("failed to remove pid file at: %s\n", g_pidfile);
    }
  }

  if (g_logfd > -1) {
    log_always("process %d exiting\n", getpid());
    close(g_logfd);
  }
}

// ----- help -----

void printHelp() {
  printf("%s v.%d.%d\n", PROCNAME, VERSION, MINVERSION);
  printf("keep track of how much time you spend on various tasks\n");
  printf("usage:\n");
  printf("\ttimeKeeper [options]\n");
  printf("-t <no> \t... select a task <no> [%d - %d]\n", MIN_IDX, MAX_IDX);
  printf("-n <str>\t... set name <str> for selected task\n");
  printf("-s      \t... start counter for selected task\n");
  printf("-e      \t... stop any running task counter\n");
  printf("-v      \t... show current task data as notification\n");
  printf("-x      \t... write current task data to file\n");
  printf("-q      \t... stop daemon (writes data to file automatically)\n");
  printf("-h      \t... print this help message and exit\n");
  printf("\n");
  printf("notes:\n");
  printf(" -s and -n require the task being set with -t also\n");
  printf(" -v and -x may be combined with -t to show/print only this task's data\n");
  printf(" all options except -t are mutually exclusive\n");
  printf(" data is written to file '%s.dat' in caller's home\n", PROCNAME);
  printf(" a logfile of the daemon process is kept at '/tmp/%s.log'\n", PROCNAME);
}

/*  Check if provided index is valid
 *  param optional:
 *    -1 ... idx must not be set (i.e. == 0)
 *     0 ... idx may be 0
 *     1 ... idx must be set (i.e. != 0)
*/
int validateIdx(int idx, int optional) {
  int err = 0;

  if ((idx != 0) && ((idx < MIN_IDX) || (idx > MAX_IDX))) {
    // set && out of range
    log_error("task must be supplied in range %d-%d\n", MIN_IDX, MAX_IDX);
    err++;
  }

  if (idx == 0) {
    // not set
    if (optional <= 0) {
      // not set && not required
      return 1;
    }
  } else {
    // set
    if ((optional >= 0) && !err) {
      // allowed && correct
      return 1;
    }
  }

  return 0;
}
