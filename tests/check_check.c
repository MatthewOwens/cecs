#include <check.h>
#include "runner.h"
#include "check_check.h"

START_TEST(test_test)
{
	int i = 3;
	ck_assert_int_eq(i, 3);
}
END_TEST

Suite * test_suite(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("test");
	tc = tcase_create("case");

	tcase_add_test(tc, test_test);
	suite_add_tcase(s, tc);

	return s;
}
