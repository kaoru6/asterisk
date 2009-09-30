/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2005-2006, Digium, Inc.
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
 * \ingroup functions
 */

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include <regex.h>
#include <ctype.h>

#include "asterisk/module.h"
#include "asterisk/channel.h"
#include "asterisk/pbx.h"
#include "asterisk/utils.h"
#include "asterisk/app.h"
#include "asterisk/localtime.h"

AST_THREADSTORAGE(result_buf);

/*** DOCUMENTATION
	<function name="FIELDQTY" language="en_US">
		<synopsis>
			Count the fields with an arbitrary delimiter
		</synopsis>
		<syntax>
			<parameter name="varname" required="true" />
			<parameter name="delim" required="true" />
		</syntax>
		<description>
			<para>The delimiter may be specified as a special or extended ASCII character, by encoding it.  The characters
			<literal>\n</literal>, <literal>\r</literal>, and <literal>\t</literal> are all recognized as the newline,
			carriage return, and tab characters, respectively.  Also, octal and hexadecimal specifications are recognized
			by the patterns <literal>\0nnn</literal> and <literal>\xHH</literal>, respectively.  For example, if you wanted
			to encode a comma as the delimiter, you could use either <literal>\054</literal> or <literal>\x2C</literal>.</para>
			<para>Example: If ${example} contains <literal>ex-amp-le</literal>, then ${FIELDQTY(example,-)} returns 3.</para>
		</description>
	</function>
	<function name="LISTFILTER" language="en_US">
		<synopsis>Remove an item from a list, by name.</synopsis>
		<syntax>
			<parameter name="varname" required="true" />
			<parameter name="delim" required="true" default="," />
			<parameter name="value" required="true" />
		</syntax>
		<description>
			<para>Remove <replaceable>value</replaceable> from the list contained in the <replaceable>varname</replaceable>
			variable, where the list delimiter is specified by the <replaceable>delim</replaceable> parameter.  This is
			very useful for removing a single channel name from a list of channels, for example.</para>
		</description>
	</function>
	<function name="FILTER" language="en_US">
		<synopsis>
			Filter the string to include only the allowed characters
		</synopsis>
		<syntax>
			<parameter name="allowed-chars" required="true" />
			<parameter name="string" required="true" />
		</syntax>
		<description>
			<para>Permits all characters listed in <replaceable>allowed-chars</replaceable>, 
			filtering all others outs. In addition to literally listing the characters, 
			you may also use ranges of characters (delimited by a <literal>-</literal></para>
			<para>Hexadecimal characters started with a <literal>\x</literal>(i.e. \x20)</para>
			<para>Octal characters started with a <literal>\0</literal> (i.e. \040)</para>
			<para>Also <literal>\t</literal>,<literal>\n</literal> and <literal>\r</literal> are recognized.</para> 
			<note><para>If you want the <literal>-</literal> character it needs to be prefixed with a 
			<literal>\</literal></para></note>
		</description>
	</function>
	<function name="REGEX" language="en_US">
		<synopsis>
			Check string against a regular expression.
		</synopsis>
		<syntax argsep=" ">
			<parameter name="&quot;regular expression&quot;" required="true" />
			<parameter name="string" required="true" />
		</syntax>
		<description>
			<para>Return <literal>1</literal> on regular expression match or <literal>0</literal> otherwise</para>
			<para>Please note that the space following the double quotes separating the 
			regex from the data is optional and if present, is skipped. If a space is 
			desired at the beginning of the data, then put two spaces there; the second 
			will not be skipped.</para>
		</description>
	</function>
	<application name="ClearHash" language="en_US">
		<synopsis>
			Clear the keys from a specified hashname.
		</synopsis>
		<syntax>
			<parameter name="hashname" required="true" />
		</syntax>
		<description>
			<para>Clears all keys out of the specified <replaceable>hashname</replaceable>.</para>
		</description>
	</application>
	<function name="HASH" language="en_US">
		<synopsis>
			Implementation of a dialplan associative array
		</synopsis>
		<syntax>
			<parameter name="hashname" required="true" />
			<parameter name="hashkey" />
		</syntax>
		<description>
			<para>In two arguments mode, gets and sets values to corresponding keys within
			a named associative array. The single-argument mode will only work when assigned
			to from a function defined by func_odbc</para>
		</description>
	</function>
	<function name="HASHKEYS" language="en_US">
		<synopsis>
			Retrieve the keys of the HASH() function.
		</synopsis>
		<syntax>
			<parameter name="hashname" required="true" />
		</syntax>
		<description>
			<para>Returns a comma-delimited list of the current keys of the associative array 
			defined by the HASH() function. Note that if you iterate over the keys of 
			the result, adding keys during iteration will cause the result of the HASHKEYS()
			function to change.</para>
		</description>
	</function>
	<function name="KEYPADHASH" language="en_US">
		<synopsis>
			Hash the letters in string into equivalent keypad numbers.
		</synopsis>
		<syntax>
			<parameter name="string" required="true" />
		</syntax>
		<description>
			<para>Example: ${KEYPADHASH(Les)} returns "537"</para>
		</description>
	</function>
	<function name="ARRAY" language="en_US">
		<synopsis>
			Allows setting multiple variables at once.
		</synopsis>
		<syntax>
			<parameter name="var1" required="true" />
			<parameter name="var2" required="false" multiple="true" />
			<parameter name="varN" required="false" />
		</syntax>
		<description>
			<para>The comma-delimited list passed as a value to which the function is set will 
			be interpreted as a set of values to which the comma-delimited list of 
			variable names in the argument should be set.</para>
			<para>Example: Set(ARRAY(var1,var2)=1,2) will set var1 to 1 and var2 to 2</para>
		</description>
	</function>
	<function name="STRPTIME" language="en_US">
		<synopsis>
			Returns the epoch of the arbitrary date/time string structured as described by the format.
		</synopsis>
		<syntax>
			<parameter name="datetime" required="true" />
			<parameter name="timezone" required="true" />
			<parameter name="format" required="true" />
		</syntax>
		<description>
			<para>This is useful for converting a date into <literal>EPOCH</literal> time, 
			possibly to pass to an application like SayUnixTime or to calculate the difference
			between the two date strings</para>
			<para>Example: ${STRPTIME(2006-03-01 07:30:35,America/Chicago,%Y-%m-%d %H:%M:%S)} returns 1141219835</para>
		</description>
	</function>
	<function name="STRFTIME" language="en_US">
		<synopsis>
			Returns the current date/time in the specified format.
		</synopsis>
		<syntax>
			<parameter name="epoch" />
			<parameter name="timezone" />
			<parameter name="format" />
		</syntax>
		<description>
			<para>STRFTIME supports all of the same formats as the underlying C function
			<emphasis>strftime(3)</emphasis>.
			It also supports the following format: <literal>%[n]q</literal> - fractions of a second,
			with leading zeros.</para>
			<para>Example: <literal>%3q</literal> will give milliseconds and <literal>%1q</literal>
			will give tenths of a second. The default is set at milliseconds (n=3).
			The common case is to use it in combination with %S, as in <literal>%S.%3q</literal>.</para>
		</description>
		<see-also>
			<ref type="manpage">strftime(3)</ref>
		</see-also>
	</function>
	<function name="EVAL" language="en_US">
		<synopsis>
			Evaluate stored variables
		</synopsis>
		<syntax>
			<parameter name="variable" required="true" />
		</syntax>
		<description>
			<para>Using EVAL basically causes a string to be evaluated twice.
			When a variable or expression is in the dialplan, it will be
			evaluated at runtime. However, if the results of the evaluation
			is in fact another variable or expression, using EVAL will have it
			evaluated a second time.</para>
			<para>Example: If the <variable>MYVAR</variable> contains
			<variable>OTHERVAR</variable>, then the result of ${EVAL(
			<variable>MYVAR</variable>)} in the dialplan will be the
			contents of <variable>OTHERVAR</variable>. Normally just
			putting <variable>MYVAR</variable> in the dialplan the result
			would be <variable>OTHERVAR</variable>.</para>
		</description>
	</function>
	<function name="TOUPPER" language="en_US">
		<synopsis>
			Convert string to all uppercase letters.
		</synopsis>
		<syntax>
			<parameter name="string" required="true" />
		</syntax>
		<description>
			<para>Example: ${TOUPPER(Example)} returns "EXAMPLE"</para>
		</description>
	</function>
	<function name="TOLOWER" language="en_US">
		<synopsis>
			Convert string to all lowercase letters.
		</synopsis>
		<syntax>
			<parameter name="string" required="true" />
		</syntax>
		<description>
			<para>Example: ${TOLOWER(Example)} returns "example"</para>
		</description>
	</function>
	<function name="LEN" language="en_US">
		<synopsis>
			Return the length of the string given.
		</synopsis>
		<syntax>
			<parameter name="string" required="true" />
		</syntax>
		<description>
			<para>Example: ${LEN(example)} returns 7</para>
		</description>
	</function>
	<function name="QUOTE" language="en_US">
		<synopsis>
			Quotes a given string, escaping embedded quotes as necessary
		</synopsis>
		<syntax>
			<parameter name="string" required="true" />
		</syntax>
		<description>
			<para>Example: ${QUOTE(ab"c"de)} will return "abcde"</para>
		</description>
	</function>
	<function name="CSV_QUOTE" language="en_US">
		<synopsis>
			Quotes a given string for use in a CSV file, escaping embedded quotes as necessary
		</synopsis>
		<syntax>
			<parameter name="string" required="true" />
		</syntax>
		<description>
			<para>Example: ${CSV_QUOTE("a,b" 123)} will return """a,b"" 123"</para>
		</description>
	</function>
	<function name="SHIFT" language="en_US">
		<synopsis>
			Removes and returns the first item off of a variable containing delimited text
		</synopsis>
		<syntax>
			<parameter name="varname" required="true" />
			<parameter name="delimiter" required="false" default="," />
		</syntax>
		<description>
			<para>Example:</para>
			<para>exten => s,1,Set(array=one,two,three)</para>
			<para>exten => s,n,While($["${SET(var=${SHIFT(array)})}" != ""])</para>
			<para>exten => s,n,NoOp(var is ${var})</para>
			<para>exten => s,n,EndWhile</para>
			<para>This would iterate over each value in array, left to right, and
				would result in NoOp(var is one), NoOp(var is two), and
				NoOp(var is three) being executed.
			</para>
		</description>
	</function>	
	<function name="POP" language="en_US">
		<synopsis>
			Removes and returns the last item off of a variable containing delimited text
		</synopsis>
		<syntax>
			<parameter name="varname" required="true" />
			<parameter name="delimiter" required="false" default="," />
		</syntax>
		<description>
			<para>Example:</para>
			<para>exten => s,1,Set(array=one,two,three)</para>
			<para>exten => s,n,While($["${SET(var=${POP(array)})}" != ""])</para>
			<para>exten => s,n,NoOp(var is ${var})</para>
			<para>exten => s,n,EndWhile</para>
			<para>This would iterate over each value in array, right to left, and
				would result in NoOp(var is three), NoOp(var is two), and
				NoOp(var is one) being executed.
			</para>
		</description>
	</function>	
	<function name="PUSH" language="en_US">
		<synopsis>
			Appends one or more values to the end of a variable containing delimited text
		</synopsis>
		<syntax>
			<parameter name="varname" required="true" />
			<parameter name="delimiter" required="false" default="," />
		</syntax>
		<description>
			<para>Example: Set(PUSH(array)=one,two,three) would append one,
				two, and three to the end of the values stored in the variable
				"array".
			</para>
		</description>
	</function>
	<function name="UNSHIFT" language="en_US">
		<synopsis>
			Inserts one or more values to the beginning of a variable containing delimited text
		</synopsis>
		<syntax>
			<parameter name="varname" required="true" />
			<parameter name="delimiter" required="false" default="," />
		</syntax>
		<description>
			<para>Example: Set(UNSHIFT(array)=one,two,three) would insert one,
				two, and three before the values stored in the variable
				"array".
			</para>
		</description>
	</function>
 ***/

