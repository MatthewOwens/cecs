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

START_TEST(yaml_position)
{
	s_position pos = {0};
	s_velocity vel = {0};
	s_uv uv = {0};
	s_name name = {0};

	//TODO: call component load func
	
	ck_assert_float_eq(pos.x, 5.f);
	ck_assert_float_eq(pos.y, 5.f);
	ck_assert_float_eq(pos.z, 5.f);

	ck_assert_float_eq(vel.x, 3.0);
	ck_assert_float_eq(vel.y, 0.0);
	ck_assert_float_eq(vel.z, 0.0);

	ck_assert_float_eq(uv.u, 1.0);
	ck_assert_float_eq(uv.v, 1.0);

	ck_assert_ptr_null(name.str);
}
END_TEST

Suite * yaml_suite(void)
{
	Suite * s;
	TCase * tc = tcase_create("yaml init");
	s = suite_create("yaml suite");

	tcase_add_test(tc, yaml_position);
	suite_add_tcase(s, tc);
	return s;
}
