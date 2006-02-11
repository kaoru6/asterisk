/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2005, Digium, Inc.
 * Portions Copyright (C) 2005, Tilghman Lesher.  All rights reserved.
 * Portions Copyright (C) 2005, Anthony Minessale II
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
 * \brief String manipulation dialplan functions
 *
 * \author Tilghman Lesher
 * \author Anothony Minessale II 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include "asterisk/module.h"
#include "asterisk/channel.h"
#include "asterisk/pbx.h"
#include "asterisk/logger.h"
#include "asterisk/utils.h"
#include "asterisk/app.h"
#include "asterisk/localtime.h"

static char *function_fieldqty(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len)
{
	char *varval, workspace[4096];
	int fieldcount = 0;
	char *parse;
	AST_DECLARE_APP_ARGS(args,
		AST_APP_ARG(varname);
		AST_APP_ARG(delim);
	);

	if (!(parse = ast_strdupa(data))) {
		ast_copy_string(buf, "0", len);
		return buf;
	}

	AST_STANDARD_APP_ARGS(args, parse);
	if (args.delim) {
		pbx_retrieve_variable(chan, args.varname, &varval, workspace, sizeof(workspace), NULL);
		while (strsep(&varval, args.delim))
			fieldcount++;
	} else {
		fieldcount = 1;
	}
	snprintf(buf, len, "%d", fieldcount);

	return buf;
}

static struct ast_custom_function fieldqty_function = {
	.name = "FIELDQTY",
	.synopsis = "Count the fields, with an arbitrary delimiter",
	.syntax = "FIELDQTY(<varname>,<delim>)",
	.read = function_fieldqty,
};

static char *filter(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len)
{
	char *parse;
	AST_DECLARE_APP_ARGS(args,
		AST_APP_ARG(allowed);
		AST_APP_ARG(string);
	);
	char *outbuf=buf;

	if (!(parse = ast_strdupa(data)))
		return "";

	AST_STANDARD_APP_ARGS(args, parse);

	if (!args.string ) {
		ast_log(LOG_ERROR, "Usage: FILTER(<allowed-chars>,<string>)\n");
		return "";
	}

	for ( ; *(args.string) && (buf + len - 1 > outbuf); (args.string)++) {
		if (strchr(args.allowed, *(args.string))) {
			*outbuf = *(args.string);
			outbuf++;
		}
	}
	*outbuf = '\0';
	
	return buf;
}

static struct ast_custom_function filter_function = {
	.name = "FILTER",
	.synopsis = "Filter the string to include only the allowed characters",
	.syntax = "FILTER(<allowed-chars>,<string>)",
	.read = filter,
};

static char *regex(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len) 
{
	char *parse;
	AST_DECLARE_APP_ARGS(args,
		AST_APP_ARG(null);
		AST_APP_ARG(reg);
		AST_APP_ARG(str);
	);
			        

	char errstr[256] = "";
	int errcode;
	regex_t regexbuf;

	ast_copy_string(buf, "0", len);
	
	if (!(parse = ast_strdupa(data)))
		return buf;

	AST_NONSTANDARD_APP_ARGS(args, parse, '"');

	ast_log(LOG_DEBUG, "FUNCTION REGEX (%s)(%s)\n", args.reg, args.str);

	if ((errcode = regcomp(&regexbuf, args.reg, REG_EXTENDED | REG_NOSUB))) {
		regerror(errcode, &regexbuf, errstr, sizeof(errstr));
		ast_log(LOG_WARNING, "Malformed input %s(%s): %s\n", cmd, data, errstr);
	} else {
		if (!regexec(&regexbuf, args.str, 0, NULL, 0))
			ast_copy_string(buf, "1", len); 
	}
	regfree(&regexbuf);

	return buf;
}

static struct ast_custom_function regex_function = {
	.name = "REGEX",
	.synopsis = "Regular Expression: Returns 1 if data matches regular expression.",
	.syntax = "REGEX(\"<regular expression>\" <data>)",
	.read = regex,
};