static int function_fieldqty(struct ast_channel *chan, const char *cmd,
			     char *parse, char *buf, size_t len)
{
	char *varsubst, varval[8192], *varval2 = varval;
	int fieldcount = 0;
	AST_DECLARE_APP_ARGS(args,
			     AST_APP_ARG(varname);
			     AST_APP_ARG(delim);
		);
	char delim[2] = "";
	size_t delim_used;

	AST_STANDARD_APP_ARGS(args, parse);
	if (args.delim) {
		ast_get_encoded_char(args.delim, delim, &delim_used);

		varsubst = alloca(strlen(args.varname) + 4);

		sprintf(varsubst, "${%s}", args.varname);
		pbx_substitute_variables_helper(chan, varsubst, varval, sizeof(varval) - 1);
		if (ast_strlen_zero(varval2))
			fieldcount = 0;
		else {
			while (strsep(&varval2, delim))
				fieldcount++;
		}
	} else {
		fieldcount = 1;
	}
	snprintf(buf, len, "%d", fieldcount);

	return 0;
}

static struct ast_custom_function fieldqty_function = {
	.name = "FIELDQTY",
	.synopsis = "Count the fields, with an arbitrary delimiter",
	.syntax = "FIELDQTY(<varname>,<delim>)",
	.read = function_fieldqty,
};

