/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Trivial application to playback a sound file
 * 
 * Copyright (C) 1999, Mark Spencer
 *
 * Mark Spencer <markster@linux-support.net>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */
 
#include <asterisk/file.h>
#include <asterisk/logger.h>
#include <asterisk/channel.h>
#include <asterisk/pbx.h>
#include <asterisk/module.h>
#include <asterisk/translate.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

static char *tdesc = "Trivial Playback Application";

static char *app = "Playback";

static char *synopsis = "Play a file";

static char *descrip = 
"  Playback(filename[|option]):  Plays  back  a  given  filename (do not put\n"
"extension). Options may also be  included following a pipe symbol. The only\n"
"defined option at this time is 'skip',  which  causes  the  playback of the\n"
"message to  be  skipped  if  the  channel is not in the 'up' state (i.e. it\n"
"hasn't been  answered  yet. If 'skip' is not specified, the channel will be\n"
"answered before the sound is played. Returns -1 if the channel was hung up,\n"
"or if the file does not exist. Returns 0 otherwise.\n";

STANDARD_LOCAL_USER;

LOCAL_USER_DECL;

static int playback_exec(struct ast_channel *chan, void *data)
{
	int res = 0;
	struct localuser *u;
	char tmp[256];
	char *options;
	int option_skip=0;
	if (!data || !strlen((char *)data)) {
		ast_log(LOG_WARNING, "Playback requires an argument (filename)\n");
		return -1;
	}
	strncpy(tmp, (char *)data, sizeof(tmp));
	strtok(tmp, "|");
	options = strtok(NULL, "|");
	if (options && !strcasecmp(options, "skip"))
		option_skip = 1;
	LOCAL_USER_ADD(u);
	if (chan->state != AST_STATE_UP) {
		if (option_skip) {
			/* At the user's option, skip if the line is not up */
			LOCAL_USER_REMOVE(u);
			return 0;
		} else
			/* Otherwise answer */
			res = ast_answer(chan);
	}
	if (!res) {
		ast_stopstream(chan);
		res = ast_streamfile(chan, tmp, chan->language);
		if (!res) 
			res = ast_waitstream(chan, "");
		else
			ast_log(LOG_WARNING, "ast_streamfile failed on %s for %s\n", chan->name, (char *)data);
		ast_stopstream(chan);
	}
	LOCAL_USER_REMOVE(u);
	return res;
}

int unload_module(void)
{
	STANDARD_HANGUP_LOCALUSERS;
	return ast_unregister_application(app);
}

int load_module(void)
{
	return ast_register_application(app, playback_exec, synopsis, descrip);
}

char *description(void)
{
	return tdesc;
}

int usecount(void)
{
	int res;
	STANDARD_USECOUNT(res);
	return res;
}

char *key()
{
	return ASTERISK_GPL_KEY;
}
