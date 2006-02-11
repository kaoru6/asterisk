/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2005, Digium, Inc.
 * Copyright (C) 2005, Olle E. Johansson, Edvina.net
 * Copyright (C) 2005, Russell Bryant <russelb@clemson.edu> 
 *
 * See http://www.asterisk.org for more information about
 * the Asterisk project. Please do not directly contact
 * any of the maintainers of this project for assistance;
 * the project provides a web site, mailing lists and IRC
 * channels for your use.
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

/*! \file
 *
 * \brief MD5 digest related dialplan functions
 * 
 * \author Olle E. Johansson <oej@edvina.net>
 * \author Russell Bryant <russelb@clemson.edu>
 */

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include "asterisk/module.h"
#include "asterisk/channel.h"
#include "asterisk/pbx.h"
#include "asterisk/logger.h"
#include "asterisk/utils.h"
#include "asterisk/app.h"

static char *md5(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len) 
{
	char md5[33];

	if (ast_strlen_zero(data)) {
		ast_log(LOG_WARNING, "Syntax: MD5(<data>) - missing argument!\n");
		return NULL;
	}

	ast_md5_hash(md5, data);
	ast_copy_string(buf, md5, len);
	
	return buf;
}

static char *checkmd5(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len) 
{
	char newmd5[33];
	char *parse;
	static int deprecated = 0;
	AST_DECLARE_APP_ARGS(args,
		AST_APP_ARG(digest);
		AST_APP_ARG(data);
	);

	if (ast_strlen_zero(data)) {
		ast_log(LOG_WARNING, "Syntax: CHECK_MD5(<digest>,<data>) - missing argument!\n");
		return NULL;
	}

	if (!(parse = ast_strdupa(data)))
		return NULL;
	
	AST_STANDARD_APP_ARGS(args, parse);
	
	if (args.argc < 2) {
		ast_log(LOG_WARNING, "Syntax: CHECK_MD5(<digest>,<data>) - missing argument!\n");
		return NULL;
	}

	if (!deprecated) {
		deprecated = 1;
		ast_log(LOG_WARNING, "CHECK_MD5() is deprecated in Asterisk 1.4 and later.\n");
	}

	ast_md5_hash(newmd5, args.data);

	if (!strcasecmp(newmd5, args.digest) )	/* they match */
		ast_copy_string(buf, "1", len);
	else
		ast_copy_string(buf, "0", len);
	
	return buf;
}

static struct ast_custom_function md5_function = {
	.name = "MD5",
	.synopsis = "Computes an MD5 digest",
	.syntax = "MD5(<data>)",
	.read = md5,
};

static struct ast_custom_function checkmd5_function = {
	.name = "CHECK_MD5",
	.synopsis = "Checks an MD5 digest",
	.desc = "Returns 1 on a match, 0 otherwise\n",
	.syntax = "CHECK_MD5(<digest>,<data>)",
	.read = checkmd5,
};

static char *tdesc = "MD5 digest dialplan functions";

int unload_module(void)
{
        return ast_custom_function_unregister(&md5_function) || ast_custom_function_unregister(&checkmd5_function);
}

int load_module(void)
{
        return ast_custom_function_register(&md5_function) || ast_custom_function_register(&checkmd5_function);
}

char *description(void)
{
	return tdesc;
}

int usecount(void)
{
	return 0;
}

char *key()
{
	return ASTERISK_GPL_KEY;
}
