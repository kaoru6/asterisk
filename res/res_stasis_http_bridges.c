/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2012 - 2013, Digium, Inc.
 *
 * David M. Lee, II <dlee@digium.com>
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

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!!!!                               DO NOT EDIT                        !!!!!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * This file is generated by a mustache template. Please see the original
 * template in rest-api-templates/res_stasis_http_resource.c.mustache
 */

/*! \file
 *
 * \brief Bridge resources
 *
 * \author David M. Lee, II <dlee@digium.com>
 */

/*** MODULEINFO
	<depend type="module">res_stasis_http</depend>
	<depend type="module">res_stasis</depend>
	<support_level>core</support_level>
 ***/

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include "asterisk/module.h"
#include "asterisk/stasis_app.h"
#include "stasis_http/resource_bridges.h"

/*!
 * \brief Parameter parsing callback for /bridges.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void stasis_http_get_bridges_cb(
    struct ast_variable *get_params, struct ast_variable *path_vars,
    struct ast_variable *headers, struct stasis_http_response *response)
{
	struct ast_get_bridges_args args = {};
	stasis_http_get_bridges(headers, &args, response);
}
/*!
 * \brief Parameter parsing callback for /bridges.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void stasis_http_new_bridge_cb(
    struct ast_variable *get_params, struct ast_variable *path_vars,
    struct ast_variable *headers, struct stasis_http_response *response)
{
	struct ast_new_bridge_args args = {};
	struct ast_variable *i;

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "type") == 0) {
			args.type = (i->value);
		} else
		{}
	}
	stasis_http_new_bridge(headers, &args, response);
}
/*!
 * \brief Parameter parsing callback for /bridges/{bridgeId}.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void stasis_http_get_bridge_cb(
    struct ast_variable *get_params, struct ast_variable *path_vars,
    struct ast_variable *headers, struct stasis_http_response *response)
{
	struct ast_get_bridge_args args = {};
	struct ast_variable *i;

	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "bridgeId") == 0) {
			args.bridge_id = (i->value);
		} else
		{}
	}
	stasis_http_get_bridge(headers, &args, response);
}
/*!
 * \brief Parameter parsing callback for /bridges/{bridgeId}.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void stasis_http_delete_bridge_cb(
    struct ast_variable *get_params, struct ast_variable *path_vars,
    struct ast_variable *headers, struct stasis_http_response *response)
{
	struct ast_delete_bridge_args args = {};
	struct ast_variable *i;

	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "bridgeId") == 0) {
			args.bridge_id = (i->value);
		} else
		{}
	}
	stasis_http_delete_bridge(headers, &args, response);
}
/*!
 * \brief Parameter parsing callback for /bridges/{bridgeId}/addChannel.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void stasis_http_add_channel_to_bridge_cb(
    struct ast_variable *get_params, struct ast_variable *path_vars,
    struct ast_variable *headers, struct stasis_http_response *response)
{
	struct ast_add_channel_to_bridge_args args = {};
	struct ast_variable *i;

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "channel") == 0) {
			args.channel = (i->value);
		} else
		{}
	}
	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "bridgeId") == 0) {
			args.bridge_id = (i->value);
		} else
		{}
	}
	stasis_http_add_channel_to_bridge(headers, &args, response);
}
/*!
 * \brief Parameter parsing callback for /bridges/{bridgeId}/removeChannel.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void stasis_http_remove_channel_from_bridge_cb(
    struct ast_variable *get_params, struct ast_variable *path_vars,
    struct ast_variable *headers, struct stasis_http_response *response)
{
	struct ast_remove_channel_from_bridge_args args = {};
	struct ast_variable *i;

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "channel") == 0) {
			args.channel = (i->value);
		} else
		{}
	}
	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "bridgeId") == 0) {
			args.bridge_id = (i->value);
		} else
		{}
	}
	stasis_http_remove_channel_from_bridge(headers, &args, response);
}
/*!
 * \brief Parameter parsing callback for /bridges/{bridgeId}/record.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void stasis_http_record_bridge_cb(
    struct ast_variable *get_params, struct ast_variable *path_vars,
    struct ast_variable *headers, struct stasis_http_response *response)
{
	struct ast_record_bridge_args args = {};
	struct ast_variable *i;

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "name") == 0) {
			args.name = (i->value);
		} else
		if (strcmp(i->name, "maxDurationSeconds") == 0) {
			args.max_duration_seconds = atoi(i->value);
		} else
		if (strcmp(i->name, "maxSilenceSeconds") == 0) {
			args.max_silence_seconds = atoi(i->value);
		} else
		if (strcmp(i->name, "append") == 0) {
			args.append = atoi(i->value);
		} else
		if (strcmp(i->name, "beep") == 0) {
			args.beep = atoi(i->value);
		} else
		if (strcmp(i->name, "terminateOn") == 0) {
			args.terminate_on = (i->value);
		} else
		{}
	}
	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "bridgeId") == 0) {
			args.bridge_id = (i->value);
		} else
		{}
	}
	stasis_http_record_bridge(headers, &args, response);
}

/*! \brief REST handler for /api-docs/bridges.{format} */
static struct stasis_rest_handlers bridges_bridgeId_addChannel = {
	.path_segment = "addChannel",
	.callbacks = {
		[AST_HTTP_POST] = stasis_http_add_channel_to_bridge_cb,
	},
	.num_children = 0,
	.children = {  }
};
/*! \brief REST handler for /api-docs/bridges.{format} */
static struct stasis_rest_handlers bridges_bridgeId_removeChannel = {
	.path_segment = "removeChannel",
	.callbacks = {
		[AST_HTTP_POST] = stasis_http_remove_channel_from_bridge_cb,
	},
	.num_children = 0,
	.children = {  }
};
/*! \brief REST handler for /api-docs/bridges.{format} */
static struct stasis_rest_handlers bridges_bridgeId_record = {
	.path_segment = "record",
	.callbacks = {
		[AST_HTTP_POST] = stasis_http_record_bridge_cb,
	},
	.num_children = 0,
	.children = {  }
};
/*! \brief REST handler for /api-docs/bridges.{format} */
static struct stasis_rest_handlers bridges_bridgeId = {
	.path_segment = "bridgeId",
	.is_wildcard = 1,
	.callbacks = {
		[AST_HTTP_GET] = stasis_http_get_bridge_cb,
		[AST_HTTP_DELETE] = stasis_http_delete_bridge_cb,
	},
	.num_children = 3,
	.children = { &bridges_bridgeId_addChannel,&bridges_bridgeId_removeChannel,&bridges_bridgeId_record, }
};
/*! \brief REST handler for /api-docs/bridges.{format} */
static struct stasis_rest_handlers bridges = {
	.path_segment = "bridges",
	.callbacks = {
		[AST_HTTP_GET] = stasis_http_get_bridges_cb,
		[AST_HTTP_POST] = stasis_http_new_bridge_cb,
	},
	.num_children = 1,
	.children = { &bridges_bridgeId, }
};

static int load_module(void)
{
	stasis_app_ref();
	return stasis_http_add_handler(&bridges);
}

static int unload_module(void)
{
	stasis_http_remove_handler(&bridges);
	stasis_app_unref();
	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "RESTful API module - Bridge resources",
	.load = load_module,
	.unload = unload_module,
	.nonoptreq = "res_stasis_http,res_stasis",
	);
