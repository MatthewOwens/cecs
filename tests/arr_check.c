#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "runner.h"
#include "arr.h"

static array(int) arr = array_init();
static const int itr_max = 6;


START_TEST(arr_init)
{
	ck_assert_int_eq(arr.capacity, 0);
	ck_assert_int_eq(arr.length, 0);
	ck_assert_ptr_null(arr.data);
}
END_TEST

START_TEST(arr_push)
{
	for(int i = 0; i < itr_max; ++i){
		array_push(arr, i);
	}

	ck_assert_ptr_nonnull(arr.data);

	for(int i = 0; i < itr_max; ++i){
		ck_assert_int_eq(arr.data[i], i);
	}

	ck_assert_int_eq(arr.length, itr_max);
	ck_assert_int_eq(arr.capacity, 8);
	printf("len: %zu, cap: %zu, itr_max: %d\n", arr.length, arr.capacity, itr_max);
}
END_TEST

START_TEST(arr_pop)
{
	array_pop(arr);
	printf("len: %zu, cap: %zu, itr_max: %d\n", arr.length, arr.capacity, itr_max);
	ck_assert_int_eq(arr.length, itr_max - 1);
	ck_assert_int_eq(arr.capacity, 8);
	ck_assert_int_eq(arr.data[arr.length], itr_max - 1);
}
END_TEST

START_TEST(arr_pop_at)
{
	array_pop_at(arr, 3);
	// -2 since we've popped already
	ck_assert_int_eq(arr.length, itr_max - 2);
	ck_assert_int_eq(arr.capacity, 8);
	ck_assert_int_eq(arr.data[3], 4);
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
	tcase_add_test(tc, arr_pop);
	tcase_add_test(tc, arr_pop_at);
	suite_add_tcase(s, tc);
	return s;
}
