/*  _   _                _  __
 * | |_(_)_ __ ___   ___| |/ /___  ___ _ __   ___ _ __
 * | __| | '_ ` _ \ / _ \ ' // _ \/ _ \ '_ \ / _ \ '__|
 * | |_| | | | | | |  __/ . \  __/  __/ |_) |  __/ |
 *  \__|_|_| |_| |_|\___|_|\_\___|\___| .__/ \___|_|
 *   <einKnie@gmx.at>                 |_|
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "ui.h"

void notify(const char *text, int t) {

	int ret = 0;
	char outbuf[MAX_TEXT*12];

	snprintf(outbuf, sizeof(outbuf),
		"notify-send -t %d \"%s\" \"%s\"",
		(t * 1000), PROCNAME, text);
	log_debug("calling:\n%s\n", outbuf);
	if ((ret = system(outbuf)) < 0) {
		log_error("failed to send notification: %d\n", ret);
	}

}

int getInput(const char *prompt, char *out, size_t n) {

	char buf[MAX_TEXT] = "\0";
	snprintf(buf, sizeof(buf), "zenity --entry --text=\"%s\"", prompt);

	FILE *fd = popen(buf, "r");
	fgets(out, n, fd);
	if (strlen(out) < 1) {
		log_debug("user input aborted\n");
		popup("Aborted");
		return 0;
	}
	out[strlen(out) -1] = '\0';
	log_debug("Got user input: %s\n", out);
	return 1;

}

void popup(const char *text) {

	int ret = 0;
	char buf[MAX_TEXT] = "\0";
	snprintf(buf, sizeof(buf), "zenity --info --no-wrap --text=\"%s\"", text);
	if ((ret = system(buf)) < 0) {
		log_error("failed to show popup\n");
	}

}
