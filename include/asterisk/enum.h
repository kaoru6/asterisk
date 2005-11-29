/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * ENUM support
 * 
 * Copyright (C) 1999-2005, Digium, inc
 *
 * Mark Spencer <markster@digium.com>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */

/*!	\file enum.h
	\brief DNS and ENUM functions
*/

#ifndef _ASTERISK_ENUM_H
#define _ASTERISK_ENUM_H
#include "asterisk/channel.h"
/*! \brief Lookup entry in ENUM Returns 1 if found, 0 if not found, -1 on hangup 
	\param chan	Channel
	\param number	Number in E164 format without the + (for e164.arpa) or format 
			requested by enum service used (enum.conf)
	\param location Number returned	(or SIP uri)
	\param maxloc	Max length
	\param tech	Technology (from url scheme in response)
	\param maxtech	Max length
*/
extern int ast_get_enum(struct ast_channel *chan, const char *number, char *location, int maxloc, char *technology, int maxtech);

/*!	\brief Lookup DNS TXT record (used by app TXTCIDnum
	\param chan	Channel
	\param number	E164 number without the +
	\param locatio	Number returned	(or SIP uri)
	\param maxloc	Max length of number
	\param tech 	Technology (not used in TXT records)
	\param maxtech	Max length
	\param txt	Text string (return value)
	\param maxtxt	Max length of "txt"
*/
extern int ast_get_txt(struct ast_channel *chan, const char *number, char *location, int maxloc, char *technology, int maxtech, char *txt, int maxtxt);

extern int ast_enum_init(void);
extern int ast_enum_reload(void);
#endif
