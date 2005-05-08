/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Functions for interaction with the Asterisk database
 * 
 * Copyright (C) 2005, Russell Bryant <russelb@clemson.edu> 
 *
 * func_db.c adapted from the old app_db.c, copyright by the following people 
 * Copyright (C) 2005, Mark Spencer <markster@digium.com>
 * Copyright (C) 2003, Jefferson Noxon <jeff@debian.org>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>

#include "asterisk/channel.h"
#include "asterisk/pbx.h"
#include "asterisk/logger.h"
#include "asterisk/options.h"
#include "asterisk/utils.h"
#include "asterisk/app.h"
#include "asterisk/astdb.h"

static char *function_db_read(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len)
{
	int argc;	
	char *args;
	char *argv[2];
	char *family;
	char *key;

	if (!data || ast_strlen_zero(data)) {
		ast_log(LOG_WARNING, "DB requires an argument, DB(<family>/<key>)\n");
		return buf;
	}

	args = ast_strdupa(data);
	argc = ast_separate_app_args(args, '/', argv, sizeof(argv) / sizeof(argv[0]));
	
	if (argc > 1) {
		family = argv[0];
		key = argv[1];
	} else {
		ast_log(LOG_WARNING, "DB requires an argument, DB(<family>/<key>)\n");
		return buf;
	}

	if (ast_db_get(family, key, buf, len-1)) {
		ast_log(LOG_WARNING, "DB: %s/%s not found in database.\n", family, key);
	}
	
	return buf;
}

static void function_db_write(struct ast_channel *chan, char *cmd, char *data, const char *value) 
{
	int argc;	
	char *args;
	char *argv[2];
	char *family;
	char *key;

	if (!data || ast_strlen_zero(data)) {
		ast_log(LOG_WARNING, "DB requires an argument, DB(<family>/<key>)=<value>\n");
		return;
	}

	args = ast_strdupa(data);
	argc = ast_separate_app_args(args, '/', argv, sizeof(argv) / sizeof(argv[0]));
	
	if (argc > 1) {
		family = argv[0];
		key = argv[1];
	} else {
		ast_log(LOG_WARNING, "DB requires an argument, DB(<family>/<key>)=value\n");
		return;
	}

	if (ast_db_put(family, key, (char*)value)) {
		ast_log(LOG_WARNING, "DB: Error writing value to database.\n");
	}
}

#ifndef BUILTIN_FUNC
static
#endif
struct ast_custom_function db_function = {
	.name = "DB",
	.synopsis = "Read or Write from/to the Asterisk database",
	.syntax = "DB(<family>/<key>)",
	.desc = "This function will read or write a value from/to the Asterisk database."
		"DB(...) will read a value from the database, while DB(...)=value"
		"will write a value to the database.  On a read, this function"
		"returns the value from the datase, or NULL if it does not exist."
		"On a write, this function will always return NULL.",
	.read = function_db_read,
	.write = function_db_write,
};

