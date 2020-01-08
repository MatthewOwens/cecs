#include "cecs.h"
#include <cyaml/cyaml.h>
#include "runner.h"
#include <check.h>

typedef struct {
	float x;
	float y;
	float z;
} s_position;

typedef struct {
	float x;
	float y;
	float z;
}s_velocity;

typedef struct {
	float u;
	float v;
}s_uv;

typedef struct {
	const char* str;
}s_name;

static s_position pos = {0};
static s_velocity vel = {0};
static s_uv uv = {0};
static s_name name = {0};

void yaml_init()	// TODO: stub
{
}

void yaml_teardown()	// TODO: stub
{
}

START_TEST(position_loaded)
{
	ck_assert_float_eq(pos.x, 5.f);
	ck_assert_float_eq(pos.y, 5.f);
	ck_assert_float_eq(pos.z, 5.f);
}
END_TEST

START_TEST(velocity_loaded)
{
	ck_assert_float_eq(vel.x, 3.0);
	ck_assert_float_eq(vel.y, 0.0);
	ck_assert_float_eq(vel.z, 0.0);
}
END_TEST

START_TEST(uv_loaded)
{
	ck_assert_float_eq(uv.u, 1.0);
	ck_assert_float_eq(uv.v, 1.0);
}
END_TEST

START_TEST(name_loaded)
{
	ck_assert_str_eq(name.str, "test");
}
END_TEST

Suite * comp_yaml_suite(void)
{
	Suite * s;
	TCase * load_case = tcase_create("component load");
	s = suite_create("component yaml suite");

	tcase_add_unchecked_fixture(load_case, yaml_init, yaml_teardown);
	tcase_add_test(load_case, position_loaded);
	tcase_add_test(load_case, velocity_loaded);
	tcase_add_test(load_case, uv_loaded);
	tcase_add_test(load_case, name_loaded);
	suite_add_tcase(s, load_case);
	return s;
}
