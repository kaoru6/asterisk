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
 * template in rest-api-templates/res_ari_resource.c.mustache
 */

/*! \file
 *
 * \brief Playback control resources
 *
 * \author David M. Lee, II <dlee@digium.com>
 */

/*** MODULEINFO
	<depend type="module">res_ari</depend>
	<depend type="module">res_stasis</depend>
	<support_level>core</support_level>
 ***/

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include "asterisk/app.h"
#include "asterisk/module.h"
#include "asterisk/stasis_app.h"
#include "ari/resource_playbacks.h"
#if defined(AST_DEVMODE)
#include "ari/ari_model_validators.h"
#endif

#define MAX_VALS 128

/*!
 * \brief Parameter parsing callback for /playbacks/{playbackId}.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void ast_ari_playbacks_get_cb(
	struct ast_tcptls_session_instance *ser,
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	struct ast_ari_playbacks_get_args args = {};
	struct ast_variable *i;
	RAII_VAR(struct ast_json *, body, NULL, ast_json_unref);
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "playbackId") == 0) {
			args.playback_id = (i->value);
		} else
		{}
	}
	ast_ari_playbacks_get(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 0: /* Implementation is still a stub, or the code wasn't set */
		is_valid = response->message == NULL;
		break;
	case 500: /* Internal Server Error */
	case 501: /* Not Implemented */
	case 404: /* The playback cannot be found */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ast_ari_validate_playback(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /playbacks/{playbackId}\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /playbacks/{playbackId}\n");
		ast_ari_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */

fin: __attribute__((unused))
	return;
}
/*!
 * \brief Parameter parsing callback for /playbacks/{playbackId}.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void ast_ari_playbacks_stop_cb(
	struct ast_tcptls_session_instance *ser,
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	struct ast_ari_playbacks_stop_args args = {};
	struct ast_variable *i;
	RAII_VAR(struct ast_json *, body, NULL, ast_json_unref);
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "playbackId") == 0) {
			args.playback_id = (i->value);
		} else
		{}
	}
	ast_ari_playbacks_stop(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 0: /* Implementation is still a stub, or the code wasn't set */
		is_valid = response->message == NULL;
		break;
	case 500: /* Internal Server Error */
	case 501: /* Not Implemented */
	case 404: /* The playback cannot be found */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ast_ari_validate_void(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /playbacks/{playbackId}\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /playbacks/{playbackId}\n");
		ast_ari_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */

fin: __attribute__((unused))
	return;
}
/*!
 * \brief Parameter parsing callback for /playbacks/{playbackId}/control.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void ast_ari_playbacks_control_cb(
	struct ast_tcptls_session_instance *ser,
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	struct ast_ari_playbacks_control_args args = {};
	struct ast_variable *i;
	RAII_VAR(struct ast_json *, body, NULL, ast_json_unref);
	struct ast_json *field;
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "operation") == 0) {
			args.operation = (i->value);
		} else
		{}
	}
	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "playbackId") == 0) {
			args.playback_id = (i->value);
		} else
		{}
	}
	/* Look for a JSON request entity */
	body = ast_http_get_json(ser, headers);
	if (!body) {
		switch (errno) {
		case EFBIG:
			ast_ari_response_error(response, 413, "Request Entity Too Large", "Request body too large");
			goto fin;
		case ENOMEM:
			ast_ari_response_error(response, 500, "Internal Server Error", "Error processing request");
			goto fin;
		case EIO:
			ast_ari_response_error(response, 400, "Bad Request", "Error parsing request body");
			goto fin;
		}
	}
	/* Parse query parameters out of it */
	field = ast_json_object_get(body, "operation");
	if (field) {
		args.operation = ast_json_string_get(field);
	}
	ast_ari_playbacks_control(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 0: /* Implementation is still a stub, or the code wasn't set */
		is_valid = response->message == NULL;
		break;
	case 500: /* Internal Server Error */
	case 501: /* Not Implemented */
	case 400: /* The provided operation parameter was invalid */
	case 404: /* The playback cannot be found */
	case 409: /* The operation cannot be performed in the playback's current state */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ast_ari_validate_void(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /playbacks/{playbackId}/control\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /playbacks/{playbackId}/control\n");
		ast_ari_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */

fin: __attribute__((unused))
	return;
}

/*! \brief REST handler for /api-docs/playbacks.{format} */
static struct stasis_rest_handlers playbacks_playbackId_control = {
	.path_segment = "control",
	.callbacks = {
		[AST_HTTP_POST] = ast_ari_playbacks_control_cb,
	},
	.num_children = 0,
	.children = {  }
};
/*! \brief REST handler for /api-docs/playbacks.{format} */
static struct stasis_rest_handlers playbacks_playbackId = {
	.path_segment = "playbackId",
	.is_wildcard = 1,
	.callbacks = {
		[AST_HTTP_GET] = ast_ari_playbacks_get_cb,
		[AST_HTTP_DELETE] = ast_ari_playbacks_stop_cb,
	},
	.num_children = 1,
	.children = { &playbacks_playbackId_control, }
};
/*! \brief REST handler for /api-docs/playbacks.{format} */
static struct stasis_rest_handlers playbacks = {
	.path_segment = "playbacks",
	.callbacks = {
	},
	.num_children = 1,
	.children = { &playbacks_playbackId, }
};

static int load_module(void)
{
	int res = 0;
	stasis_app_ref();
	res |= ast_ari_add_handler(&playbacks);
	return res;
}

static int unload_module(void)
{
	ast_ari_remove_handler(&playbacks);
	stasis_app_unref();
	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "RESTful API module - Playback control resources",
	.load = load_module,
	.unload = unload_module,
	.nonoptreq = "res_ari,res_stasis",
	);