/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Channel Management
 * 
 * Copyright (C) 1999, Mark Spencer
 *
 * Mark Spencer <markster@linux-support.net>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */

/* DB3 is licensed under Sleepycat Public License and is thus incompatible
   with GPL.  To avoid having to make another exception (and complicate 
   licensing even further) we elect to use DB1 which is BSD licensed */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <asterisk/channel.h>
#include <asterisk/file.h>
#include <asterisk/app.h>
#include <asterisk/dsp.h>
#include <asterisk/logger.h>
#include <asterisk/options.h>
#include <asterisk/astdb.h>
#include <asterisk/cli.h>
#include "db1-ast/include/db.h"
#include "asterisk.h"
#include "astconf.h"

static DB *astdb;
static pthread_mutex_t dblock = AST_MUTEX_INITIALIZER;

static int dbinit(void) 
{
	if (!astdb) {
		if (!(astdb = dbopen((char *)ast_config_AST_DB, O_CREAT | O_RDWR, 0664, DB_BTREE, NULL))) {
			ast_log(LOG_WARNING, "Unable to open Asterisk database\n");
		}
	}
	if (astdb)
		return 0;
	return -1;
}


static inline int keymatch(const char *key, const char *prefix)
{
	if (!strlen(prefix))
		return 1;
	if (!strcasecmp(key, prefix))
		return 1;
	if ((strlen(key) > strlen(prefix)) &&
		!strncasecmp(key, prefix, strlen(prefix))) {
		if (key[strlen(prefix)] == '/')
			return 1;
	}
	return 0;
}

int ast_db_deltree(const char *family, const char *keytree)
{
	char prefix[256];
	DBT key, data;
	char *keys;
	int res;
	int pass;
	
	if (family) {
		if (keytree)
			snprintf(prefix, sizeof(prefix), "/%s/%s", family, keytree);
		else
			snprintf(prefix, sizeof(prefix), "/%s", family);
	} else if (keytree)
		return -1;
	else
		strcpy(prefix, "");
	
	ast_pthread_mutex_lock(&dblock);
	if (dbinit()) 
		return -1;
	
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	pass = 0;
	while(!(res = astdb->seq(astdb, &key, &data, pass++ ? R_NEXT : R_FIRST))) {
		if (key.size) {
			keys = key.data;
			keys[key.size - 1] = '\0';
		} else
			keys = "<bad key>";
		if (keymatch(keys, prefix)) {
			astdb->del(astdb, &key, 0);
		}
	}
	astdb->sync(astdb, 0);
	ast_pthread_mutex_unlock(&dblock);
	return 0;
}

int ast_db_put(const char *family, const char *keys, char *value)
{
	char fullkey[256];
	DBT key, data;
	int res;

	ast_pthread_mutex_lock(&dblock);
	if (dbinit()) {
		ast_pthread_mutex_unlock(&dblock);
		return -1;
	}

	snprintf(fullkey, sizeof(fullkey), "/%s/%s", family, keys);
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data = fullkey;
	key.size = strlen(fullkey) + 1;
	data.data = value;
	data.size = strlen(value) + 1;
	res = astdb->put(astdb, &key, &data, 0);
	astdb->sync(astdb, 0);
	ast_pthread_mutex_unlock(&dblock);
	if (res)
		ast_log(LOG_WARNING, "Unable to put value '%s' for key '%s' in family '%s'\n", value, keys, family);
	return res;
}

int ast_db_get(const char *family, const char *keys, char *value, int valuelen)
{
	char fullkey[256];
	DBT key, data;
	int res;

	ast_pthread_mutex_lock(&dblock);
	if (dbinit()) {
		ast_pthread_mutex_unlock(&dblock);
		return -1;
	}

	snprintf(fullkey, sizeof(fullkey), "/%s/%s", family, keys);
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data = fullkey;
	key.size = strlen(fullkey) + 1;
	
	res = astdb->get(astdb, &key, &data, 0);
	
	ast_pthread_mutex_unlock(&dblock);

	/* Be sure to NULL terminate our data either way */
	if (res) {
		value[0] = 0;
		ast_log(LOG_DEBUG, "Unable to find key '%s' in family '%s'\n", keys, family);
	} else {
#if 0
		printf("Got value of size %d\n", data.size);
#endif
		if (data.size) {
			((char *)data.data)[data.size - 1] = '\0';
			strncpy(value, data.data, valuelen - 1);
			value[valuelen - 1] = '\0';
		} else {
			ast_log(LOG_NOTICE, "Strange, empty value for /%s/%s\n", family, keys);
			value[0] = '\0';
		}
	}
	return res;
}

