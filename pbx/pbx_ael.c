/* 
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2006, Digium, Inc.
 *
 * Steve Murphy <murf@parsetree.com>
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
 * \brief Compile symbolic Asterisk Extension Logic into Asterisk extensions, version 2.
 * 
 */

/*** MODULEINFO
	<depend>res_ael_share</depend>
 ***/

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <regex.h>
#include <sys/stat.h>

#include "asterisk/pbx.h"
#include "asterisk/config.h"
#include "asterisk/module.h"
#include "asterisk/logger.h"
#include "asterisk/cli.h"
#include "asterisk/app.h"
#include "asterisk/callerid.h"
#include "asterisk/ael_structs.h"
#include "asterisk/pval.h"
#ifdef AAL_ARGCHECK
#include "asterisk/argdesc.h"
#endif

/* these functions are in ../ast_expr2.fl */

#define DEBUG_READ   (1 << 0)
#define DEBUG_TOKENS (1 << 1)
#define DEBUG_MACROS (1 << 2)
#define DEBUG_CONTEXTS (1 << 3)

static char *config = "extensions.ael";
static char *registrar = "pbx_ael";
static int pbx_load_module(void);

#ifndef AAL_ARGCHECK
/* for the time being, short circuit all the AAL related structures
   without permanently removing the code; after/during the AAL 
   development, this code can be properly re-instated 
*/

#endif

#ifdef AAL_ARGCHECK
int option_matches_j( struct argdesc *should, pval *is, struct argapp *app);
int option_matches( struct argdesc *should, pval *is, struct argapp *app);
int ael_is_funcname(char *name);
#endif

int check_app_args(pval *appcall, pval *arglist, struct argapp *app);
void check_pval(pval *item, struct argapp *apps, int in_globals);
void check_pval_item(pval *item, struct argapp *apps, int in_globals);
void check_switch_expr(pval *item, struct argapp *apps);
void ast_expr_register_extra_error_info(char *errmsg);
void ast_expr_clear_extra_error_info(void);
struct pval *find_macro(char *name);
struct pval *find_context(char *name);
struct pval *find_context(char *name);
struct pval *find_macro(char *name);
struct ael_priority *new_prio(void);
struct ael_extension *new_exten(void);
void linkprio(struct ael_extension *exten, struct ael_priority *prio);
void destroy_extensions(struct ael_extension *exten);
void set_priorities(struct ael_extension *exten);
void add_extensions(struct ael_extension *exten);
void ast_compile_ael2(struct ast_context **local_contexts, struct pval *root);
void destroy_pval(pval *item);
void destroy_pval_item(pval *item);
int is_float(char *arg );
int is_int(char *arg );
int is_empty(char *arg);

/* static void substitute_commas(char *str); */

static int aeldebug = 0;

/* interface stuff */

/* if all the below are static, who cares if they are present? */

static int pbx_load_module(void)
{
	int errs=0, sem_err=0, sem_warn=0, sem_note=0;
	char *rfilename;
	struct ast_context *local_contexts=NULL, *con;
	struct pval *parse_tree;

	ast_log(LOG_NOTICE, "Starting AEL load process.\n");
	if (config[0] == '/')
		rfilename = (char *)config;
	else {
		rfilename = alloca(strlen(config) + strlen(ast_config_AST_CONFIG_DIR) + 2);
		sprintf(rfilename, "%s/%s", ast_config_AST_CONFIG_DIR, config);
	}
	if (access(rfilename,R_OK) != 0) {
		ast_log(LOG_NOTICE, "File %s not found; AEL declining load\n", rfilename);
		return AST_MODULE_LOAD_DECLINE;
	}
	
	parse_tree = ael2_parse(rfilename, &errs);
	ast_log(LOG_NOTICE, "AEL load process: parsed config file name '%s'.\n", rfilename);
	ael2_semantic_check(parse_tree, &sem_err, &sem_warn, &sem_note);
	if (errs == 0 && sem_err == 0) {
		ast_log(LOG_NOTICE, "AEL load process: checked config file name '%s'.\n", rfilename);
		ast_compile_ael2(&local_contexts, parse_tree);
		ast_log(LOG_NOTICE, "AEL load process: compiled config file name '%s'.\n", rfilename);
		
		ast_merge_contexts_and_delete(&local_contexts, registrar);
		ast_log(LOG_NOTICE, "AEL load process: merged config file name '%s'.\n", rfilename);
		for (con = ast_walk_contexts(NULL); con; con = ast_walk_contexts(con))
			ast_context_verify_includes(con);
		ast_log(LOG_NOTICE, "AEL load process: verified config file name '%s'.\n", rfilename);
	} else {
		ast_log(LOG_ERROR, "Sorry, but %d syntax errors and %d semantic errors were detected. It doesn't make sense to compile.\n", errs, sem_err);
		destroy_pval(parse_tree); /* free up the memory */
		return AST_MODULE_LOAD_DECLINE;
	}
	destroy_pval(parse_tree); /* free up the memory */
	
	return AST_MODULE_LOAD_SUCCESS;
}