static int filter(struct ast_channel *chan, const char *cmd, char *parse, char *buf,
		  size_t len)
{
	AST_DECLARE_APP_ARGS(args,
			     AST_APP_ARG(allowed);
			     AST_APP_ARG(string);
	);
	char *outbuf = buf, ac;
	char allowed[256] = "";
	size_t allowedlen = 0;

	AST_STANDARD_APP_ARGS(args, parse);

	if (!args.string) {
		ast_log(LOG_ERROR, "Usage: FILTER(<allowed-chars>,<string>)\n");
		return -1;
	}

	/* Expand ranges */
	for (; *(args.allowed) && allowedlen < sizeof(allowed); ) {
		char c1 = 0, c2 = 0;
		size_t consumed = 0;

		if (ast_get_encoded_char(args.allowed, &c1, &consumed))
			return -1;
		args.allowed += consumed;

		if (*(args.allowed) == '-') {
			if (ast_get_encoded_char(args.allowed + 1, &c2, &consumed))
				c2 = -1;
			args.allowed += consumed + 1;

			/*!\note
			 * Looks a little strange, until you realize that we can overflow
			 * the size of a char.
			 */
			for (ac = c1; ac != c2 && allowedlen < sizeof(allowed) - 1; ac++)
				allowed[allowedlen++] = ac;
			allowed[allowedlen++] = ac;

			ast_debug(4, "c1=%d, c2=%d\n", c1, c2);

			/* Decrement before the loop increment */
			(args.allowed)--;
		} else
			allowed[allowedlen++] = c1;
	}

	ast_debug(1, "Allowed: %s\n", allowed);

	for (; *(args.string) && (buf + len - 1 > outbuf); (args.string)++) {
		if (strchr(allowed, *(args.string)))
			*outbuf++ = *(args.string);
	}
	*outbuf = '\0';

	return 0;
}

static struct ast_custom_function filter_function = {
	.name = "FILTER",
	.synopsis = "Filter the string to include only the allowed characters",
	.syntax = "FILTER(<allowed-chars>,<string>)",
	.read = filter,
	.desc =
"Permits all characters listed in <allowed-chars>, filtering all others out.\n"
"In addition to literally listing the characters, you may also use ranges of\n"
"characters (delimited by a '-'), as well as hexadecimal characters started\n"
"with a \\x (i.e. \\x20) and octal characters started with \\0 (i.e. \\040).\n"
"Also, \\t, \\n, and \\r are recognized.  If you want a literal '-' character,\n"
"simply prefix it with a '\\'\n",
};

