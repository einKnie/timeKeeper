/*  _   _                _  __
 * | |_(_)_ __ ___   ___| |/ /___  ___ _ __   ___ _ __
 * | __| | '_ ` _ \ / _ \ ' // _ \/ _ \ '_ \ / _ \ '__|
 * | |_| | | | | | |  __/ . \  __/  __/ |_) |  __/ |
 *  \__|_|_| |_| |_|\___|_|\_\___|\___| .__/ \___|_|
 *   <einKnie@gmx.at>                 |_|
 */
#ifndef _IPC_CTL_H_
#define _IPC_CTL_H_

#include <sys/ipc.h>
#include "timekeeper.h"

enum opType {
  ENone = 0,
  EStartCtr,
  EEndCtr,
  EShowInfo,
  ESetName,
  ESave,
  EQuit
};

/* type: type of operation
 * idx: which task
 * text: additional data
 *
 * note: this is a bit hacky, since msgsend(...) requires that
 * the send structr has a long int (must be > 0) and a buffer.
 * the size of the buffer alone must be given as msgsz argument. by
 * having two ints here, we can simluate the required long
 * and transmit more data (two flags instead of one) in the meantime.
 * but: this also means that not both type and idx can be 0 at the same time
 */
struct msg {
  int  type;
  int  idx;
  char text[MAX_TEXT];
};

/// Initialize msgQueue
/// @param [in] daemon 1 if daemon process, 0 if client
/// @return 0 on error, 1 on success
int initIpc(int daemon);

/// Close && cleanup msgQueue
/// @return 0 on error, 1 on success
int exitIpc();

/// Wait for a message via msgQueue
/// @param [in,out] message holds received message after successful call
/// @return 0 on error, 1 on success
int waitForMsg(struct msg *message);

/// Send a message via msgQueue
/// @param [in] message
/// @return 0 on error, 1 on success
int sendMsg(struct msg message);

/// Handle a message
/// @param [in] message
/// @return 0 on error, 1 on success
int handleMsg(struct msg message);

#endif
