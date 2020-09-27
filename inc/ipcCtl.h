#ifndef _IPC_CTL_H_
#define _IPC_CTL_H_

#include <sys/ipc.h>
#include "timeKeeper.h"

enum opType {
  ENone = 0,
  EStartCtr,
  EEndCtr,
  EShowInfo,
  ESetName,
  EGetTime,
  ESave,
  EQuit
};

/// type: type of operation
/// idx: which task
/// text: additional data
struct msg {
  int  type;
  int  idx;
  char text[100];
};

key_t initKey(const char *keyfile);
int   initIpc(const char *keyfile, int daemon);
int   exitIpc();
int   waitForMsg(struct msg *message);
int   sendMsg(struct msg message);
int   handleMsg(struct msg message);

#endif
