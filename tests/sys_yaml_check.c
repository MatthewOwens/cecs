#include "sys_yaml_check.h"
#include <check.h>
#include "cecs.h"
#include <stdio.h>
#include "runner.h"
#include "entity_load.h"
#include "components.h"

START_TEST(stub)
{
	ck_abort_msg("STUB!\n");
}
END_TEST

Suite * sys_yaml_suite(void)
{
	Suite * s = suite_create("system yaml suite");
	TCase * yaml_load = tcase_create("yaml load");

	tcase_add_test(yaml_load, stub);

	suite_add_tcase(s, yaml_load);
	return s;
}
