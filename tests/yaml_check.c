#include "cecs.h"
#include <cyaml/cyaml.h>
#include "runner.h"
#include <check.h>

struct position {
	float x;
	float y;
	float z;
};

struct velocity{
	float x;
	float y;
	float z;
};

struct uv{
	float u;
	float v;
};

struct name{
	const char* str;
};

START_TEST(yaml_runner_test)
{
	ck_abort_msg("stub!\n");
}

Suite * yaml_suite(void)
{
	Suite * s;
	TCase * tcinit = tcase_create("yaml init");
	tcase_add_test(tcinit, yaml_runner_test);
	s = suite_create("yaml suite");
	return s;
}
