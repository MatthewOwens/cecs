#include "sys_yaml_check.h"
#include <check.h>
#include "cecs.h"
#include <stdio.h>
#include "runner.h"
#include "entity_load.h"
#include "components.h"

#define ENTITY_COUNT 5

static struct cecs *cecs = NULL;
static struct cecs_entity *entities[ENTITY_COUNT];

static void ent_init()
{
	cecs_load_ent_yaml(cecs, "entities.yml");
	cecs_add_entity(cecs, "actor", &entities[0]);
	cecs_add_entity(cecs, "named_actor", &entities[1]);
	cecs_add_entity(cecs, "map_location", &entities[2]);
	cecs_add_entity(cecs, "actor", &entities[3]);
	cecs_add_entity(cecs, "map_location", &entities[4]);
}

static void ent_teardown()
{
	for(int i = 0; i < ENTITY_COUNT; ++i){
			cecs_rem_entity(cecs, &entities[i]);
	}
}

static void suite_init()
{
	cecs = cecs_init();
	reg_loaded_components(cecs);
	ent_init();
}

static void suite_teardown()
{
	ent_teardown();
	cecs_free(cecs);
}

static void yaml_init()
{
	cecs_load_sys_yaml(cecs, "systems.yml");
}

static void yaml_teardown()// TODO: stub
{
}

START_TEST(systems_added)
{
	// system names are defined in yaml
	ck_assert_ptr_nonnull(cecs_system(cecs, "movement"));
	ck_assert_ptr_nonnull(cecs_system(cecs, "pathing"));
}
END_TEST

START_TEST(systems_array_size)
{
	// only 2 defined in yaml
	ck_assert_int_eq(cecs->num_systems, 2);

	/*
	 * since we're doubling the size of cecs->systems when we need to
	 * realloc, two should be good, and the third indice should be null
	 */ 
	ck_assert_ptr_nonnull(&cecs->systems[0]);
	ck_assert_ptr_nonnull(&cecs->systems[1]);
	ck_assert_ptr_null(&cecs->systems[2]);
}
END_TEST

START_TEST(movement_load_correct)
{
	ck_assert_ptr_nonnull(cecs_system(cecs, "movement")->init);
	ck_assert_ptr_nonnull(cecs_system(cecs, "movement")->run);
	ck_assert_ptr_nonnull(cecs_system(cecs, "movement")->free);

	ck_abort_msg("STUB!\n");
}
END_TEST

START_TEST(movement_runs_correct)
{
	cecs_system(cecs, "movement")->init(cecs);
	cecs_system(cecs, "movement")->run();
	cecs_system(cecs, "movement")->free();

	ck_abort_msg("STUB!\n");
}
END_TEST

Suite * sys_yaml_suite(void)
{
	Suite * s = suite_create("system yaml suite");
	TCase * yaml_load = tcase_create("yaml load");

	tcase_add_unchecked_fixture(yaml_load, suite_init, suite_teardown);
	tcase_add_unchecked_fixture(yaml_load, yaml_init, yaml_teardown);

	tcase_add_test(yaml_load, systems_array_size);
	tcase_add_test(yaml_load, systems_added);
	tcase_add_test(yaml_load, movement_load_correct);
	tcase_add_test(yaml_load, movement_runs_correct);

	suite_add_tcase(s, yaml_load);
	return s;
}
