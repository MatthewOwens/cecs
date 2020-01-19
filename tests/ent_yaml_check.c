#include "cecs.h"
#include <check.h>
#include <stdio.h>
#include "runner.h"
#include <cyaml/cyaml.h>

static struct cecs *cecs = NULL;

void ent_yaml_init()	//TODO: stub
{
}

void ent_yaml_teardown()//TODO: stub
{
}

START_TEST(stub)
{
	ck_abort_msg("STUB!\n");
}
END_TEST

Suite * ent_yaml_suite(void)
{
	Suite * s;
	TCase * load_case = tcase_create("entity load");
	s = suite_create("entity yaml suite");

	tcase_add_unchecked_fixture(load_case, ent_yaml_init, ent_yaml_teardown);
	tcase_add_test(load_case, stub);
	suite_add_tcase(s, load_case);
	return s;
}
