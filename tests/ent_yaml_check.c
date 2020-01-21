#include "cecs.h"
#include <check.h>
#include <stdio.h>
#include "runner.h"
#include <cyaml/cyaml.h>

#define ENTITY_COUNT 5

static struct cecs *cecs = NULL;
static int manual_entities[ENTITY_COUNT];
static int yaml_entities[ENTITY_COUNT];

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

void yaml_init() //TODO: stub
{
}

void yaml_teardown() //TODO: stub
{
}

void manual_init()
{

	printf("\t%s\n", __FUNCTION__);
	/*
	 * Adding 'actors', 'named_actors' and one 'map_location' manually.
	 * We'll use these to check cecs_add_entity_v() and then as a base of
	 * comparison later on for the yaml_entities
	*/
	for(int i = 0; i < ENTITY_COUNT - 1; ++i) {
		if(i%2 == 0) {
			cecs_add_entity_v(cecs, &manual_entities[i], 3,
					"position", "velocity", "uvs");
		} else {
			cecs_add_entity_v(cecs, &manual_entities[i], 4,
					"position", "velocity", "name", "uvs");
		}
	}

	cecs_add_entity_v(cecs, &manual_entities[ENTITY_COUNT - 1], 2,
			"position", "name");
}

void manual_teardown()
{
	for(int i = 0; i < ENTITY_COUNT; ++i){
			cecs_rem_entity(cecs, &manual_entities[i]);
	}
}

START_TEST(stub)
{
	ck_abort_msg("STUB!\n");
}
END_TEST

START_TEST(manual_ent_load)
{
	ck_abort_msg("STUB!\n");
}
END_TEST

START_TEST(yaml_ent_load)
{
	ck_abort_msg("STUB!\n");
}
END_TEST

START_TEST(comparison)
{
	ck_abort_msg("STUB!\n");
}
END_TEST

Suite * ent_yaml_suite(void)
{
	Suite * s = suite_create("entity yaml suite");
	TCase * load_case = tcase_create("entity load");

	tcase_add_unchecked_fixture(load_case, suite_init, suite_teardown);
	tcase_add_unchecked_fixture(load_case, manual_init, manual_teardown);
	tcase_add_unchecked_fixture(load_case, yaml_init, yaml_teardown);
	tcase_add_test(load_case, manual_ent_load);
	tcase_add_test(load_case, yaml_ent_load);
	tcase_add_test(load_case, comparison);

	suite_add_tcase(s, load_case);
	return s;
}
