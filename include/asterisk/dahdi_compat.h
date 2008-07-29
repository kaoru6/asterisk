/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2008, Digium, Inc.
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

/*! 
 * \file
 * \brief DAHDI compatibility with zaptel
 */

#ifndef DAHDI_COMPAT_H
#define DAHDI_COMPAT_H

#if defined(HAVE_DAHDI)

#include <dahdi/user.h>

#elif defined(HAVE_ZAPTEL)

#include <zaptel/zaptel.h>

/* Compiling against Zaptel instead of DAHDI */

#define __DAHDI_SIG_FXO __ZT_SIG_FXO
#define __DAHDI_SIG_FXS __ZT_SIG_FXS
#define DAHDI_ALARM_BLUE ZT_ALARM_BLUE
#define DAHDI_ALARM_LOOPBACK ZT_ALARM_LOOPBACK
#define DAHDI_ALARM_NONE ZT_ALARM_NONE
#define DAHDI_ALARM_NOTOPEN ZT_ALARM_NOTOPEN
#define DAHDI_ALARM_RECOVER ZT_ALARM_RECOVER
#define DAHDI_ALARM_RED ZT_ALARM_RED
#define DAHDI_ALARM_YELLOW ZT_ALARM_YELLOW
#define DAHDI_AUDIOMODE ZT_AUDIOMODE
#define dahdi_bufferinfo zt_bufferinfo
#define DAHDI_CHANNO ZT_CHANNO
#define DAHDI_CHECK_HOOKSTATE ZT_CHECK_HOOKSTATE
#define DAHDI_CONF_CONF ZT_CONF_CONF
#define DAHDI_CONF_CONFANN ZT_CONF_CONFANN
#define DAHDI_CONF_CONFANNMON ZT_CONF_CONFANNMON
#define DAHDI_CONF_CONFMON ZT_CONF_CONFMON
#define DAHDI_CONF_DIGITALMON ZT_CONF_DIGITALMON
#define DAHDI_CONF_LISTENER ZT_CONF_LISTENER
#define DAHDI_CONF_MONITORBOTH ZT_CONF_MONITORBOTH
#define DAHDI_CONF_NORMAL ZT_CONF_NORMAL
#define DAHDI_CONF_PSEUDO_LISTENER ZT_CONF_PSEUDO_LISTENER
#define DAHDI_CONF_PSEUDO_TALKER ZT_CONF_PSEUDO_TALKER
#define DAHDI_CONF_REALANDPSEUDO ZT_CONF_REALANDPSEUDO
#define DAHDI_CONF_TALKER ZT_CONF_TALKER
#define DAHDI_CONFDIAG ZT_CONFDIAG
#define dahdi_confinfo zt_confinfo
#define DAHDI_CONFMUTE ZT_CONFMUTE
#define DAHDI_DEFAULT_NUM_BUFS ZT_DEFAULT_NUM_BUFS
#define DAHDI_DIAL ZT_DIAL
#define DAHDI_DIALING ZT_DIALING
#define DAHDI_DIAL_OP_APPEND ZT_DIAL_OP_APPEND
#define dahdi_dialoperation zt_dialoperation
#define DAHDI_DIAL_OP_REPLACE ZT_DIAL_OP_REPLACE
#define dahdi_dialparams zt_dialparams
#define DAHDI_ECHOCANCEL ZT_ECHOCANCEL
#define DAHDI_ECHOTRAIN ZT_ECHOTRAIN
#define DAHDI_EVENT_ALARM ZT_EVENT_ALARM
#define DAHDI_EVENT_BITSCHANGED ZT_EVENT_BITSCHANGED
#define DAHDI_EVENT_DIALCOMPLETE ZT_EVENT_DIALCOMPLETE
#define DAHDI_EVENT_DTMFDOWN ZT_EVENT_DTMFDOWN
#define DAHDI_EVENT_DTMFUP ZT_EVENT_DTMFUP
#define DAHDI_EVENT_EC_DISABLED ZT_EVENT_EC_DISABLED
#define DAHDI_EVENT_HOOKCOMPLETE ZT_EVENT_HOOKCOMPLETE
#define DAHDI_EVENT_NOALARM ZT_EVENT_NOALARM
#define DAHDI_EVENT_NONE ZT_EVENT_NONE
#define DAHDI_EVENT_ONHOOK ZT_EVENT_ONHOOK
#define DAHDI_EVENT_POLARITY ZT_EVENT_POLARITY
#define DAHDI_EVENT_PULSEDIGIT ZT_EVENT_PULSEDIGIT
#define DAHDI_EVENT_PULSE_START ZT_EVENT_PULSE_START
#define DAHDI_EVENT_REMOVED ZT_EVENT_REMOVED
#define DAHDI_EVENT_RINGBEGIN ZT_EVENT_RINGBEGIN
#define DAHDI_EVENT_RINGEROFF ZT_EVENT_RINGEROFF
#define DAHDI_EVENT_RINGERON ZT_EVENT_RINGERON
#define DAHDI_EVENT_RINGOFFHOOK ZT_EVENT_RINGOFFHOOK
#define DAHDI_EVENT_TIMER_EXPIRED ZT_EVENT_TIMER_EXPIRED
#define DAHDI_EVENT_TIMER_PING ZT_EVENT_TIMER_PING
#define DAHDI_EVENT_WINKFLASH ZT_EVENT_WINKFLASH
#define DAHDI_FLASH ZT_FLASH
#define DAHDI_FLUSH ZT_FLUSH
#define DAHDI_FLUSH_ALL ZT_FLUSH_ALL
#define DAHDI_FLUSH_BOTH ZT_FLUSH_BOTH
#define DAHDI_FLUSH_READ ZT_FLUSH_READ
#define DAHDI_FLUSH_WRITE ZT_FLUSH_WRITE
#define dahdi_gains zt_gains
#define DAHDI_GET_BUFINFO ZT_GET_BUFINFO
#define DAHDI_GETCONF ZT_GETCONF
#define DAHDI_GETCONFMUTE ZT_GETCONFMUTE
#define DAHDI_GETEVENT ZT_GETEVENT
#define DAHDI_GETGAINS ZT_GETGAINS
#define DAHDI_GET_PARAMS ZT_GET_PARAMS
#define DAHDI_HOOK ZT_HOOK
#define DAHDI_IOMUX ZT_IOMUX
#define DAHDI_IOMUX_READ ZT_IOMUX_READ
#define DAHDI_IOMUX_SIGEVENT ZT_IOMUX_SIGEVENT
#define DAHDI_IOMUX_WRITE ZT_IOMUX_WRITE
#define DAHDI_LAW_ALAW ZT_LAW_ALAW
#define DAHDI_LAW_DEFAULT ZT_LAW_DEFAULT
#define DAHDI_LAW_MULAW ZT_LAW_MULAW
#define DAHDI_MAX_NUM_BUFS ZT_MAX_NUM_BUFS
#define DAHDI_MAX_SPANS ZT_MAX_SPANS
#define DAHDI_OFFHOOK ZT_OFFHOOK
#define DAHDI_ONHOOK ZT_ONHOOK
#define DAHDI_ONHOOKTRANSFER ZT_ONHOOKTRANSFER
#define dahdi_params zt_params
#define DAHDI_POLICY_IMMEDIATE ZT_POLICY_IMMEDIATE
#define DAHDI_PRI ZT_PRI
#define DAHDI_RING ZT_RING
#define DAHDI_RINGOFF ZT_RINGOFF
#define DAHDI_SENDTONE ZT_SENDTONE
#define DAHDI_SET_BLOCKSIZE ZT_SET_BLOCKSIZE
#define DAHDI_SET_BUFINFO ZT_SET_BUFINFO
#define DAHDI_SETCADENCE ZT_SETCADENCE
#define DAHDI_SETCONF ZT_SETCONF
#define DAHDI_SET_DIALPARAMS ZT_SET_DIALPARAMS
#define DAHDI_SETGAINS ZT_SETGAINS
#define DAHDI_SETLAW ZT_SETLAW
#define DAHDI_SETLINEAR ZT_SETLINEAR
#define DAHDI_SET_PARAMS ZT_SET_PARAMS
#define DAHDI_SETTONEZONE ZT_SETTONEZONE
#define DAHDI_SIG_CLEAR ZT_SIG_CLEAR
#define DAHDI_SIG_EM ZT_SIG_EM
#define DAHDI_SIG_EM_E1 ZT_SIG_EM_E1
#define DAHDI_SIG_FXO ZT_SIG_FXO
#define DAHDI_SIG_FXOGS ZT_SIG_FXOGS
#define DAHDI_SIG_FXOKS ZT_SIG_FXOKS
#define DAHDI_SIG_FXOLS ZT_SIG_FXOLS
#define DAHDI_SIG_FXS ZT_SIG_FXS
#define DAHDI_SIG_FXSGS ZT_SIG_FXSGS
#define DAHDI_SIG_FXSKS ZT_SIG_FXSKS
#define DAHDI_SIG_FXSLS ZT_SIG_FXSLS
#define DAHDI_SIG_HARDHDLC ZT_SIG_HARDHDLC
#define DAHDI_SIG_HDLCFCS ZT_SIG_HDLCFCS
#define DAHDI_SIG_SF ZT_SIG_SF
#define dahdi_spaninfo zt_spaninfo
#define DAHDI_SPANSTAT ZT_SPANSTAT
#define DAHDI_SPECIFY ZT_SPECIFY
#define DAHDI_START ZT_START
#define DAHDI_TCOP_ALLOCATE ZT_TCOP_ALLOCATE
#define DAHDI_TCOP_GETINFO ZT_TCOP_GETINFO
#define DAHDI_TCOP_RELEASE ZT_TCOP_RELEASE
#define DAHDI_TCOP_TRANSCODE ZT_TCOP_TRANSCODE
#define DAHDI_TIMERACK ZT_TIMERACK
#define DAHDI_TIMERCONFIG ZT_TIMERCONFIG
#define DAHDI_TIMERPING ZT_TIMERPING
#define DAHDI_TIMERPONG ZT_TIMERPONG
#define DAHDI_TONE_BUSY ZT_TONE_BUSY
#define DAHDI_TONE_CONGESTION ZT_TONE_CONGESTION
#define DAHDI_TONEDETECT ZT_TONEDETECT
#define DAHDI_TONEDETECT_MUTE ZT_TONEDETECT_MUTE
#define DAHDI_TONEDETECT_ON ZT_TONEDETECT_ON
#define DAHDI_TONE_DIALRECALL ZT_TONE_DIALRECALL
#define DAHDI_TONE_DIALTONE ZT_TONE_DIALTONE
#define DAHDI_TONE_DTMF_A ZT_TONE_DTMF_A
#define DAHDI_TONE_DTMF_BASE ZT_TONE_DTMF_BASE
#define DAHDI_TONE_DTMF_p ZT_TONE_DTMF_p
#define DAHDI_TONE_DTMF_s ZT_TONE_DTMF_s
#define DAHDI_TONE_INFO ZT_TONE_INFO
#define DAHDI_TONE_RINGTONE ZT_TONE_RINGTONE
#define DAHDI_TONE_STUTTER ZT_TONE_STUTTER
#define dahdi_transcode_header zt_transcode_header
#define dahdi_transcode_info zt_transcode_info
#define DAHDI_TRANSCODE_MAGIC ZT_TRANSCODE_MAGIC
#define DAHDI_TRANSCODE_OP ZT_TRANSCODE_OP
#define DAHDI_vldtmf ZT_vldtmf
#define DAHDI_WINK ZT_WINK
#define HAVE_DAHDI HAVE_ZAPTEL

#define dahdi_ring_cadence zt_ring_cadence

#endif

#endif /* DAHDI_COMPAT_H */
