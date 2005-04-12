/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Internal Asterisk's hangup causes
 *
 * Copyright (C) 2003, Digium
 *
 * Martin Pycko <martinp@digium.com>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */

#ifndef _ASTERISK_CAUSES_H
#define _ASTERISK_CAUSES_H

/* Causes for disconnection (from Q.931) */
#define AST_CAUSE_UNALLOCATED				1
#define AST_CAUSE_NO_ROUTE_TRANSIT_NET			2
#define AST_CAUSE_NO_ROUTE_DESTINATION			3
#define AST_CAUSE_CHANNEL_UNACCEPTABLE			6
#define AST_CAUSE_CALL_AWARDED_DELIVERED		7
#define AST_CAUSE_NORMAL_CLEARING			16
#define AST_CAUSE_USER_BUSY				17
#define AST_CAUSE_NO_USER_RESPONSE			18
#define AST_CAUSE_NO_ANSWER				19
#define AST_CAUSE_CALL_REJECTED				21
#define AST_CAUSE_NUMBER_CHANGED			22
#define AST_CAUSE_DESTINATION_OUT_OF_ORDER		27
#define AST_CAUSE_INVALID_NUMBER_FORMAT			28
#define AST_CAUSE_FACILITY_REJECTED			29
#define AST_CAUSE_RESPONSE_TO_STATUS_ENQUIRY		30
#define AST_CAUSE_NORMAL_UNSPECIFIED			31
#define AST_CAUSE_NORMAL_CIRCUIT_CONGESTION		34
#define AST_CAUSE_NETWORK_OUT_OF_ORDER			38
#define AST_CAUSE_NORMAL_TEMPORARY_FAILURE		41
#define AST_CAUSE_SWITCH_CONGESTION			42
#define AST_CAUSE_ACCESS_INFO_DISCARDED			43
#define AST_CAUSE_REQUESTED_CHAN_UNAVAIL		44
#define AST_CAUSE_PRE_EMPTED				45
#define AST_CAUSE_FACILITY_NOT_SUBSCRIBED  		50
#define AST_CAUSE_OUTGOING_CALL_BARRED     		52
#define AST_CAUSE_INCOMING_CALL_BARRED     		54
#define AST_CAUSE_BEARERCAPABILITY_NOTAUTH		57
#define AST_CAUSE_BEARERCAPABILITY_NOTAVAIL     	58
#define AST_CAUSE_BEARERCAPABILITY_NOTIMPL		65
#define AST_CAUSE_CHAN_NOT_IMPLEMENTED     		66
#define AST_CAUSE_FACILITY_NOT_IMPLEMENTED      	69
#define AST_CAUSE_INVALID_CALL_REFERENCE		81
#define AST_CAUSE_INCOMPATIBLE_DESTINATION		88
#define AST_CAUSE_INVALID_MSG_UNSPECIFIED  		95
#define AST_CAUSE_MANDATORY_IE_MISSING			96
#define AST_CAUSE_MESSAGE_TYPE_NONEXIST			97
#define AST_CAUSE_WRONG_MESSAGE				98
#define AST_CAUSE_IE_NONEXIST				99
#define AST_CAUSE_INVALID_IE_CONTENTS			100
#define AST_CAUSE_WRONG_CALL_STATE			101
#define AST_CAUSE_RECOVERY_ON_TIMER_EXPIRE		102
#define AST_CAUSE_MANDATORY_IE_LENGTH_ERROR		103
#define AST_CAUSE_PROTOCOL_ERROR			111
#define AST_CAUSE_INTERWORKING				127

/* Special Asterisk aliases */
#define AST_CAUSE_BUSY 					AST_CAUSE_USER_BUSY
#define AST_CAUSE_FAILURE 				AST_CAUSE_NETWORK_OUT_OF_ORDER
#define AST_CAUSE_NORMAL 				AST_CAUSE_NORMAL_CLEARING
#define AST_CAUSE_NOANSWER	 			AST_CAUSE_NO_ANSWER
#define AST_CAUSE_CONGESTION	 			AST_CAUSE_NORMAL_CIRCUIT_CONGESTION
#define AST_CAUSE_UNREGISTERED				AST_CAUSE_NO_ROUTE_DESTINATION
#define AST_CAUSE_NOTDEFINED 				0
#define AST_CAUSE_NOSUCHDRIVER				AST_CAUSE_CHAN_NOT_IMPLEMENTED

#endif