static int regex(struct ast_channel *chan, const char *cmd, char *parse, char *buf,
		 size_t len)
{
	AST_DECLARE_APP_ARGS(args,
			     AST_APP_ARG(null);
			     AST_APP_ARG(reg);
			     AST_APP_ARG(str);
	);
	int errcode;
	regex_t regexbuf;

	buf[0] = '\0';

	AST_NONSTANDARD_APP_ARGS(args, parse, '"');

	if (args.argc != 3) {
		ast_log(LOG_ERROR, "Unexpected arguments: should have been in the form '\"<regex>\" <string>'\n");
		return -1;
	}
	if ((*args.str == ' ') || (*args.str == '\t'))
		args.str++;

	ast_debug(1, "FUNCTION REGEX (%s)(%s)\n", args.reg, args.str);

	if ((errcode = regcomp(&regexbuf, args.reg, REG_EXTENDED | REG_NOSUB))) {
		regerror(errcode, &regexbuf, buf, len);
		ast_log(LOG_WARNING, "Malformed input %s(%s): %s\n", cmd, parse, buf);
		return -1;
	}
	
	strcpy(buf, regexec(&regexbuf, args.str, 0, NULL, 0) ? "0" : "1");

	regfree(&regexbuf);

	return 0;
}

static struct ast_custom_function regex_function = {
	.name = "REGEX",
	.synopsis = "Regular Expression",
	.desc =  
		"Returns 1 if data matches regular expression, or 0 otherwise.\n"
		"Please note that the space following the double quotes separating the regex from the data\n"
		"is optional and if present, is skipped. If a space is desired at the beginning of the data,\n"
	        "then put two spaces there; the second will not be skipped.\n",
	.syntax = "REGEX(\"<regular expression>\" <data>)",
	.read = regex,
};

#define HASH_PREFIX	"~HASH~%s~"
#define HASH_FORMAT	HASH_PREFIX "%s~"

static char *app_clearhash = "ClearHash";
static char *syn_clearhash = "Clear the keys from a specified hashname";
static char *desc_clearhash =
"ClearHash(<hashname>)\n"
"  Clears all keys out of the specified hashname\n";

/* This function probably should migrate to main/pbx.c, as pbx_builtin_clearvar_prefix() */
static void clearvar_prefix(struct ast_channel *chan, const char *prefix)
{
	struct ast_var_t *var;
	int len = strlen(prefix);
	AST_LIST_TRAVERSE_SAFE_BEGIN(&chan->varshead, var, entries) {
		if (strncasecmp(prefix, ast_var_name(var), len) == 0) {
			AST_LIST_REMOVE_CURRENT(entries);
			ast_free(var);
		}
	}
	AST_LIST_TRAVERSE_SAFE_END
}

static int exec_clearhash(struct ast_channel *chan, void *data)
{
	char prefix[80];
	snprintf(prefix, sizeof(prefix), HASH_PREFIX, data ? (char *)data : "null");
	clearvar_prefix(chan, prefix);
	return 0;
}

static int array(struct ast_channel *chan, const char *cmd, char *var,
		 const char *value)
{
	AST_DECLARE_APP_ARGS(arg1,
			     AST_APP_ARG(var)[100];
	);
	AST_DECLARE_APP_ARGS(arg2,
			     AST_APP_ARG(val)[100];
	);
	char *origvar = "", *value2, varname[256];
	int i, ishash = 0;

	value2 = ast_strdupa(value);
	if (!var || !value2)
		return -1;

	if (!strcmp(cmd, "HASH")) {
		const char *var2 = pbx_builtin_getvar_helper(chan, "~ODBCFIELDS~");
		origvar = var;
		if (var2)
			var = ast_strdupa(var2);
		else {
			if (chan)
				ast_autoservice_stop(chan);
			return -1;
		}
		ishash = 1;
	}

	/* The functions this will generally be used with are SORT and ODBC_*, which
	 * both return comma-delimited lists.  However, if somebody uses literal lists,
	 * their commas will be translated to vertical bars by the load, and I don't
	 * want them to be surprised by the result.  Hence, we prefer commas as the
	 * delimiter, but we'll fall back to vertical bars if commas aren't found.
	 */
	ast_debug(1, "array (%s=%s)\n", var, value2);
	AST_STANDARD_APP_ARGS(arg1, var);

	AST_STANDARD_APP_ARGS(arg2, value2);

	for (i = 0; i < arg1.argc; i++) {
		ast_debug(1, "array set value (%s=%s)\n", arg1.var[i],
				arg2.val[i]);
		if (i < arg2.argc) {
			if (ishash) {
				snprintf(varname, sizeof(varname), HASH_FORMAT, origvar, arg1.var[i]);
				pbx_builtin_setvar_helper(chan, varname, arg2.val[i]);
			} else {
				pbx_builtin_setvar_helper(chan, arg1.var[i], arg2.val[i]);
			}
		} else {
			/* We could unset the variable, by passing a NULL, but due to
			 * pushvar semantics, that could create some undesired behavior. */
			if (ishash) {
				snprintf(varname, sizeof(varname), HASH_FORMAT, origvar, arg1.var[i]);
				pbx_builtin_setvar_helper(chan, varname, "");
			} else {
				pbx_builtin_setvar_helper(chan, arg1.var[i], "");
			}
		}
	}

	return 0;
}

