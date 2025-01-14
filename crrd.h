/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or https://opensource.org/licenses/CDDL-1.0.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright (c) 2024 Klara Inc.
 *
 * This software was developed by
 * Fred Weigel <fred.weigel@klarasystems.com>
 * Mariusz Zaborski <mariusz.zaborski@klarasystems.com>
 * under sponsorship from Wasabi Technology, Inc. and Klara Inc.
 */

#ifndef _CRRD_H_
#define _CRRD_H_

#ifdef TESTING
#include <stdint.h>
#include <sys/time.h>
typedef long long longlong_t;
typedef longlong_t hrtime_t;
#endif

#define	RRD_MAX_ENTRIES	512

typedef enum {
	DBRRD_FLOOR,
	DBRRD_CEILING
} dbrrd_rounding_t;

typedef struct {
	hrtime_t	rrdd_time;
	uint64_t	rrdd_txg;
} rrd_data_t;

typedef struct {
	size_t		rrd_head;	/* head (beginning) */
	size_t		rrd_tail;	/* tail (end) */
	size_t		rrd_length;

	rrd_data_t	rrd_entries[RRD_MAX_ENTRIES];
} rrd_t;

typedef struct {
	rrd_t		dbr_minutes;
	rrd_t		dbr_days;
	rrd_t		dbr_months;
} dbrrd_t;

rrd_t *rrd_create(void);
size_t rrd_len(rrd_t *rrd);

const rrd_data_t *rrd_entry(rrd_t *r, size_t i);
const rrd_data_t *rrd_tail_entry(rrd_t *rrd);
uint64_t rrd_tail(rrd_t *rrd);
uint64_t rrd_get(rrd_t *rrd, size_t i);

void rrd_add(rrd_t *rrd, hrtime_t time, uint64_t txg);

void dbrrd_add(dbrrd_t *db, hrtime_t time, uint64_t txg);
uint64_t dbrrd_query(dbrrd_t *r, hrtime_t tv, dbrrd_rounding_t rouding);

#endif
