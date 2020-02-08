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

void load_check(int check_start)
{
	ck_assert_int_eq(cecs->num_entities, ENTITY_COUNT + check_start);
	ck_assert_uint_ge(sizeof(cecs->entities) * cecs->num_entities,
			sizeof(cecs->entities) * (ENTITY_COUNT + check_start));

	for(int i = check_start; i < cecs->num_entities; ++i) {
		ck_assert_int_eq(cecs->entities[i].id, i);
	}

	ck_assert_int_eq(cecs->entities[check_start+0].mask,expected_masks[0]);
	ck_assert_int_eq(cecs->entities[check_start+1].mask,expected_masks[1]);
	ck_assert_int_eq(cecs->entities[check_start+2].mask,expected_masks[0]);
	ck_assert_int_eq(cecs->entities[check_start+3].mask,expected_masks[1]);
	ck_assert_int_eq(cecs->entities[check_start+4].mask,expected_masks[2]);
}

START_TEST(stub)
{
	ck_abort_msg("STUB!\n");
}
END_TEST

START_TEST(manual_ent_load)
{
	load_check(0);
}
END_TEST

START_TEST(yaml_ent_load)
{
	load_check(5);
}
END_TEST

START_TEST(yaml_ent_registration)
{
	ck_assert_int_eq(cecs->registered_entities.length, 3);
	ck_assert_int_eq(cecs->registered_entity_names.length, 3);

	ck_assert_str_eq(cecs->registered_entity_names.data[0],"actor");
	ck_assert_str_eq(cecs->registered_entity_names.data[1],"named_actor");
	ck_assert_str_eq(cecs->registered_entity_names.data[2],"map_location");
}

START_TEST(comparison)
{
	/* 
	 * double checking here, so we don't read garbage and go
	 * down a rabbit hole
	 */
	ck_assert_int_eq(cecs->num_entities, ENTITY_COUNT * 2);

	/*
	 * checking that the entities loaded wiuth manual_init() and
	 * cecs_load_comp_yaml are identical
	 */
	for(int i = 0; i < ENTITY_COUNT; ++i) {
		ck_assert_int_eq(cecs->entities[i].id,
				cecs->entities[i + ENTITY_COUNT].id);
		ck_assert_int_eq(cecs->entities[i].mask,
				cecs->entities[i+ENTITY_COUNT].mask);
	}
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