static int hashkeys_read(struct ast_channel *chan, const char *cmd, char *data, char *buf, size_t len)
{
	struct ast_var_t *newvar;
	int plen;
	char prefix[80];
	snprintf(prefix, sizeof(prefix), HASH_PREFIX, data);
	plen = strlen(prefix);

	memset(buf, 0, len);
	AST_LIST_TRAVERSE(&chan->varshead, newvar, entries) {
		if (strncasecmp(prefix, ast_var_name(newvar), plen) == 0) {
			/* Copy everything after the prefix */
			strncat(buf, ast_var_name(newvar) + plen, len - strlen(buf) - 1);
			/* Trim the trailing ~ */
			buf[strlen(buf) - 1] = ',';
		}
	}
	/* Trim the trailing comma */
	buf[strlen(buf) - 1] = '\0';
	return 0;
}

static int hash_write(struct ast_channel *chan, const char *cmd, char *var, const char *value)
{
	char varname[256];
	AST_DECLARE_APP_ARGS(arg,
		AST_APP_ARG(hashname);
		AST_APP_ARG(hashkey);
	);

	if (!strchr(var, ',')) {
		/* Single argument version */
		return array(chan, "HASH", var, value);
	}

	AST_STANDARD_APP_ARGS(arg, var);
	snprintf(varname, sizeof(varname), HASH_FORMAT, arg.hashname, arg.hashkey);
	pbx_builtin_setvar_helper(chan, varname, value);

	return 0;
}

static int hash_read(struct ast_channel *chan, const char *cmd, char *data, char *buf, size_t len)
{
	char varname[256];
	const char *varvalue;
	AST_DECLARE_APP_ARGS(arg,
		AST_APP_ARG(hashname);
		AST_APP_ARG(hashkey);
	);

	AST_STANDARD_APP_ARGS(arg, data);
	if (arg.argc == 2) {
		snprintf(varname, sizeof(varname), HASH_FORMAT, arg.hashname, arg.hashkey);
		varvalue = pbx_builtin_getvar_helper(chan, varname);
		if (varvalue)
			ast_copy_string(buf, varvalue, len);
		else
			*buf = '\0';
	} else if (arg.argc == 1) {
		char colnames[4096];
		int i;
		AST_DECLARE_APP_ARGS(arg2,
			AST_APP_ARG(col)[100];
		);

		/* Get column names, in no particular order */
		hashkeys_read(chan, "HASHKEYS", arg.hashname, colnames, sizeof(colnames));
		pbx_builtin_setvar_helper(chan, "~ODBCFIELDS~", colnames);

		AST_STANDARD_APP_ARGS(arg2, colnames);
		*buf = '\0';

		/* Now get the corresponding column values, in exactly the same order */
		for (i = 0; i < arg2.argc; i++) {
			snprintf(varname, sizeof(varname), HASH_FORMAT, arg.hashname, arg2.col[i]);
			varvalue = pbx_builtin_getvar_helper(chan, varname);
			strncat(buf, varvalue, len - strlen(buf) - 1);
			strncat(buf, ",", len - strlen(buf) - 1);
		}

		/* Strip trailing comma */
		buf[strlen(buf) - 1] = '\0';
	}

	return 0;
}

static struct ast_custom_function hash_function = {
	.name = "HASH",
	.synopsis = "Implementation of a dialplan associative array",
	.syntax = "HASH(hashname[,hashkey])",
	.write = hash_write,
	.read = hash_read,
	.desc =
		"In two argument mode, gets and sets values to corresponding keys within a named\n"
		"associative array.  The single-argument mode will only work when assigned to from\n"
		"a function defined by func_odbc.so.\n",
};

static struct ast_custom_function hashkeys_function = {
	.name = "HASHKEYS",
	.synopsis = "Retrieve the keys of a HASH()",
	.syntax = "HASHKEYS(<hashname>)",
	.read = hashkeys_read,
	.desc =
		"Returns a comma-delimited list of the current keys of an associative array\n"
	   	"defined by the HASH() function.  Note that if you iterate over the keys of\n"
		"the result, adding keys during iteration will cause the result of the HASHKEYS\n"
		"function to change.\n",
};

static struct ast_custom_function array_function = {
	.name = "ARRAY",
	.synopsis = "Allows setting multiple variables at once",
	.syntax = "ARRAY(var1[,var2[...][,varN]])",
	.write = array,
	.desc =
		"The comma-separated list passed as a value to which the function is set will\n"
		"be interpreted as a set of values to which the comma-separated list of\n"
		"variable names in the argument should be set.\n"
		"Hence, Set(ARRAY(var1,var2)=1,2) will set var1 to 1 and var2 to 2.\n",
};

