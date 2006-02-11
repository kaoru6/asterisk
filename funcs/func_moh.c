/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 1999 - 2005, Digium, Inc.
 *
 * Russell Bryant <russelb@clemson.edu> 
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
 * \brief Functions for reading or setting the MusicOnHold class
 *
 * \author Russell Bryant <russelb@clemson.edu> 
 */

#include <stdlib.h>

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include "asterisk/module.h"
#include "asterisk/channel.h"
#include "asterisk/pbx.h"
#include "asterisk/utils.h"
#include "asterisk/stringfields.h"

static char *moh_read(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len)
{
	ast_copy_string(buf, chan->musicclass, len);

	return buf;
}

static void moh_write(struct ast_channel *chan, char *cmd, char *data, const char *value) 
{
	ast_string_field_set(chan, musicclass, value);
}

static struct ast_custom_function moh_function = {
	.name = "MUSICCLASS",
	.synopsis = "Read or Set the MusicOnHold class",
	.syntax = "MUSICCLASS()",
	.desc = "This function will read or set the music on hold class for a channel.\n",
	.read = moh_read,
	.write = moh_write,
};

static char *tdesc = "Music-on-hold dialplan function";

int unload_module(void)
{
        return ast_custom_function_unregister(&moh_function);
}

int load_module(void)
{
        return ast_custom_function_register(&moh_function);
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
