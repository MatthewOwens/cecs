#include "cecs.h"
#include "runner.h"
#include "components.h"
#include <check.h>
#include <stdio.h>

static struct cecs *cecs = NULL;

void comp_yaml_init()
{
	cecs = cecs_init();
	reg_loaded_components(cecs);
}

void comp_yaml_teardown()
{
	cecs_free(cecs);
}

START_TEST(loaded_count_correct)
{
	ck_assert_int_eq(cecs->num_components, 5);
	ck_assert_ptr_nonnull(cecs->components);
}
END_TEST

START_TEST(position_loaded)
{
	uint32_t key = cecs_component_key(cecs, "position");
	ck_assert_uint_eq(key, 0);
}
END_TEST

START_TEST(velocity_loaded)
{
	uint32_t key = cecs_component_key(cecs, "velocity");
	ck_assert_uint_eq(key, 1);
}
END_TEST

START_TEST(uv_loaded)
{
	uint32_t key = cecs_component_key(cecs, "uvs");
	ck_assert_uint_eq(key, 2);
}
END_TEST

START_TEST(stats_loaded)
{
	uint32_t key = cecs_component_key(cecs, "stats");
	ck_assert_uint_eq(key, 3);
}

START_TEST(name_loaded)
{
	uint32_t key = cecs_component_key(cecs, "name");
	ck_assert_uint_eq(key, 4);
}
END_TEST

Suite * comp_yaml_suite(void)
{
	Suite * s;
	TCase * load_case = tcase_create("component load");
	s = suite_create("component yaml suite");

	tcase_add_unchecked_fixture(load_case, comp_yaml_init, comp_yaml_teardown);
	tcase_add_test(load_case, loaded_count_correct);
	tcase_add_test(load_case, position_loaded);
	tcase_add_test(load_case, velocity_loaded);
	tcase_add_test(load_case, uv_loaded);
	tcase_add_test(load_case, stats_loaded);
	tcase_add_test(load_case, name_loaded);
	suite_add_tcase(s, load_case);
	return s;
}