static int acf_sprintf(struct ast_channel *chan, const char *cmd, char *data, char *buf, size_t len)
{
#define SPRINTF_FLAG	0
#define SPRINTF_WIDTH	1
#define SPRINTF_PRECISION	2
#define SPRINTF_LENGTH	3
#define SPRINTF_CONVERSION	4
	int i, state = -1, argcount = 0;
	char *formatstart = NULL, *bufptr = buf;
	char formatbuf[256] = "";
	int tmpi;
	double tmpd;
	AST_DECLARE_APP_ARGS(arg,
				AST_APP_ARG(format);
				AST_APP_ARG(var)[100];
	);

	AST_STANDARD_APP_ARGS(arg, data);

	/* Scan the format, converting each argument into the requisite format type. */
	for (i = 0; arg.format[i]; i++) {
		switch (state) {
		case SPRINTF_FLAG:
			if (strchr("#0- +'I", arg.format[i]))
				break;
			state = SPRINTF_WIDTH;
		case SPRINTF_WIDTH:
			if (arg.format[i] >= '0' && arg.format[i] <= '9')
				break;

			/* Next character must be a period to go into a precision */
			if (arg.format[i] == '.') {
				state = SPRINTF_PRECISION;
			} else {
				state = SPRINTF_LENGTH;
				i--;
			}
			break;
		case SPRINTF_PRECISION:
			if (arg.format[i] >= '0' && arg.format[i] <= '9')
				break;
			state = SPRINTF_LENGTH;
		case SPRINTF_LENGTH:
			if (strchr("hl", arg.format[i])) {
				if (arg.format[i + 1] == arg.format[i])
					i++;
				state = SPRINTF_CONVERSION;
				break;
			} else if (strchr("Lqjzt", arg.format[i])) {
				state = SPRINTF_CONVERSION;
				break;
			}
			state = SPRINTF_CONVERSION;
		case SPRINTF_CONVERSION:
			if (strchr("diouxXc", arg.format[i])) {
				/* Integer */

				/* Isolate this format alone */
				ast_copy_string(formatbuf, formatstart, sizeof(formatbuf));
				formatbuf[&arg.format[i] - formatstart + 1] = '\0';

				/* Convert the argument into the required type */
				if (arg.var[argcount]) {
					if (sscanf(arg.var[argcount++], "%30d", &tmpi) != 1) {
						ast_log(LOG_ERROR, "Argument '%s' is not an integer number for format '%s'\n", arg.var[argcount - 1], formatbuf);
						goto sprintf_fail;
					}
				} else {
					ast_log(LOG_ERROR, "SPRINTF() has more format specifiers than arguments!\n");
					goto sprintf_fail;
				}

				/* Format the argument */
				snprintf(bufptr, buf + len - bufptr, formatbuf, tmpi);

				/* Update the position of the next parameter to print */
				bufptr = strchr(buf, '\0');
			} else if (strchr("eEfFgGaA", arg.format[i])) {
				/* Double */

				/* Isolate this format alone */
				ast_copy_string(formatbuf, formatstart, sizeof(formatbuf));
				formatbuf[&arg.format[i] - formatstart + 1] = '\0';

				/* Convert the argument into the required type */
				if (arg.var[argcount]) {
					if (sscanf(arg.var[argcount++], "%30lf", &tmpd) != 1) {
						ast_log(LOG_ERROR, "Argument '%s' is not a floating point number for format '%s'\n", arg.var[argcount - 1], formatbuf);
						goto sprintf_fail;
					}
				} else {
					ast_log(LOG_ERROR, "SPRINTF() has more format specifiers than arguments!\n");
					goto sprintf_fail;
				}

				/* Format the argument */
				snprintf(bufptr, buf + len - bufptr, formatbuf, tmpd);

				/* Update the position of the next parameter to print */
				bufptr = strchr(buf, '\0');
			} else if (arg.format[i] == 's') {
				/* String */

				/* Isolate this format alone */
				ast_copy_string(formatbuf, formatstart, sizeof(formatbuf));
				formatbuf[&arg.format[i] - formatstart + 1] = '\0';

				/* Format the argument */
				snprintf(bufptr, buf + len - bufptr, formatbuf, arg.var[argcount++]);

				/* Update the position of the next parameter to print */
				bufptr = strchr(buf, '\0');
			} else if (arg.format[i] == '%') {
				/* Literal data to copy */
				*bufptr++ = arg.format[i];
			} else {
				/* Not supported */

				/* Isolate this format alone */
				ast_copy_string(formatbuf, formatstart, sizeof(formatbuf));
				formatbuf[&arg.format[i] - formatstart + 1] = '\0';

				ast_log(LOG_ERROR, "Format type not supported: '%s' with argument '%s'\n", formatbuf, arg.var[argcount++]);
				goto sprintf_fail;
			}
			state = -1;
			break;
		default:
			if (arg.format[i] == '%') {
				state = SPRINTF_FLAG;
				formatstart = &arg.format[i];
				break;
			} else {
				/* Literal data to copy */
				*bufptr++ = arg.format[i];
			}
		}
	}
	*bufptr = '\0';
	return 0;
sprintf_fail:
	return -1;
}

static struct ast_custom_function sprintf_function = {
	.name = "SPRINTF",
	.synopsis = "Format a variable according to a format string",
	.syntax = "SPRINTF(<format>,<arg1>[,...<argN>])",
	.read = acf_sprintf,
	.desc =
"Parses the format string specified and returns a string matching that format.\n"
"Supports most options supported by sprintf(3).  Returns a shortened string if\n"
"a format specifier is not recognized.\n",
};

static int quote(struct ast_channel *chan, const char *cmd, char *data, char *buf, size_t len)
{
	char *bufptr = buf, *dataptr = data;

	if (len < 3){ /* at least two for quotes and one for binary zero */
		ast_log(LOG_ERROR, "Not enough buffer");
		return -1;
	}

	if (ast_strlen_zero(data)) {
		ast_log(LOG_WARNING, "No argument specified!\n");
		ast_copy_string(buf, "\"\"", len);
		return 0;
	}

	*bufptr++ = '"';
	for (; bufptr < buf + len - 3; dataptr++) {
		if (*dataptr == '\\') {
			*bufptr++ = '\\';
			*bufptr++ = '\\';
		} else if (*dataptr == '"') {
			*bufptr++ = '\\';
			*bufptr++ = '"';
		} else if (*dataptr == '\0') {
			break;
		} else {
			*bufptr++ = *dataptr;
		}
	}
	*bufptr++ = '"';
	*bufptr = '\0';
	return 0;
}

