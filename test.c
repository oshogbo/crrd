/*
 * Test functions for crrd
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "crrd.h"

/*
 * Two macros:
 *
 * SEC2HR converts time in seconds to hrtime_t
 * HR2SEC converts hrtime_t to seconds
 */
#define SEC2HR(s) ((hrtime_t)((s) * 1000LL * 1000LL * 1000LL))
#define HR2SEC(s) ((long)((s) / (1000LL * 1000LL * 1000LL)))

void
rrd_print(rrd_t *r)
{

	for (size_t i = 0; i < rrd_len(r); i++) {
		const rrd_data_t *data;

		data = rrd_entry(r, i);
		printf("%lld: %ld\n", data->rrdd_time, data->rrdd_txg);
	}
}

void
dbrrd_print(dbrrd_t *db)
{

	printf("Minutes list:\n");
	rrd_print(&db->dbr_minutes);
	printf("Days list:\n");
	rrd_print(&db->dbr_days);
	printf("Months list:\n");
	rrd_print(&db->dbr_months);
}

#define CCRD_ASSERT_EQ(db, time, txg) do {					\
		if (dbrrd_query(db, time) != txg) {				\
			printf("%d failed: query(%lld) %lu != %lu\n",		\
			    __LINE__, time, dbrrd_query(db, time), txg);	\
			dbrrd_print(db);					\
			exit(1);						\
		}								\
	} while (0)


void
lineral_test()
{
	struct {
		hrtime_t time;
		uint64_t txg;
	} test[] = {
		{5, 1},
		{10, 2},
		{15, 3},
		{20, 4},
		{30, 5},
	};
	dbrrd_t db = {0};

	for (size_t i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
		dbrrd_add(&db, test[i].time, test[i].txg);
		CCRD_ASSERT_EQ(&db, test[i].time, test[i].txg);
	}

	for (size_t i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
		CCRD_ASSERT_EQ(&db, test[i].time, test[i].txg);
	}
}

void
days_test()
{
	dbrrd_t db = {0};

	for (size_t i = 0; i < 1024; i++) {
		dbrrd_add(&db, SEC2HR(5 * i * 60 + 60 * 3600), i);
	}

	CCRD_ASSERT_EQ(&db, SEC2HR(5 * 60 + 60 * 3600), (uint64_t)0U);
}

void
month_test()
{
	dbrrd_t db = {0};

	for (size_t i = 0; i < 1024; i++) {
		dbrrd_add(&db, SEC2HR(i * 60 * 3660 + 60 * 3600 * 30), i);
	}

	CCRD_ASSERT_EQ(&db, SEC2HR(2 * 60 * 3600 * 30), (uint64_t)30U);
}

void
prefirst_test()
{
	dbrrd_t db = {0};

	for (size_t i = 0; i < 512; i++) {
		dbrrd_add(&db, 5 * (i + 1), i);
	}

	CCRD_ASSERT_EQ(&db, (hrtime_t)1, (uint64_t)0U);
}

void
postlast_test()
{
	dbrrd_t db = {0};

	for (size_t i = 0; i < 512; i++) {
		dbrrd_add(&db, 5 * (i + 1), i);
	}

	CCRD_ASSERT_EQ(&db, (hrtime_t)5120, (uint64_t)511U);
}

int
main(int ac, char **av)
{
	printf("crrd - C RRD Database\n");

	lineral_test();
	days_test();
	month_test();
	prefirst_test();
	postlast_test();

	printf("test passed\n");

	return (0);
}