int ast_db_del(const char *family, const char *keys)
{
	char fullkey[256];
	DBT key;
	int res;

	ast_pthread_mutex_lock(&dblock);
	if (dbinit()) {
		ast_pthread_mutex_unlock(&dblock);
		return -1;
	}
	
	snprintf(fullkey, sizeof(fullkey), "/%s/%s", family, keys);
	memset(&key, 0, sizeof(key));
	key.data = fullkey;
	key.size = strlen(fullkey) + 1;
	
	res = astdb->del(astdb, &key, 0);
	astdb->sync(astdb, 0);
	
	ast_pthread_mutex_unlock(&dblock);

	if (res) 
		ast_log(LOG_DEBUG, "Unable to find key '%s' in family '%s'\n", keys, family);
	return res;
}

static int database_put(int fd, int argc, char *argv[])
{
	int res;
	if (argc != 5)
		return RESULT_SHOWUSAGE;
	res = ast_db_put(argv[2], argv[3], argv[4]);
	if (res) 
		ast_cli(fd, "Failed to update entry\n");
	else
		ast_cli(fd, "Updated database successfully\n");
	return RESULT_SUCCESS;
}

static int database_get(int fd, int argc, char *argv[])
{
	int res;
	char tmp[256];
	if (argc != 4)
		return RESULT_SHOWUSAGE;
	res = ast_db_get(argv[2], argv[3], tmp, sizeof(tmp));
	if (res) 
		ast_cli(fd, "Database entry not found.\n");
	else
		ast_cli(fd, "Value: %s\n", tmp);
	return RESULT_SUCCESS;
}

static int database_del(int fd, int argc, char *argv[])
{
	int res;
	if (argc != 4)
		return RESULT_SHOWUSAGE;
	res = ast_db_del(argv[2], argv[3]);
	if (res) 
		ast_cli(fd, "Database entry does not exist.\n");
	else
		ast_cli(fd, "Database entry removed.\n");
	return RESULT_SUCCESS;
}

static int database_deltree(int fd, int argc, char *argv[])
{
	int res;
	if ((argc < 3) || (argc > 4))
		return RESULT_SHOWUSAGE;
	if (argc == 4)
		res = ast_db_deltree(argv[2], argv[3]);
	else
		res = ast_db_deltree(argv[2], NULL);
	if (res) 
		ast_cli(fd, "Database entries do not exist.\n");
	else
		ast_cli(fd, "Database entries removed.\n");
	return RESULT_SUCCESS;
}

static int database_show(int fd, int argc, char *argv[])
{
	char prefix[256];
	DBT key, data;
	char *keys, *values;
	int res;
	int pass;

	if (argc == 4) {
		/* Family and key tree */
		snprintf(prefix, sizeof(prefix), "/%s/%s", argv[2], argv[3]);
	} else if (argc == 3) {
		/* Family only */
		snprintf(prefix, sizeof(prefix), "/%s", argv[2]);
	} else if (argc == 2) {
		/* Neither */
		strcpy(prefix, "");
	} else
		return RESULT_SHOWUSAGE;
	ast_pthread_mutex_lock(&dblock);
	if (dbinit()) {
		ast_pthread_mutex_unlock(&dblock);
		ast_cli(fd, "Database unavailable\n");
		return RESULT_SUCCESS;	
	}
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	pass = 0;
	while(!(res = astdb->seq(astdb, &key, &data, pass++ ? R_NEXT : R_FIRST))) {
		if (key.size) {
			keys = key.data;
			keys[key.size - 1] = '\0';
		} else
			keys = "<bad key>";
		if (data.size) {
			values = data.data;
			values[data.size - 1]='\0';
		} else
			values = "<bad value>";
		if (keymatch(keys, prefix)) {
				ast_cli(fd, "%-50s: %-25s\n", keys, values);
		}
	}
	ast_pthread_mutex_unlock(&dblock);
	return RESULT_SUCCESS;	
}

