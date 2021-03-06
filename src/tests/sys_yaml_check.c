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

START_TEST(adds_and_consumes)
{
	struct cecs_system* movement = cecs_system(cecs, "movement");
	struct cecs_system* pathing = cecs_system(cecs, "pathing");

	ck_assert_ptr_nonnull(movement->consumeKeys.data);
	ck_assert_ptr_null(movement->addKeys.data);

	ck_assert_ptr_nonnull(pathing->addKeys.data);
	ck_assert_ptr_null(pathing->consumeKeys.data);

	CECS_COMP_KEY mkey = movement->consumeKeys.data[0];
	CECS_COMP_KEY pkey = pathing->addKeys.data[0];

	printf("movement_s key is %d\n", cecs_component_key(cecs, "movement_s"));
	ck_assert_int_ne( (pkey & cecs_component_key(cecs, "movement_s")), 0);
	ck_assert_int_ne( (mkey & cecs_component_key(cecs, "movement_s")), 0);
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
	tcase_add_test(yaml_load, adds_and_consumes);
	tcase_add_test(yaml_load, movement_load_correct);
	tcase_add_test(yaml_load, movement_runs_correct);
	tcase_add_test(yaml_load, pathing_load_correct);
	tcase_add_test(yaml_load, pathing_runs_correct);

	suite_add_tcase(s, yaml_load);
	return s;
}
