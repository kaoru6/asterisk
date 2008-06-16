/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2008, Digium, Inc.
 *
 * Kevin P. Fleming <kpfleming@digium.com>
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
 * \brief Timing source management
 *
 * \author Kevin P. Fleming <kpfleming@digium.com>
 */

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include "asterisk/_private.h"

#include "asterisk/timing.h"
#include "asterisk/lock.h"
#include "asterisk/cli.h"
#include "asterisk/utils.h"
#include "asterisk/time.h"

AST_RWLOCK_DEFINE_STATIC(lock);

static struct ast_timing_functions timer_funcs;

void *ast_install_timing_functions(struct ast_timing_functions *funcs)
{
	if (!funcs->timer_open ||
	    !funcs->timer_close ||
		!funcs->timer_set_rate ||
	    !funcs->timer_ack ||
	    !funcs->timer_get_event ||
	    !funcs->timer_enable_continuous ||
	    !funcs->timer_disable_continuous) {
		return NULL;
	}

	ast_rwlock_wrlock(&lock);

	if (timer_funcs.timer_open) {
		ast_rwlock_unlock(&lock);
		ast_log(LOG_NOTICE, "Multiple timing modules are loaded.  You should only load one.\n");
		return NULL;
	}
	
	timer_funcs = *funcs;

	ast_rwlock_unlock(&lock);

	return &timer_funcs;
}

void ast_uninstall_timing_functions(void *handle)
{
	ast_rwlock_wrlock(&lock);

	if (handle != &timer_funcs) {
		ast_rwlock_unlock(&lock);
		return;
	}

	memset(&timer_funcs, 0, sizeof(timer_funcs));

	ast_rwlock_unlock(&lock);
}

int ast_timer_open(void)
{
	int timer;

	ast_rwlock_rdlock(&lock);

	if (!timer_funcs.timer_open) {
		ast_rwlock_unlock(&lock);
		return -1;
	}

	timer = timer_funcs.timer_open();

	ast_rwlock_unlock(&lock);

	return timer;
}

void ast_timer_close(int timer)
{
	ast_rwlock_rdlock(&lock);

	if (!timer_funcs.timer_close) {
		ast_rwlock_unlock(&lock);
		return;
	}

	timer_funcs.timer_close(timer);

	ast_rwlock_unlock(&lock);
}

int ast_timer_set_rate(int handle, unsigned int rate)
{
	int res;

	ast_rwlock_rdlock(&lock);

	if (!timer_funcs.timer_set_rate) {
		ast_rwlock_unlock(&lock);
		return -1;
	}

	res = timer_funcs.timer_set_rate(handle, rate);

	ast_rwlock_unlock(&lock);

	return res;
}

void ast_timer_ack(int handle, unsigned int quantity)
{
	ast_rwlock_rdlock(&lock);

	if (!timer_funcs.timer_ack) {
		ast_rwlock_unlock(&lock);
		return;
	}

	timer_funcs.timer_ack(handle, quantity);

	ast_rwlock_unlock(&lock);
}

int ast_timer_enable_continuous(int handle)
{
	int result;

	ast_rwlock_rdlock(&lock);

	if (!timer_funcs.timer_enable_continuous) {
		ast_rwlock_unlock(&lock);
		return -1;
	}

	result = timer_funcs.timer_enable_continuous(handle);

	ast_rwlock_unlock(&lock);

	return result;
}

int ast_timer_disable_continuous(int handle)
{
	int result;

	ast_rwlock_rdlock(&lock);

	if (!timer_funcs.timer_disable_continuous) {
		ast_rwlock_unlock(&lock);
		return -1;
	}

	result = timer_funcs.timer_disable_continuous(handle);

	ast_rwlock_unlock(&lock);

	return result;
}

enum ast_timing_event ast_timer_get_event(int handle)
{
	enum ast_timing_event result;

	ast_rwlock_rdlock(&lock);

	if (!timer_funcs.timer_get_event) {
		ast_rwlock_unlock(&lock);
		return -1;
	}

	result = timer_funcs.timer_get_event(handle);

	ast_rwlock_unlock(&lock);

	return result;
}

static char *timing_test(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a)
{
	int fd, count = 0;
	struct timeval start, end;

	switch (cmd) {
	case CLI_INIT:
		e->command = "timing test";
		e->usage = "Usage: timing test\n";
		return NULL;
	case CLI_GENERATE:
		return NULL;
	}

	ast_cli(a->fd, "Attempting to test a timer with 50 ticks per second ...\n");

	if ((fd = ast_timer_open()) == -1) {
		ast_cli(a->fd, "Failed to open timing fd\n");
		return CLI_FAILURE;
	}

	start = ast_tvnow();

	ast_timer_set_rate(fd, 50);

	while (ast_tvdiff_ms((end = ast_tvnow()), start) < 1000) {
		int res;
		struct pollfd pfd = {
			.fd = fd,
			.events = POLLIN | POLLPRI,
		};

		res = poll(&pfd, 1, 100);

		if (res == 1) {
			count++;
			ast_timer_ack(fd, 1);
		} else if (!res) {
			ast_cli(a->fd, "poll() timed out!  This is bad.\n");
		} else if (errno != EAGAIN && errno != EINTR) {
			ast_cli(a->fd, "poll() returned error: %s\n", strerror(errno));
		}
	}

	ast_timer_close(fd);

	ast_cli(a->fd, "It has been %d milliseconds, and we got %d timer ticks\n", 
		ast_tvdiff_ms(end, start), count);

	return CLI_SUCCESS;
}

static struct ast_cli_entry cli_timing[] = {
	AST_CLI_DEFINE(timing_test, "Run a timing test"),
};

int ast_timing_init(void)
{
	return ast_cli_register_multiple(cli_timing, ARRAY_LEN(cli_timing));
}
