/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Utility functions
 *
 * Copyright (C) 2004, Digium
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */

#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h> 
#include <asterisk/lock.h>
#include <asterisk/utils.h>

#if defined(__FreeBSD__) || defined(__OpenBSD__)

/* duh? ERANGE value copied from web... */
#define ERANGE 34
#undef gethostbyname

AST_MUTEX_DEFINE_STATIC(__mutex);

static int gethostbyname_r (const char *name, struct hostent *ret, char *buf,
                            size_t buflen, struct hostent **result, 
                            int *h_errnop) 
{
	int hsave;
	struct hostent *ph;
	ast_mutex_lock(&__mutex); /* begin critical area */
	hsave = h_errno;

	ph = gethostbyname(name);
	*h_errnop = h_errno; /* copy h_errno to *h_herrnop */
	if (ph == NULL) {
		*result = NULL;
	} else {
		char **p, **q;
		char *pbuf;
		int nbytes=0;
		int naddr=0, naliases=0;
		/* determine if we have enough space in buf */

		/* count how many addresses */
		for (p = ph->h_addr_list; *p != 0; p++) {
			nbytes += ph->h_length; /* addresses */
			nbytes += sizeof(*p); /* pointers */
			naddr++;
		}
		nbytes += sizeof(*p); /* one more for the terminating NULL */

		/* count how many aliases, and total length of strings */
		for (p = ph->h_aliases; *p != 0; p++) {
			nbytes += (strlen(*p)+1); /* aliases */
			nbytes += sizeof(*p);  /* pointers */
			naliases++;
		}
		nbytes += sizeof(*p); /* one more for the terminating NULL */

		/* here nbytes is the number of bytes required in buffer */
		/* as a terminator must be there, the minimum value is ph->h_length */
		if(nbytes > buflen) {
			*result = NULL;
			ast_mutex_unlock(&__mutex); /* end critical area */
			return ERANGE; /* not enough space in buf!! */
		}

		/* There is enough space. Now we need to do a deep copy! */
		/* Allocation in buffer:
			from [0] to [(naddr-1) * sizeof(*p)]:
			pointers to addresses
			at [naddr * sizeof(*p)]:
			NULL
			from [(naddr+1) * sizeof(*p)] to [(naddr+naliases) * sizeof(*p)] :
			pointers to aliases
			at [(naddr+naliases+1) * sizeof(*p)]:
			NULL
			then naddr addresses (fixed length), and naliases aliases (asciiz).
		*/

		*ret = *ph;   /* copy whole structure (not its address!) */

		/* copy addresses */
		q = (char **)buf; /* pointer to pointers area (type: char **) */
		ret->h_addr_list = q; /* update pointer to address list */
		pbuf = buf + ((naddr+naliases+2)*sizeof(*p)); /* skip that area */
		for (p = ph->h_addr_list; *p != 0; p++) {
			memcpy(pbuf, *p, ph->h_length); /* copy address bytes */
			*q++ = pbuf; /* the pointer is the one inside buf... */
			pbuf += ph->h_length; /* advance pbuf */
		}
		*q++ = NULL; /* address list terminator */

		/* copy aliases */
		ret->h_aliases = q; /* update pointer to aliases list */
		for (p = ph->h_aliases; *p != 0; p++) {
			strcpy(pbuf, *p); /* copy alias strings */
			*q++ = pbuf; /* the pointer is the one inside buf... */
			pbuf += strlen(*p); /* advance pbuf */
			*pbuf++ = 0; /* string terminator */
		}
		*q++ = NULL; /* terminator */

		strcpy(pbuf, ph->h_name); /* copy alias strings */
		ret->h_name = pbuf;
		pbuf += strlen(ph->h_name); /* advance pbuf */
		*pbuf++ = 0; /* string terminator */

		*result = ret;  /* and let *result point to structure */

	}
	h_errno = hsave;  /* restore h_errno */
	ast_mutex_unlock(&__mutex); /* end critical area */

	return (*result == NULL); /* return 0 on success, non-zero on error */
}


#endif

struct hostent *ast_gethostbyname(const char *host, struct ast_hostent *hp)
{
	int res;
	int herrno;
	const char *s;
	struct hostent *result = NULL;
	/* Although it is perfectly legitimate to lookup a pure integer, for
	   the sake of the sanity of people who like to name their peers as
	   integers, we break with tradition and refuse to look up a
	   pure integer */
	s = host;
	while(s && *s) {
		if (!isdigit(*s))
			break;
		s++;
	}
	if (!s || !*s)
		return NULL;
	res = gethostbyname_r(host, &hp->hp, hp->buf, sizeof(hp->buf), &result, &herrno);

	if (res || !result || !hp->hp.h_addr_list || !hp->hp.h_addr_list[0])
		return NULL;
	return &hp->hp;
}


/* This is a regression test for recursive mutexes.
   test_for_thread_safety() will return 0 if recursive mutex locks are
   working properly, and non-zero if they are not working properly. */

AST_MUTEX_DEFINE_STATIC(test_lock);
AST_MUTEX_DEFINE_STATIC(test_lock2);
static pthread_t test_thread; 
static int lock_count = 0;
static int test_errors = 0;

static void *test_thread_body(void *data) 
{ 
  ast_mutex_lock(&test_lock);
  lock_count += 10;
  if(lock_count != 10) test_errors++;
  ast_mutex_lock(&test_lock);
  lock_count += 10;
  if(lock_count != 20) test_errors++;
  ast_mutex_lock(&test_lock2);
  ast_mutex_unlock(&test_lock);
  lock_count -= 10;
  if(lock_count != 10) test_errors++;
  ast_mutex_unlock(&test_lock);
  lock_count -= 10;
  ast_mutex_unlock(&test_lock2);
  if(lock_count != 0) test_errors++;
  return NULL;
} 

int test_for_thread_safety(void)
{ 
  ast_mutex_lock(&test_lock2);
  ast_mutex_lock(&test_lock);
  lock_count += 1;
  ast_mutex_lock(&test_lock);
  lock_count += 1;
  pthread_create(&test_thread, NULL, test_thread_body, NULL); 
  pthread_yield();
  usleep(100);
  if(lock_count != 2) test_errors++;
  ast_mutex_unlock(&test_lock);
  lock_count -= 1;
  pthread_yield(); 
  usleep(100); 
  if(lock_count != 1) test_errors++;
  ast_mutex_unlock(&test_lock);
  lock_count -= 1;
  if(lock_count != 0) test_errors++;
  ast_mutex_unlock(&test_lock2);
  pthread_yield();
  usleep(100);
  if(lock_count != 0) test_errors++;
  pthread_join(test_thread, NULL);
  return(test_errors);          /* return 0 on success. */
}
