#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "runner.h"
#include "cecs_check.h"
#include "cecs.h"
#include "cecs_component.h"
#include "cecs_entity.h"
#include "cecs_system.h"

static struct cecs *cecs = NULL;

void init_setup()
{
	cecs = cecs_init();
}

void init_teardown()
{
	cecs_free(cecs);
}

START_TEST(cecs_check_init)
{
	ck_assert_ptr_nonnull(cecs);
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

	s = suite_create("cecs suite");
	TCase * tcinit = tcase_create("cecs init");

	tcase_add_checked_fixture(tcinit, init_setup, init_teardown);
	tcase_add_test(tcinit, cecs_check_init);

	suite_add_tcase(s, tcinit);
	return s;
}