static struct ast_custom_function quote_function = {
	.name = "QUOTE",
	.synopsis = "Quotes a given string, escaping embedded quotes as necessary",
	.syntax = "QUOTE(<string>)",
	.read = quote,
};

static int csv_quote(struct ast_channel *chan, const char *cmd, char *data, char *buf, size_t len)
{
	char *bufptr = buf, *dataptr = data;

	if (len < 3){ /* at least two for quotes and one for binary zero */
		ast_log(LOG_ERROR, "Not enough buffer");
		return -1;
	}

	if (ast_strlen_zero(data)) {
		ast_log(LOG_WARNING, "No argument specified!\n");
		ast_copy_string(buf,"\"\"",len);
		return 0;
	}

	*bufptr++ = '"';
	for (; bufptr < buf + len - 3; dataptr++){
		if (*dataptr == '"') {
			*bufptr++ = '"';
			*bufptr++ = '"';
		} else if (*dataptr == '\0') {
			break;
		} else {
			*bufptr++ = *dataptr;
		}
	}
	*bufptr++ = '"';
	*bufptr='\0';
	return 0;
}

static struct ast_custom_function csv_quote_function = {
	.name = "CSV_QUOTE",
	.read = csv_quote,
};

static int len(struct ast_channel *chan, const char *cmd, char *data, char *buf, size_t len)
{
	int length = 0;

	if (data)
		length = strlen(data);

	snprintf(buf, len, "%d", length);

	return 0;
}

static struct ast_custom_function len_function = {
	.name = "LEN",
	.synopsis = "Returns the length of the argument given",
	.syntax = "LEN(<string>)",
	.read = len,
};

static int acf_strftime(struct ast_channel *chan, const char *cmd, char *parse,
			char *buf, size_t len)
{
	AST_DECLARE_APP_ARGS(args,
			     AST_APP_ARG(epoch);
			     AST_APP_ARG(timezone);
			     AST_APP_ARG(format);
	);
	struct timeval tv;
	struct ast_tm tm;

	buf[0] = '\0';

	AST_STANDARD_APP_ARGS(args, parse);

	ast_get_timeval(args.epoch, &tv, ast_tvnow(), NULL);
	ast_localtime(&tv, &tm, args.timezone);

	if (!args.format)
		args.format = "%c";

	if (ast_strftime(buf, len, args.format, &tm) <= 0)
		ast_log(LOG_WARNING, "C function strftime() output nothing?!!\n");

	buf[len - 1] = '\0';

	return 0;
}

static struct ast_custom_function strftime_function = {
	.name = "STRFTIME",
	.synopsis = "Returns the current date/time in a specified format.",
	.syntax = "STRFTIME([<epoch>][,[timezone][,format]])",
	.desc =
"STRFTIME sports all of the same formats as the underlying C function\n"
"strftime(3) - see the man page for details.  It also supports the\n"
"following format:\n"
" %[n]q - fractions of a second, with leading zeroes.  For example, %3q will\n"
"         give milliseconds and %1q will give tenths of a second.  The default\n"
"         is to output milliseconds (n=3).  The common case is to use it in\n"
"         combination with %S, as in \"%S.%3q\".\n",
	.read = acf_strftime,
};

static int acf_strptime(struct ast_channel *chan, const char *cmd, char *data,
			char *buf, size_t len)
{
	AST_DECLARE_APP_ARGS(args,
			     AST_APP_ARG(timestring);
			     AST_APP_ARG(timezone);
			     AST_APP_ARG(format);
	);
	union {
		struct ast_tm atm;
		struct tm time;
	} t = { { 0, }, };

	buf[0] = '\0';

	if (!data) {
		ast_log(LOG_ERROR,
				"Asterisk function STRPTIME() requires an argument.\n");
		return -1;
	}

	AST_STANDARD_APP_ARGS(args, data);

	if (ast_strlen_zero(args.format)) {
		ast_log(LOG_ERROR,
				"No format supplied to STRPTIME(<timestring>,<timezone>,<format>)");
		return -1;
	}

	if (!strptime(args.timestring, args.format, &t.time)) {
		ast_log(LOG_WARNING, "C function strptime() output nothing?!!\n");
	} else {
		struct timeval tv;
		/* Since strptime(3) does not check DST, force ast_mktime() to calculate it. */
		t.atm.tm_isdst = -1;
		tv = ast_mktime(&t.atm, args.timezone);
		snprintf(buf, len, "%d", (int) tv.tv_sec);
	}

	return 0;
}

static struct ast_custom_function strptime_function = {
	.name = "STRPTIME",
	.synopsis =
		"Returns the epoch of the arbitrary date/time string structured as described in the format.",
	.syntax = "STRPTIME(<datetime>,<timezone>,<format>)",
	.desc =
		"This is useful for converting a date into an EPOCH time, possibly to pass to\n"
		"an application like SayUnixTime or to calculate the difference between two\n"
		"date strings.\n"
		"\n"
		"Example:\n"
		"  ${STRPTIME(2006-03-01 07:30:35,America/Chicago,%Y-%m-%d %H:%M:%S)} returns 1141219835\n",
	.read = acf_strptime,
};

