#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "runner.h"
#include "cecs_check.h"
#include "cecs.h"

static struct cecs *cecs = NULL;

START_TEST(cecs_check_init)
{
	cecs = cecs_init();
	ck_assert_int_eq(cecs->num_components, 0);
	ck_assert_int_eq(cecs->num_entities, 0);
	ck_assert_int_eq(cecs->num_systems, 0);

	ck_assert_ptr_null(cecs->components);
	ck_assert_ptr_null(cecs->entities);
	ck_assert_ptr_null(cecs->systems);
}
END_TEST

Suite * cecs_suite(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("cecs suite");
	tc = tcase_create("cecs case");

	tcase_add_test(tc, cecs_check_init);
	suite_add_tcase(s, tc);
	return s;
}
