/*
 * asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2013, Digium, Inc.
 *
 * Kevin Harwell <kharwell@digium.com>
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

/*** MODULEINFO
	<depend>pjproject</depend>
	<depend>res_pjsip</depend>
	<depend>res_pjsip_pubsub</depend>
	<depend>res_pjsip_exten_state</depend>
	<support_level>core</support_level>
 ***/

#include "asterisk.h"

#include <pjsip.h>
#include <pjsip_simple.h>
#include <pjlib.h>

#include "asterisk/module.h"
#include "asterisk/res_pjsip.h"
#include "asterisk/res_pjsip_pubsub.h"
#include "asterisk/res_pjsip_presence_xml.h"
#include "asterisk/res_pjsip_body_generator_types.h"

void ast_sip_sanitize_xml(const char *input, char *output, size_t len)
{
	char *copy = ast_strdupa(input);
	char *break_point;

	output[0] = '\0';

	while ((break_point = strpbrk(copy, "<>\"&'\n\r"))) {
		char to_escape = *break_point;

		*break_point = '\0';
		strncat(output, copy, len);

		switch (to_escape) {
		case '<':
			strncat(output, "&lt;", len);
			break;
		case '>':
			strncat(output, "&gt;", len);
			break;
		case '"':
			strncat(output, "&quot;", len);
			break;
		case '&':
			strncat(output, "&amp;", len);
			break;
		case '\'':
			strncat(output, "&apos;", len);
			break;
		case '\r':
			strncat(output, "&#13;", len);
			break;
		case '\n':
			strncat(output, "&#10;", len);
			break;
		};

		copy = break_point + 1;
	}

	/* Be sure to copy everything after the final bracket */
	if (*copy) {
		strncat(output, copy, len);
	}
}

void ast_sip_presence_exten_state_to_str(int state, char **statestring, char **pidfstate,
			       char **pidfnote, enum ast_sip_pidf_state *local_state)
{
	switch (state) {
	case AST_EXTENSION_RINGING:
		*statestring = "early";
		*local_state = NOTIFY_INUSE;
		*pidfstate = "busy";
		*pidfnote = "Ringing";
		break;
	case AST_EXTENSION_INUSE:
		*statestring = "confirmed";
		*local_state = NOTIFY_INUSE;
		*pidfstate = "busy";
		*pidfnote = "On the phone";
		break;
	case AST_EXTENSION_BUSY:
		*statestring = "confirmed";
		*local_state = NOTIFY_CLOSED;
		*pidfstate = "busy";
		*pidfnote = "On the phone";
		break;
	case AST_EXTENSION_UNAVAILABLE:
		*statestring = "terminated";
		*local_state = NOTIFY_CLOSED;
		*pidfstate = "away";
		*pidfnote = "Unavailable";
		break;
	case AST_EXTENSION_ONHOLD:
		*statestring = "confirmed";
		*local_state = NOTIFY_CLOSED;
		*pidfstate = "busy";
		*pidfnote = "On hold";
		break;
	case AST_EXTENSION_NOT_INUSE:
	default:
		/* Default setting */
		*statestring = "terminated";
		*local_state = NOTIFY_OPEN;
		*pidfstate = "--";
		*pidfnote ="Ready";
		break;
	}
}

pj_xml_attr *ast_sip_presence_xml_create_attr(pj_pool_t *pool,
		pj_xml_node *node, const char *name, const char *value)
{
	pj_xml_attr *attr = PJ_POOL_ALLOC_T(pool, pj_xml_attr);

	pj_strdup2(pool, &attr->name, name);
	pj_strdup2(pool, &attr->value, value);

	pj_xml_add_attr(node, attr);
	return attr;
}

pj_xml_node *ast_sip_presence_xml_create_node(pj_pool_t *pool,
		pj_xml_node *parent, const char* name)
{
	pj_xml_node *node = PJ_POOL_ALLOC_T(pool, pj_xml_node);

	pj_list_init(&node->attr_head);
	pj_list_init(&node->node_head);

	pj_strdup2(pool, &node->name, name);

	node->content.ptr = NULL;
	node->content.slen = 0;

	pj_xml_add_node(parent, node);
	return node;
}

void ast_sip_presence_xml_find_node_attr(pj_pool_t* pool,
		pj_xml_node *parent, const char *node_name, const char *attr_name,
		pj_xml_node **node, pj_xml_attr **attr)
{
	pj_str_t name;

	if (!(*node = pj_xml_find_node(parent, pj_cstr(&name, node_name)))) {
		*node = ast_sip_presence_xml_create_node(pool, parent, node_name);
	}

	if (!(*attr = pj_xml_find_attr(*node, pj_cstr(&name, attr_name), NULL))) {
		*attr = ast_sip_presence_xml_create_attr(pool, *node, attr_name, "");
	}
}
