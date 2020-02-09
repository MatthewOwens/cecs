#include "cecs.h"
#include <check.h>
#include <stdio.h>
#include "runner.h"
#include "entity_load.h"
#include <cyaml/cyaml.h>

#define ENTITY_COUNT 5

static struct cecs *cecs = NULL;
static struct cecs_entity *manual_entities[ENTITY_COUNT];
static struct cecs_entity *yaml_entities[ENTITY_COUNT];

/*
 * manually grabbed based on the order that components are loaded in
 * components.yml. Correct as of 1dbc6173a, so if you break the manual_ent_load
 * test, check if you've updated the component loading and update accordingly
*/
static int expected_masks [3] = {3, 7 ,4};

void suite_init()
{
	cecs = cecs_init();
	reg_loaded_components(cecs);

	printf("\t%s\n", __FUNCTION__);
}

void suite_teardown()
{
	cecs_free(cecs);
}

void yaml_init()
{
	cecs_load_ent_yaml(cecs, "entities.yml");
}

void yaml_teardown()
{
	for(int i = 0; i < ENTITY_COUNT; ++i){
			cecs_rem_entity(cecs, &yaml_entities[i]);
	}
}

START_TEST(stub)
{
	ck_abort_msg("STUB!\n");
}
END_TEST

START_TEST(yaml_reg_array_lengths)
{
	ck_assert_int_eq(cecs->registered_entities.length, 3);
	ck_assert_int_eq(cecs->registered_entity_names.length, 3);
}
END_TEST

START_TEST(yaml_reg_ent_names)
{
	ck_assert_str_eq(cecs->registered_entity_names.data[0],"actor");
	ck_assert_str_eq(cecs->registered_entity_names.data[1],"named_actor");
	ck_assert_str_eq(cecs->registered_entity_names.data[2],"map_location");
}

Suite * ent_yaml_suite(void)
{
	Suite * s = suite_create("entity yaml suite");
	TCase * yaml_add = tcase_create("yaml add");
	TCase * yaml_registration = tcase_create("yaml registration");

	tcase_add_unchecked_fixture(yaml_add, suite_init, suite_teardown);
	tcase_add_unchecked_fixture(yaml_add, yaml_init, yaml_teardown);

	tcase_add_unchecked_fixture(yaml_registration,
		suite_init, suite_teardown);
	tcase_add_unchecked_fixture(yaml_registration,
		yaml_init, yaml_teardown);

	tcase_add_test(yaml_registration, yaml_reg_array_lengths);
	tcase_add_test(yaml_registration, yaml_reg_ent_names);

	suite_add_tcase(s, yaml_add);
	suite_add_tcase(s, yaml_registration);
	return s;
}