static void array(struct ast_channel *chan, char *cmd, char *data, const char *value)
{
	AST_DECLARE_APP_ARGS(arg1,
		AST_APP_ARG(var)[100];
	);
	AST_DECLARE_APP_ARGS(arg2,
		AST_APP_ARG(val)[100];
	);
	char *var, *value2;
	int i;

	var = ast_strdupa(data);
	value2 = ast_strdupa(value);
	if (!var || !value2)
		return;

	/* The functions this will generally be used with are SORT and ODBC_*, which
	 * both return comma-delimited lists.  However, if somebody uses literal lists,
	 * their commas will be translated to vertical bars by the load, and I don't
	 * want them to be surprised by the result.  Hence, we prefer commas as the
	 * delimiter, but we'll fall back to vertical bars if commas aren't found.
	 */
	ast_log(LOG_DEBUG, "array (%s=%s)\n", var, value2);
	if (strchr(var, ',')) {
		AST_NONSTANDARD_APP_ARGS(arg1, var, ',');
	} else {
		AST_STANDARD_APP_ARGS(arg1, var);
	}

	if (strchr(value2, ',')) {
		AST_NONSTANDARD_APP_ARGS(arg2, value2, ',');
	} else {
		AST_STANDARD_APP_ARGS(arg2, value2);
	}

	for (i = 0; i < arg1.argc; i++) {
		ast_log(LOG_DEBUG, "array set value (%s=%s)\n", arg1.var[i], arg2.val[i]);
		if (i < arg2.argc) {
			pbx_builtin_setvar_helper(chan, arg1.var[i], arg2.val[i]);
		} else {
			/* We could unset the variable, by passing a NULL, but due to
			 * pushvar semantics, that could create some undesired behavior. */
			pbx_builtin_setvar_helper(chan, arg1.var[i], "");
		}
	}
}

static struct ast_custom_function array_function = {
	.name = "ARRAY",
	.synopsis = "Allows setting multiple variables at once",
	.syntax = "ARRAY(var1[,var2[...][,varN]])",
	.write = array,
	.desc =
"The comma-separated list passed as a value to which the function is set will\n"
"be interpreted as a set of values to which the comma-separated list of\n"
"variable names in the argument should be set.\n"
"Hence, Set(ARRAY(var1,var2)=1,2) will set var1 to 1 and var2 to 2\n"
"Note: remember to either backslash your commas in extensions.conf or quote the\n"
"entire argument, since Set can take multiple arguments itself.\n",
};

static char *len(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len) 
{
	int length = 0;
	if (data) {
		length = strlen(data);
	}
	snprintf(buf, len, "%d", length);
	return buf;
}

static struct ast_custom_function len_function = {
	.name = "LEN",
	.synopsis = "Returns the length of the argument given",
	.syntax = "LEN(<string>)",
	.read = len,
};

static char *acf_strftime(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len) 
{
	char *parse;
	AST_DECLARE_APP_ARGS(args,
		AST_APP_ARG(epoch);
		AST_APP_ARG(timezone);
		AST_APP_ARG(format);
	);
	long epochi;
	struct tm time;

	buf[0] = '\0';

	if (ast_strlen_zero(data)) {
		ast_log(LOG_ERROR, "Asterisk function STRFTIME() requires an argument.\n");
		return buf;
	}
	
	if (!(parse = ast_strdupa(data)))
		return buf;
	
	AST_STANDARD_APP_ARGS(args, parse);

	if (ast_strlen_zero(args.epoch) || !sscanf(args.epoch, "%ld", &epochi)) {
		struct timeval tv = ast_tvnow();
		epochi = tv.tv_sec;
	}

	ast_localtime(&epochi, &time, args.timezone);

	if (!strftime(buf, len, args.format?args.format:"%c", &time)) {
		ast_log(LOG_WARNING, "C function strftime() output nothing?!!\n");
	}
	buf[len - 1] = '\0';

	return buf;
}

static struct ast_custom_function strftime_function = {
	.name = "STRFTIME",
	.synopsis = "Returns the current date/time in a specified format.",
	.syntax = "STRFTIME([<epoch>][,[timezone][,format]])",
	.read = acf_strftime,
};

