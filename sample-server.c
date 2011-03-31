/*
 * Example mini-ipc server
 *
 * Copyright (C) 2011 CERN (www.cern.ch)
 * Author: Alessandro Rubini <rubini@gnudd.com>
 *
 * Released in the public domain
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "minipc.h"

static int ss_do_sum(int a, int b)
{
	return a + b;
}


/* These are the ones called from outside */
static int ss_sum_function(const struct minipc_pd *pd,
		       uint32_t *args, uint32_t *ret)
{
	int a, b, c;

	/* unmarshall the arguments, call, marshall back */
	minipc_unmarshall_args(pd, args, &a, &b, NULL);
	c = ss_do_sum(a, b);
	return minipc_marshall_ret(pd, ret, &c, NULL);
}

static int ss_tod_function(const struct minipc_pd *pd,
		       uint32_t *args, uint32_t *ret)
{
	struct timeval tv;
	int i;

	minipc_unmarshall_args(pd, args, NULL);
	i = gettimeofday(&tv, NULL);
	return minipc_marshall_ret(pd, ret, &tv, NULL);
}

/* Describe the two functions above */
const struct minipc_pd ss_sum_struct = {
	.f = ss_sum_function,
	.id = 0x73756d00, /* "sum\0" */
	.retval = MINIPC_ARG_ENCODE(MINIPC_AT_INT, int),
	.args = {
		MINIPC_ARG_ENCODE(MINIPC_AT_INT, int),
		MINIPC_ARG_ENCODE(MINIPC_AT_INT, int),
		0
	},
};

const struct minipc_pd ss_tod_struct = {
	.f = ss_tod_function,
	.id = 0x746f6400, /* "tod\0" */
	.retval = MINIPC_ARG_ENCODE(MINIPC_AT_STRUCT, struct timeval),
	.args = {
		0
	},
};

int main(int argc, char **argv)
{
	struct minipc_ch *server;

	server = minipc_server_create("sample", 0);
	if (!server)
		exit(1);
	minipc_set_logfile(server, stderr);
	minipc_export(server, "sum", &ss_sum_struct);
	minipc_export(server, "gettimeofday", &ss_tod_struct);
	while (1) {
		minipc_server_action(server, 1000);
		fprintf(stdout, "%s: looping...\n", __func__);
	}
}
