#include <check.h>
#include <stdlib.h>
#include <stdint.h>

START_TEST(test_test)
{
	int i = 3;
	ck_assert_int_eq(i, 3);
}
END_TEST

START_TEST(test_test_two)
{
	int i = 3;
	ck_assert_int_eq(i, 10);

END_TEST

Suite * test_suite(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("test");
	tc = tcase_create("case");

	tcase_add_test(tc, test_test);
	tcase_add_test(tc, test_test_two);
	suite_add_tcase(s, tc);

	return s;
}

Suite * ts_two(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("test two");
	tc = tcase_create("case");

	tcase_add_test(tc, test_test_two);
	suite_add_tcase(s, tc);

	return s;
}

int run_suite(Suite *s)
{
	int failed_count = 0;
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	failed_count += srunner_ntests_failed(sr);
	srunner_free(sr);
	return failed_count;
}

int main(void)
{
	int failed_count = 0;

	failed_count += run_suite(test_suite());
	failed_count += run_suite(ts_two());

	return (failed_count == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
