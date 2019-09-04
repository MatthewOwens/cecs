#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "runner.h"
#include "arr.h"

static array(int) arr = array_init();

START_TEST(arr_init)
{
	ck_assert_int_eq(arr.capacity, 0);
	ck_assert_int_eq(arr.length, 0);
	ck_assert_ptr_null(arr.data);
}
END_TEST

START_TEST(arr_push)
{
	int itr_max = 6;

	for(int i = 0; i < itr_max; ++i){
		array_push(arr, i);
	}

	ck_assert_ptr_nonnull(arr.data);

	for(int i = 0; i < itr_max; ++i){
		ck_assert_int_eq(arr.data[i], i);
	}

	ck_assert_int_eq(arr.length, itr_max);
	ck_assert_int_eq(arr.capacity, 8);
}
END_TEST


Suite * arr_suite(void)
{
	Suite *s;
	TCase *tc;
	s = suite_create("array suite");
	tc = tcase_create("case");

	tcase_add_test(tc, arr_init);
	tcase_add_test(tc, arr_push);
	suite_add_tcase(s, tc);
	return s;
}
