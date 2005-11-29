/*	$NetBSD: sig.c,v 1.9 2002/03/18 16:00:58 christos Exp $	*/

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Christos Zoulas of Cornell University.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)sig.c	8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: sig.c,v 1.9 2002/03/18 16:00:58 christos Exp $");
#endif
#endif /* not lint && not SCCSID */

/*
 * sig.c: Signal handling stuff.
 *	  our policy is to trap all signals, set a good state
 *	  and pass the ball to our caller.
 */
#include "el.h"
#include <stdlib.h>

private EditLine *sel = NULL;

private const int sighdl[] = {
#define	_DO(a)	(a),
	ALLSIGS
#undef	_DO
	- 1
};

private void sig_handler(int);

/* sig_handler():
 *	This is the handler called for all signals
 *	XXX: we cannot pass any data so we just store the old editline
 *	state in a private variable
 */
private void
sig_handler(int signo)
{
	int i;
	sigset_t nset, oset;

	(void) sigemptyset(&nset);
	(void) sigaddset(&nset, signo);
	(void) sigprocmask(SIG_BLOCK, &nset, &oset);

	switch (signo) {
	case SIGCONT:
		tty_rawmode(sel);
		if (ed_redisplay(sel, 0) == CC_REFRESH)
			re_refresh(sel);
		term__flush();
		break;

	case SIGWINCH:
		el_resize(sel);
		break;

	default:
		tty_cookedmode(sel);
		break;
	}

	for (i = 0; sighdl[i] != -1; i++)
		if (signo == sighdl[i])
			break;

	(void) signal(signo, sel->el_signal[i]);
	(void) sigprocmask(SIG_SETMASK, &oset, NULL);
	(void) kill(0, signo);
}


/* sig_init():
 *	Initialize all signal stuff
 */
protected int
sig_init(EditLine *el)
{
	int i;
	sigset_t nset, oset;

	(void) sigemptyset(&nset);
#define	_DO(a) (void) sigaddset(&nset, a);
	ALLSIGS
#undef	_DO
	    (void) sigprocmask(SIG_BLOCK, &nset, &oset);

#define	SIGSIZE (sizeof(sighdl) / sizeof(sighdl[0]) * sizeof(sig_t))

	el->el_signal = (sig_t *) el_malloc(SIGSIZE);
	if (el->el_signal == NULL)
		return (-1);
	for (i = 0; sighdl[i] != -1; i++)
		el->el_signal[i] = SIG_ERR;

	(void) sigprocmask(SIG_SETMASK, &oset, NULL);

	return (0);
}


/* sig_end():
 *	Clear all signal stuff
 */
protected void
sig_end(EditLine *el)
{

	el_free((ptr_t) el->el_signal);
	el->el_signal = NULL;
}


/* sig_set():
 *	set all the signal handlers
 */
protected void
sig_set(EditLine *el)
{
	int i;
	sigset_t nset, oset;

	(void) sigemptyset(&nset);
#define	_DO(a) (void) sigaddset(&nset, a);
	ALLSIGS
#undef	_DO
	    (void) sigprocmask(SIG_BLOCK, &nset, &oset);

	for (i = 0; sighdl[i] != -1; i++) {
		sig_t s;
		/* This could happen if we get interrupted */
		if ((s = signal(sighdl[i], sig_handler)) != sig_handler)
			el->el_signal[i] = s;
	}
	sel = el;
	(void) sigprocmask(SIG_SETMASK, &oset, NULL);
}


/* sig_clr():
 *	clear all the signal handlers
 */
protected void
sig_clr(EditLine *el)
{
	int i;
	sigset_t nset, oset;

	(void) sigemptyset(&nset);
#define	_DO(a) (void) sigaddset(&nset, a);
	ALLSIGS
#undef	_DO
	    (void) sigprocmask(SIG_BLOCK, &nset, &oset);

	for (i = 0; sighdl[i] != -1; i++)
		if (el->el_signal[i] != SIG_ERR)
			(void) signal(sighdl[i], el->el_signal[i]);

	sel = NULL;		/* we are going to die if the handler is
				 * called */
	(void) sigprocmask(SIG_SETMASK, &oset, NULL);
}
