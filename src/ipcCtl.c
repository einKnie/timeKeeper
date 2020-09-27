#include <sys/msg.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "ipcCtl.h"
#include "taskCtl.h"

struct msgQueue {
  key_t key;
  int   id;
} msgQueue;

int g_initQueue = 0;

int initIpc(const char *keyfile, int daemon) {

  if (g_initQueue) {
    printf("queue already initialized. doing nothing\n");
    return 1;
  }

  int   msgid = 0;
  key_t key   = initKey(keyfile);

  if (daemon) {
    // init ipc as listener
    msgid = msgget(key, 0666 | IPC_CREAT);
  } else {
    // init opc as sender
    msgid = msgget(key, 0666);
  }

  if (msgid < 0) {
    printf("Failed to open msgqueue: %s\n", strerror(errno));
  } else {
    printf("Message queue created\n");
    msgQueue.key = key;
    msgQueue.id  = msgid;
    g_initQueue = 1;
  }

  return g_initQueue;
}

int exitIpc() {
  if (! g_initQueue) {
    printf("queue not initialized. doing nothing...\n");
    return 1;
  }

  // remove queue
  if (msgctl(msgQueue.id, IPC_RMID, NULL) < 0) {
    printf("failed to remove msgqueue: %s\n", strerror(errno));
    return 0;
  }

  printf("remove message queue\n");
  return 1;
}

int waitForMsg(struct msg *message) {
  int ret = 0;

  if ((ret = msgrcv(msgQueue.id, message, sizeof(*message), 0, 0)) < 0) {
    printf("failed to receive message: %s\n", strerror(errno));
    ret = 0;
  } else {
    printf("received message\n");
    printf("type: %d\nidx: %d\ntext: %s\n", message->type, message->idx, message->text);
    ret = 1;
  }

  return ret;
}

int sendMsg(struct msg message) {
  int ret = 0;

  if ((ret = msgsnd(msgQueue.id, &message, sizeof(message), 0)) < 0) {
    printf("failed to send message: %s\n", strerror(errno));
    ret = 0;
  } else {
    printf("sent message\n");
    ret = 1;
  }

  return ret;
}

int handleMsg(struct msg message) {
  int ret = 1;
  printf("handling message\n");

  switch(message.type) {
    case EStartCtr:
      switchToTask(message.idx);
      break;
    case ESetName:
      printf("set task name (%s) for task %d\n", message.text, message.idx);
      setTaskName(message.idx, message.text);
      break;
    case EShowInfo:
      printf("show info notification\n");
      showTaskData();
      break;
    case ESave:
      storeTaskData(message.idx, g_savefile);
      break;
    default:
      printf("invalid message received\n");
      return 0;
  }

  return ret;
}

key_t initKey(const char *keyfile) {
  return(ftok(keyfile, VERSION));
}