static char *acf_strptime(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len) 
{
	AST_DECLARE_APP_ARGS(args,
		AST_APP_ARG(timestring);
		AST_APP_ARG(timezone);
		AST_APP_ARG(format);
	);
	struct tm time;

	memset(&time, 0, sizeof(struct tm));
	
	buf[0] = '\0';

	if (!data) {
		ast_log(LOG_ERROR, "Asterisk function STRPTIME() requires an argument.\n");
		return buf;
	}

	AST_STANDARD_APP_ARGS(args, data);

	if (ast_strlen_zero(args.format) ) {
		ast_log(LOG_ERROR, "No format supplied to STRPTIME(<timestring>|<timezone>|<format>)");
		return buf;
	}
			
	if (!strptime(args.timestring, args.format, &time)) {
		ast_log(LOG_WARNING, "C function strptime() output nothing?!!\n");
	} else {
		snprintf(buf, len, "%d", (int)ast_mktime(&time, args.timezone));	  
	}
	
	return buf;
}

static struct ast_custom_function strptime_function = {
	.name = "STRPTIME",
	.synopsis = "Returns the epoch of the arbitrary date/time string structured as described in the format.",
	.syntax = "STRPTIME(<datetime>|<timezone>|<format>)",
	.desc =
"This is useful for converting a date into an EPOCH time, possibly to pass to\n"
"an application like SayUnixTime or to calculate the difference between two\n"
"date strings.\n"
"\n"
"Example:\n"
"  ${STRPTIME(2006-03-01 07:30:35|America/Chicago|%Y-%m-%d %H:%M:%S)} returns 1141219835\n",
	.read = acf_strptime,
};

static char *function_eval(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len) 
{
	memset(buf, 0, len);

	if (ast_strlen_zero(data)) {
		ast_log(LOG_WARNING, "EVAL requires an argument: EVAL(<string>)\n");
		return buf;
	}

	pbx_substitute_variables_helper(chan, data, buf, len - 1);

	return buf;
}

static struct ast_custom_function eval_function = {
	.name = "EVAL",
	.synopsis = "Evaluate stored variables.",
	.syntax = "EVAL(<variable>)",
	.desc = "Using EVAL basically causes a string to be evaluated twice.\n"
		"When a variable or expression is in the dialplan, it will be\n"
		"evaluated at runtime. However, if the result of the evaluation\n"
		"is in fact a variable or expression, using EVAL will have it\n"
		"evaluated a second time. For example, if the variable ${MYVAR}\n"
		"contains \"${OTHERVAR}\", then the result of putting ${EVAL(${MYVAR})}\n"
		"in the dialplan will be the contents of the variable, OTHERVAR.\n"
		"Normally, by just putting ${MYVAR} in the dialplan, you would be\n"
		"left with \"${OTHERVAR}\".\n", 
	.read = function_eval,
};

static char *tdesc = "String handling dialplan functions";

int unload_module(void)
{
	int res = 0;

	res |= ast_custom_function_unregister(&fieldqty_function);
	res |= ast_custom_function_unregister(&filter_function);
	res |= ast_custom_function_unregister(&regex_function);
	res |= ast_custom_function_unregister(&array_function);
	res |= ast_custom_function_unregister(&len_function);
	res |= ast_custom_function_unregister(&strftime_function);
	res |= ast_custom_function_unregister(&strptime_function);
	res |= ast_custom_function_unregister(&eval_function);

	return res;
}

int load_module(void)
{
	int res = 0;

	res |= ast_custom_function_register(&fieldqty_function);
	res |= ast_custom_function_register(&filter_function);
	res |= ast_custom_function_register(&regex_function);
	res |= ast_custom_function_register(&array_function);
	res |= ast_custom_function_register(&len_function);
	res |= ast_custom_function_register(&strftime_function);
	res |= ast_custom_function_register(&strptime_function);
	res |= ast_custom_function_register(&eval_function);

	return res;
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

/*
Local Variables:
mode: C
c-file-style: "linux"
indent-tabs-mode: nil
End:
*/
