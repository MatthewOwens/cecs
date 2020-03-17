#include "cecs.h"
#include <check.h>
#include <stdio.h>
#include "runner.h"
#include "entity_load.h"
#include "components.h"

#define ENTITY_COUNT 5

static struct cecs *cecs = NULL;
static struct cecs_entity *entities[ENTITY_COUNT];

/*
 * manually grabbed based on the order that components are loaded in
 * components.yml. Correct as of 1dbc6173a, so if you break the manual_ent_load
 * test, check if you've updated the component loading and update accordingly
*/
static int expected_masks [3] = {3, 7 ,4};

static void suite_init()
{
	cecs = cecs_init();
	reg_loaded_components(cecs);

	printf("\t%s\n", __FUNCTION__);
}

static void suite_teardown()
{
	cecs_free(cecs);
}

static void yaml_init()
{
	cecs_load_ent_yaml(cecs, "entities.yml");
	cecs_add_entity(cecs, "actor", &entities[0]);
	cecs_add_entity(cecs, "named_actor", &entities[1]);
	cecs_add_entity(cecs, "map_location", &entities[2]);
	cecs_add_entity(cecs, "actor", &entities[3]);
	cecs_add_entity(cecs, "map_location", &entities[4]);
}

static void yaml_teardown()
{
	for(int i = 0; i < ENTITY_COUNT; ++i){
			cecs_rem_entity(cecs, &entities[i]);
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
END_TEST

START_TEST(yaml_reg_ent_masks)
{
	ck_assert_int_eq(cecs->registered_entities.data[0].mask,
		expected_masks[0]);
	ck_assert_int_eq(cecs->registered_entities.data[1].mask,
		expected_masks[1]);
	ck_assert_int_eq(cecs->registered_entities.data[2].mask,
		expected_masks[2]);
}
END_TEST

START_TEST(yaml_added_ent_count)
{
	ck_assert_int_eq(cecs->num_entities, ENTITY_COUNT);
}
END_TEST

START_TEST(yaml_added_ent_masks)
{
	if(cecs->num_entities != ENTITY_COUNT){
		ck_abort_msg("aborting, entities not added");
	}
	ck_assert_int_eq(cecs->entities[0].mask, expected_masks[0]);
	ck_assert_int_eq(cecs->entities[1].mask, expected_masks[1]);
	ck_assert_int_eq(cecs->entities[2].mask, expected_masks[2]);
	ck_assert_int_eq(cecs->entities[3].mask, expected_masks[0]);
	ck_assert_int_eq(cecs->entities[4].mask, expected_masks[2]);
}
END_TEST

START_TEST(yaml_added_ent_uids)
{
	if(cecs->num_entities != ENTITY_COUNT){
		ck_abort_msg("aborting, entities not added");
	}
	int known_ids[ENTITY_COUNT] = { -1 };

	for(int i = 0; i < ENTITY_COUNT; ++i){
		for(int j = 0; j < i; ++j){
			ck_assert_int_ne(cecs->entities[i].id, known_ids[j]);
		}
		known_ids[i] = cecs->entities[i].id;
	}
}
END_TEST

Suite * ent_yaml_suite(void)
{
	Suite * s = suite_create("entity yaml suite");
	TCase * yaml_load = tcase_create("yaml load");

	tcase_add_unchecked_fixture(yaml_load, suite_init, suite_teardown);
	tcase_add_unchecked_fixture(yaml_load, yaml_init, yaml_teardown);

	tcase_add_test(yaml_load, yaml_reg_array_lengths);
	tcase_add_test(yaml_load, yaml_reg_ent_names);
	tcase_add_test(yaml_load, yaml_reg_ent_masks);
	tcase_add_test(yaml_load, yaml_added_ent_count);
	tcase_add_test(yaml_load, yaml_added_ent_masks);
	tcase_add_test(yaml_load, yaml_added_ent_uids);

	suite_add_tcase(s, yaml_load);
	return s;
}
