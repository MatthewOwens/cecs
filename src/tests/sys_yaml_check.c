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

static void yaml_teardown()
{
	cecs_free_sys_libs();
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
}
END_TEST

START_TEST(movement_load_correct)
{
	ck_assert_ptr_nonnull(cecs_system(cecs, "movement")->init);
	ck_assert_ptr_nonnull(cecs_system(cecs, "movement")->run);
	ck_assert_ptr_nonnull(cecs_system(cecs, "movement")->free);
}
END_TEST

START_TEST(movement_runs_correct)
{
	cecs_system(cecs, "movement")->init(cecs);
	cecs_system(cecs, "movement")->run();
	cecs_system(cecs, "movement")->free();
}
END_TEST

START_TEST(pathing_load_correct)
{
	ck_assert_ptr_nonnull(cecs_system(cecs, "pathing")->init);
	ck_assert_ptr_nonnull(cecs_system(cecs, "pathing")->run);
	ck_assert_ptr_nonnull(cecs_system(cecs, "pathing")->free);
}
END_TEST

START_TEST(pathing_runs_correct)
{
	cecs_system(cecs, "pathing")->init(cecs);
	cecs_system(cecs, "pathing")->run();
	cecs_system(cecs, "pathing")->free();
}
END_TEST

START_TEST(system_depends)
{
	struct cecs_system* movement = cecs_system(cecs, "movement");

	ck_assert_ptr_nonnull(movement->dependNames.data);
	ck_assert_ptr_null(movement->dependIndices.data);
	ck_assert_int_eq(strcmp(movement->dependNames.data[0], "pathing"), 0);

	cecs_resolve_sys_deps(cecs);
	ck_assert_ptr_null(movement->dependNames.data);
	ck_assert_ptr_nonnull(movement->dependIndices.data);

	// finding the itr
	int itr = -1;

	for(int i = 0; i < cecs->num_systems; ++i) {
		if(strcmp(cecs->systems[i].name, "pathing") == 0){
			itr = i;
		}
	}
	ck_assert_int_eq(movement->dependIndices.data[0], itr);
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
	tcase_add_test(yaml_load, system_depends);
	tcase_add_test(yaml_load, movement_load_correct);
	tcase_add_test(yaml_load, movement_runs_correct);
	tcase_add_test(yaml_load, pathing_load_correct);
	tcase_add_test(yaml_load, pathing_runs_correct);

	suite_add_tcase(s, yaml_load);
	return s;
}