static int function_eval(struct ast_channel *chan, const char *cmd, char *data,
			 char *buf, size_t len)
{
	if (ast_strlen_zero(data)) {
		ast_log(LOG_WARNING, "EVAL requires an argument: EVAL(<string>)\n");
		return -1;
	}

	pbx_substitute_variables_helper(chan, data, buf, len - 1);

	return 0;
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

static int keypadhash(struct ast_channel *chan, const char *cmd, char *data, char *buf, size_t len)
{
	char *bufptr, *dataptr;

	for (bufptr = buf, dataptr = data; bufptr < buf + len - 1; dataptr++) {
		if (*dataptr == '\0') {
			*bufptr++ = '\0';
			break;
		} else if (*dataptr == '1') {
			*bufptr++ = '1';
		} else if (strchr("AaBbCc2", *dataptr)) {
			*bufptr++ = '2';
		} else if (strchr("DdEeFf3", *dataptr)) {
			*bufptr++ = '3';
		} else if (strchr("GgHhIi4", *dataptr)) {
			*bufptr++ = '4';
		} else if (strchr("JjKkLl5", *dataptr)) {
			*bufptr++ = '5';
		} else if (strchr("MmNnOo6", *dataptr)) {
			*bufptr++ = '6';
		} else if (strchr("PpQqRrSs7", *dataptr)) {
			*bufptr++ = '7';
		} else if (strchr("TtUuVv8", *dataptr)) {
			*bufptr++ = '8';
		} else if (strchr("WwXxYyZz9", *dataptr)) {
			*bufptr++ = '9';
		} else if (*dataptr == '0') {
			*bufptr++ = '0';
		}
	}
	buf[len - 1] = '\0';

	return 0;
}

static struct ast_custom_function keypadhash_function = {
	.name = "KEYPADHASH",
	.synopsis = "Hash the letters in the string into the equivalent keypad numbers.",
	.syntax = "KEYPADHASH(<string>)",
	.read = keypadhash,
	.desc = "Example:  ${KEYPADHASH(Les)} returns \"537\"\n",
};

static int string_toupper(struct ast_channel *chan, const char *cmd, char *data, char *buf, size_t len)
{
	char *bufptr = buf, *dataptr = data;

	while ((bufptr < buf + len - 1) && (*bufptr++ = toupper(*dataptr++)));

	return 0;
}

static struct ast_custom_function toupper_function = {
	.name = "TOUPPER",
	.synopsis = "Convert the string to upper case.",
	.syntax = "TOUPPER(<string>)",
	.read = string_toupper,
	.desc = "Example: ${TOUPPER(Example)} returns \"EXAMPLE\"\n",
};

static int string_tolower(struct ast_channel *chan, const char *cmd, char *data, char *buf, size_t len)
{
	char *bufptr = buf, *dataptr = data;

	while ((bufptr < buf + len - 1) && (*bufptr++ = tolower(*dataptr++)));

	return 0;
}

static struct ast_custom_function tolower_function = {
	.name = "TOLOWER",
	.synopsis = "Convert the string to lower case.",
	.syntax = "TOLOWER(<string>)",
	.read = string_tolower,
	.desc = "Example: ${TOLOWER(Example)} returns \"example\"\n",
};

static int unload_module(void)
{
	int res = 0;

	res |= ast_custom_function_unregister(&fieldqty_function);
	res |= ast_custom_function_unregister(&filter_function);
	res |= ast_custom_function_unregister(&regex_function);
	res |= ast_custom_function_unregister(&array_function);
	res |= ast_custom_function_unregister(&quote_function);
	res |= ast_custom_function_unregister(&csv_quote_function);
	res |= ast_custom_function_unregister(&len_function);
	res |= ast_custom_function_unregister(&strftime_function);
	res |= ast_custom_function_unregister(&strptime_function);
	res |= ast_custom_function_unregister(&eval_function);
	res |= ast_custom_function_unregister(&keypadhash_function);
	res |= ast_custom_function_unregister(&sprintf_function);
	res |= ast_custom_function_unregister(&hashkeys_function);
	res |= ast_custom_function_unregister(&hash_function);
	res |= ast_unregister_application(app_clearhash);
	res |= ast_custom_function_unregister(&toupper_function);
	res |= ast_custom_function_unregister(&tolower_function);

	return res;
}

static int load_module(void)
{
	int res = 0;

	res |= ast_custom_function_register(&fieldqty_function);
	res |= ast_custom_function_register(&filter_function);
	res |= ast_custom_function_register(&regex_function);
	res |= ast_custom_function_register(&array_function);
	res |= ast_custom_function_register(&quote_function);
	res |= ast_custom_function_register(&csv_quote_function);
	res |= ast_custom_function_register(&len_function);
	res |= ast_custom_function_register(&strftime_function);
	res |= ast_custom_function_register(&strptime_function);
	res |= ast_custom_function_register(&eval_function);
	res |= ast_custom_function_register(&keypadhash_function);
	res |= ast_custom_function_register(&sprintf_function);
	res |= ast_custom_function_register(&hashkeys_function);
	res |= ast_custom_function_register(&hash_function);
	res |= ast_register_application(app_clearhash, exec_clearhash, syn_clearhash, desc_clearhash);
	res |= ast_custom_function_register(&toupper_function);
	res |= ast_custom_function_register(&tolower_function);

	return res;
}

AST_MODULE_INFO_STANDARD(ASTERISK_GPL_KEY, "String handling dialplan functions");