/* CLI interface */
static char *handle_cli_ael_debug_multiple(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a)
{
	switch (cmd) {
	case CLI_INIT:
		e->command = "ael debug [read|tokens|macros|contexts|off]";
		e->usage =
			"Usage: ael debug [read|tokens|macros|contexts|off]\n"
			"       Enable AEL read, token, macro, or context debugging,\n"
			"       or disable all AEL debugging messages.  Note: this\n"
			"       currently does nothing.\n";
		return NULL;
	case CLI_GENERATE:
		return NULL;
	}

	if (a->argc != 3)
		return CLI_SHOWUSAGE;

	if (!strcasecmp(a->argv[2], "read"))
		aeldebug |= DEBUG_READ;
	else if (!strcasecmp(a->argv[2], "tokens"))
		aeldebug |= DEBUG_TOKENS;
	else if (!strcasecmp(a->argv[2], "macros"))
		aeldebug |= DEBUG_MACROS;
	else if (!strcasecmp(a->argv[2], "contexts"))
		aeldebug |= DEBUG_CONTEXTS;
	else if (!strcasecmp(a->argv[2], "off"))
		aeldebug = 0;
	else
		return CLI_SHOWUSAGE;

	return CLI_SUCCESS;
}

static char *handle_cli_ael_reload(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a)
{
	switch (cmd) {
	case CLI_INIT:
		e->command = "ael reload";
		e->usage =
			"Usage: ael reload\n"
			"       Reloads AEL configuration.\n";
		return NULL;
	case CLI_GENERATE:
		return NULL;
	}

	if (a->argc != 2)
		return CLI_SHOWUSAGE;

	return (pbx_load_module() ? CLI_FAILURE : CLI_SUCCESS);
}

static struct ast_cli_entry cli_ael[] = {
	AST_CLI_DEFINE(handle_cli_ael_reload,         "Reload AEL configuration"),
	AST_CLI_DEFINE(handle_cli_ael_debug_multiple, "Enable AEL debugging flags")
};

static int unload_module(void)
{
	ast_context_destroy(NULL, registrar);
	ast_cli_unregister_multiple(cli_ael, sizeof(cli_ael) / sizeof(struct ast_cli_entry));
	return 0;
}

static int load_module(void)
{
	ast_cli_register_multiple(cli_ael, sizeof(cli_ael) / sizeof(struct ast_cli_entry));
	return (pbx_load_module());
}

static int reload(void)
{
	return pbx_load_module();
}

#ifdef STANDALONE_AEL
#define AST_MODULE "ael"
int ael_external_load_module(void);
int ael_external_load_module(void)
{
        pbx_load_module();
        return 1;
}
#endif

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "Asterisk Extension Language Compiler",
		.load = load_module,
		.unload = unload_module,
		.reload = reload,
	       );

#ifdef AAL_ARGCHECK
static char *ael_funclist[] =
{
	"AGENT",
	"ARRAY",
	"BASE64_DECODE",
	"BASE64_ENCODE",
	"CALLERID",
	"CDR",
	"CHANNEL",
	"CHECKSIPDOMAIN",
	"CHECK_MD5",
	"CURL",
	"CUT",
	"DB",
	"DB_EXISTS",
	"DUNDILOOKUP",
	"ENUMLOOKUP",
	"ENV",
	"EVAL",
	"EXISTS",
	"FIELDQTY",
	"FILTER",
	"GROUP",
	"GROUP_COUNT",
	"GROUP_LIST",
	"GROUP_MATCH_COUNT",
	"IAXPEER",
	"IF",
	"IFTIME",
	"ISNULL",
	"KEYPADHASH",
	"LANGUAGE",
	"LEN",
	"MATH",
	"MD5",
	"MUSICCLASS",
	"QUEUEAGENTCOUNT",
	"QUEUE_MEMBER_COUNT",
	"QUEUE_MEMBER_LIST",
	"QUOTE",
	"RAND",
	"REGEX",
	"SET",
	"SHA1",
	"SIPCHANINFO",
	"SIPPEER",
	"SIP_HEADER",
	"SORT",
	"STAT",
	"STRFTIME",
	"STRPTIME",
	"TIMEOUT",
	"TXTCIDNAME",
	"URIDECODE",
	"URIENCODE",
	"VMCOUNT"
};


int ael_is_funcname(char *name)
{
	int s,t;
	t = sizeof(ael_funclist)/sizeof(char*);
	s = 0;
	while ((s < t) && strcasecmp(name, ael_funclist[s])) 
		s++;
	if ( s < t )
		return 1;
	else
		return 0;
}
#endif    
