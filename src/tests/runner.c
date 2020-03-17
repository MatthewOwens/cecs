#include "runner.h"
#include <check.h>

int run_suite(Suite *s)
{
	int failed_count = 0;
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	failed_count += srunner_ntests_failed(sr);
	srunner_free(sr);
	return failed_count;
}

int run_suite_forkless(Suite *s)
{
	int failed_count = 0;
	SRunner *sr = srunner_create(s);
	srunner_set_fork_status(sr, CK_NOFORK);
	srunner_run_all(sr, CK_NORMAL);
	failed_count += srunner_ntests_failed(sr);
	srunner_free(sr);
	return failed_count;
}
