#include "check_initilised.h"
#include <check.h>
#include "cecs.h"
#include "runner.h"
#include "entity_load.h"
#include "system_load.h"
#include "components.h"
#include "cecs_err.h"

static struct cecs* cecs = NULL;

static void init()
{
	cecs = cecs_init();
	reg_loaded_components(cecs);
	cecs_load_ent_yaml(cecs, "entities.yml");
	cecs_load_sys_yaml(cecs, "systems.yml");
}

static void teardown()
{
	cecs_free(cecs);
}

START_TEST(init_successful)
{
	ck_assert_int_eq(cecs->state, CECS_INITILISED);
}
END_TEST

START_TEST(start_successful)
{
	ck_assert_int_eq(cecs_start(cecs), CECS_NONE);
	ck_assert_int_eq(cecs->state, CECS_STARTED);
}
END_TEST

Suite * test_initilised(void)
{
	Suite * s = suite_create("cecs completed suite");
	TCase * tc = tcase_create("successful init");

	tcase_add_unchecked_fixture(tc, init, teardown);
	tcase_add_test(tc, init_successful);
	tcase_add_test(tc, start_successful);

	suite_add_tcase(s, tc);
	return s;
}
