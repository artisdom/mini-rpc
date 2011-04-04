/*
 * Private definition for mini-ipc
 *
 * Copyright (C) 2011 CERN (www.cern.ch)
 * Author: Alessandro Rubini <rubini@gnudd.com>
 * Based on ideas by Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 */
#ifndef __MINIPC_INT_H__
#define __MINIPC_INT_H__
#include <sys/un.h>
#include "minipc.h"

/*  be safe, in case some other header had them slightly differntly */
#undef container_of
#undef offsetof
#undef ARRAY_SIZE

/* We are strongly based on container_of internally */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/*
 * While public symbols are minipc_* internal ones are mpc_* to be shorter.
 * The connection includes an fd, which is the only thing returned back
 */

/* The list of functions attached to a service */
struct mpc_flist {
	const struct minipc_pd *pd;
	const char *name;
	struct mpc_flist *next;
};

/*
 * The main server or client structure. Server links have client sockets
 * hooking on it.
 */
struct mpc_link {
	struct minipc_ch ch;
	int magic;
	int flags;
	struct mpc_link *nextl;
	struct mpc_flist *flist;
	FILE *logf;
	struct sockaddr_un addr;
	char name[MINIPC_MAX_NAME];
	int fd[MINIPC_MAX_CLIENTS];
	fd_set fdset;
};
#define MPC_MAGIC		0xc0ffee99

#define MPC_FLAG_SERVER		0x00010000
#define MPC_FLAG_CLIENT		0x00020000
#define MPC_USER_FLAGS(x)	((x) & 0xffff)

#define MPC_TIMEOUT		1000 /* msec, hardwired */

static inline struct mpc_link *mpc_get_link(struct minipc_ch *ch)
{
	return container_of(ch, struct mpc_link, ch);
}

#define CHECK_LINK(link) /* Horrible shortcut, don't tell around... */	\
		if ((link)->magic != MPC_MAGIC) {			\
		errno = -EINVAL;					\
		return -1;						\
	}

extern struct mpc_link *__mpc_base;
extern void mpc_free_flist(struct mpc_link *link, struct mpc_flist *flist);

#endif /* __MINIPC_INT_H__ */