struct ast_db_entry *ast_db_gettree(const char *family, const char *keytree)
{
	char prefix[256];
	DBT key, data;
	char *keys, *values;
	int res;
	int pass;
	struct ast_db_entry *last = NULL;
	struct ast_db_entry *cur, *ret=NULL;

	if (family && strlen(family)) {
		if (keytree && strlen(keytree))
			/* Family and key tree */
			snprintf(prefix, sizeof(prefix), "/%s/%s", family, prefix);
		else
			/* Family only */
			snprintf(prefix, sizeof(prefix), "/%s", family);
	} else
		strcpy(prefix, "");
	ast_pthread_mutex_lock(&dblock);
	if (dbinit()) {
		ast_pthread_mutex_unlock(&dblock);
		ast_log(LOG_WARNING, "Database unavailable\n");
		return NULL;	
	}
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	pass = 0;
	while(!(res = astdb->seq(astdb, &key, &data, pass++ ? R_NEXT : R_FIRST))) {
		if (key.size) {
			keys = key.data;
			keys[key.size - 1] = '\0';
		} else
			keys = "<bad key>";
		if (data.size) {
			values = data.data;
			values[data.size - 1]='\0';
		} else
			values = "<bad value>";
		if (keymatch(keys, prefix)) {
				cur = malloc(sizeof(struct ast_db_entry) + strlen(keys) + strlen(values) + 2);
				if (cur) {
					cur->next = NULL;
					cur->key = cur->data + strlen(values) + 1;
					strcpy(cur->data, values);
					strcpy(cur->key, keys);
					if (last)
						last->next = cur;
					else
						ret = cur;
					last = cur;
				}
		}
	}
	ast_pthread_mutex_unlock(&dblock);
	return ret;	
}

void ast_db_freetree(struct ast_db_entry *dbe)
{
	struct ast_db_entry *last;
	while(dbe) {
		last = dbe;
		dbe = dbe->next;
		free(last);
	}
}

static char database_show_usage[] =
"Usage: database show [family [keytree]]\n"
"       Shows Asterisk database contents, optionally restricted\n"
"to a given family, or family and keytree.\n";

static char database_put_usage[] =
"Usage: database put <family> <key> <value>\n"
"       Adds or updates an entry in the Asterisk database for\n"
"a given family, key, and value.\n";

static char database_get_usage[] =
"Usage: database get <family> <key>\n"
"       Retrieves an entry in the Asterisk database for a given\n"
"family and key.\n";

static char database_del_usage[] =
"Usage: database del <family> <key>\n"
"       Deletes an entry in the Asterisk database for a given\n"
"family and key.\n";

static char database_deltree_usage[] =
"Usage: database deltree <family> [keytree]\n"
"       Deletes a family or specific keytree within a family\n"
"in the Asterisk database.\n";

struct ast_cli_entry cli_database_show =
{ { "database", "show", NULL }, database_show, "Shows database contents", database_show_usage };

struct ast_cli_entry cli_database_get =
{ { "database", "get", NULL }, database_get, "Gets database value", database_get_usage };

struct ast_cli_entry cli_database_put =
{ { "database", "put", NULL }, database_put, "Adds/updates database value", database_put_usage };

struct ast_cli_entry cli_database_del =
{ { "database", "del", NULL }, database_del, "Removes database key/value", database_del_usage };

struct ast_cli_entry cli_database_deltree =
{ { "database", "deltree", NULL }, database_deltree, "Removes database keytree/values", database_deltree_usage };

int astdb_init(void)
{
	dbinit();
	ast_cli_register(&cli_database_show);
	ast_cli_register(&cli_database_get);
	ast_cli_register(&cli_database_put);
	ast_cli_register(&cli_database_del);
	ast_cli_register(&cli_database_deltree);
	return 0;
}
