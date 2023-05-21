/*  _   _                _  __
 * | |_(_)_ __ ___   ___| |/ /___  ___ _ __   ___ _ __
 * | __| | '_ ` _ \ / _ \ ' // _ \/ _ \ '_ \ / _ \ '__|
 * | |_| | | | | | |  __/ . \  __/  __/ |_) |  __/ |
 *  \__|_|_| |_| |_|\___|_|\_\___|\___| .__/ \___|_|
 *   <einKnie@gmx.at>                 |_|
 */
#ifndef _UI_H_
#define _UI_H_

#include "timekeeper.h"

#define TASK_DATA_TIMEOUT  10
#define INFO_TIMEOUT       4

/// Send a notification
/// @param [in] text
/// @param [in] t notification timeout in seconds [ 0 == no timeout ]
/// @return 0 on success, <0 on error
int notify(const char *text, int t);

/// Get user input
/// @param [in] prompt
/// @param [in,out] out holds user input after successful call
/// @param [in] n size of \a out buffer
/// @return 0 on success, 1 on user abort, -1 on error
int  getInput(const char *prompt, char *out, size_t n);

/// Show a popup
/// @param [in] text#
/// @return 0 on success, 1 on user abort, -1 on error
int popup(const char *text);

#endif
