/*  _   _                _  __
 * | |_(_)_ __ ___   ___| |/ /___  ___ _ __   ___ _ __
 * | __| | '_ ` _ \ / _ \ ' // _ \/ _ \ '_ \ / _ \ '__|
 * | |_| | | | | | |  __/ . \  __/  __/ |_) |  __/ |
 *  \__|_|_| |_| |_|\___|_|\_\___|\___| .__/ \___|_|
 *   <einKnie@gmx.at>                 |_|
 */
#include <sys/msg.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipc_ctl.h"
#include "task_ctl.h"
#include "ui.h"

struct msgQueue {
  key_t key;
  int   id;
} g_msgQueue;
int g_initQueue = 0;

int initIpc(int daemon) {

  if (g_initQueue) {
    log_debug("queue already initialized. doing nothing\n");
    return 1;
  }

  int   msgid = 0;
  key_t key   = ftok(g_pidfile, VERSION);

  if (daemon) {
    // init ipc as listener
    msgid = msgget(key, 0666 | IPC_CREAT);
  } else {
    // init ipc as sender
    msgid = msgget(key, 0666);
  }

  if (msgid < 0) {
    log_error("Failed to open msgqueue: %s\n", strerror(errno));
  } else {
    log_notice("Message queue created\n");
    g_msgQueue.key = key;
    g_msgQueue.id  = msgid;
    g_initQueue = 1;
  }

  return g_initQueue;
}

int exitIpc() {
  if (! g_initQueue) {
    log_debug("queue not initialized. doing nothing...\n");
    return 1;
  }

  // remove queue
  if (msgctl(g_msgQueue.id, IPC_RMID, NULL) < 0) {
    log_error("failed to remove msgqueue: %s\n", strerror(errno));
    return 0;
  }

  log_notice("removed message queue\n");
  return 1;
}

int waitForMsg(struct msg *message) {

  if (! g_initQueue) {
    log_debug("queue not initialized. doing nothing...\n");
    return 0;
  }

  if ((msgrcv(g_msgQueue.id, message, sizeof(*message), 0, 0)) < 0) {
    log_error("failed to receive message: %s\n", strerror(errno));
    return 0;
  } else {
    log_notice("received message\n");
    log_debug("type: %d\nidx: %d\ntext: %s\n", \
            message->type, message->idx, message->text);
    return 1;
  }
}

int sendMsg(struct msg message) {

  if (! g_initQueue) {
    log_debug("queue not initialized. doing nothing...\n");
    return 0;
  }

  if ((msgsnd(g_msgQueue.id, &message, sizeof(message), 0)) < 0) {
    log_error("failed to send message: %s\n", strerror(errno));
    return 0;
  } else {
    log_debug("sent message\n");
    return 1;
  }
}

int handleMsg(struct msg message) {

  log_debug("handling message\n");

  switch(message.type) {
    case EStartCtr:
      if (! taskHasName(message.idx)) {
        char in[MAX_TEXT] = "\0";
        if (! getInput("Task name:", in, sizeof(in))) {
          break;
        }
        setTaskName(message.idx, in);
      }
      switchToTask(message.idx);
      break;
    case EEndCtr:
      switchToTask(0);
      break;
    case ESetName:
      log_debug("set task name (%s) for task %d\n", message.text, message.idx);
      setTaskName(message.idx, message.text);
      break;
    case EShowInfo:
      log_debug("show info notification\n");
      showTaskData(message.idx);
      break;
    case ESave:
      log_debug("writing data to file %s\n", g_savefile);
      storeTaskData(message.idx, g_savefile);
      break;
    case EQuit:
      log_debug("Shutting down...\n");
      exit(0);
    default:
      log_debug("invalid message received\n");
      return 0;
  }

  return 1;
}
