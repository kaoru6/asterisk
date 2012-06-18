/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2011-2012, Digium, Inc.
 *
 * David Vossel <dvossel@digium.com>
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
 * \brief Presence state management
 */

#ifndef _ASTERISK_PRESSTATE_H
#define _ASTERISK_PRESSTATE_H

enum ast_presence_state {
	AST_PRESENCE_NOT_SET = 0,
	AST_PRESENCE_UNAVAILABLE,
	AST_PRESENCE_AVAILABLE,
	AST_PRESENCE_AWAY,
	AST_PRESENCE_XA,
	AST_PRESENCE_CHAT,
	AST_PRESENCE_DND,
};

/*! \brief Presence state provider call back */
typedef enum ast_presence_state (*ast_presence_state_prov_cb_type)(const char *data, char **subtype, char **message);

/*!
 * \brief Convert presence state to text string for output
 *
 * \param state Current presence state
 */
const char *ast_presence_state2str(enum ast_presence_state state);

/*!
 * \brief Convert presence state from text to integer value
 *
 * \param val The text representing the presence state.  Valid values are anything
 *        that comes after AST_PRESENCE_ in one of the defined values.
 *
 * \return The AST_PRESENCE_ integer value
 */
enum ast_presence_state ast_presence_state_val(const char *val);

/*!
 * \brief Asks a presence state provider for the current presence state.
 *
 * \param presence_provider, The presence provider to retrieve the state from.
 * \param subtype, The output paramenter to store the subtype string in. Must be freed if returned
 * \param message, The output paramenter to store the message string in. Must be freed if returned
 *
 * \retval presence state value on success,
 * \retval -1 on failure.
 */
enum ast_presence_state ast_presence_state(const char *presence_provider, char **subtype, char **message);

/*!
 * \brief Notify the world that a presence provider state changed.
 */
int ast_presence_state_changed(const char *presence_provider);

/*!
 * \brief Add presence state provider
 *
 * \param label to use in hint, like label:object
 * \param callback Callback
 *
 * \retval 0 success
 * \retval -1 failure
 */
int ast_presence_state_prov_add(const char *label, ast_presence_state_prov_cb_type callback);

/*!
 * \brief Remove presence state provider
 *
 * \param label to use in hint, like label:object
 *
 * \retval -1 on failure
 * \retval 0 on success
 */
int ast_presence_state_prov_del(const char *label);

int ast_presence_state_engine_init(void);
#endif
