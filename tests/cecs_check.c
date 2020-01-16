#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "runner.h"
#include "cecs_check.h"
#include "cecs.h"
#include "cecs_component.h"
#include "cecs_entity.h"
#include "cecs_system.h"

static struct cecs *cecs = NULL;
static int enta;
static int entb;

typedef struct {
	float x;
	float y;
	float z;
} testPosComponent;

typedef struct {
	char *str;
} testStrComponent;

typedef struct {
	float u;
	float v;
} testUVComponent;

static testPosComponent posComp = {0};
static testStrComponent strComp = {0};
static testUVComponent uvComp = {0};

void init_setup()
{
	cecs = cecs_init();
}

void init_teardown()
{
	cecs_free(cecs);
}

void comp_setup()
{
	cecs_reg_component(cecs, "position", &posComp, sizeof(posComp));
	cecs_reg_component(cecs, "string", &strComp, sizeof(strComp));
	cecs_reg_component(cecs, "uv", &uvComp, sizeof(uvComp));
}

void comp_teardown()
{
}

void ent_setup()
{
	cecs_add_entity(cecs, &enta);
	cecs_ent_add_component(cecs, enta, "position");
	cecs_ent_add_component(cecs, enta, "string");

	cecs_add_entity(cecs, &entb);
	cecs_ent_add_component(cecs, entb, "uv");
	cecs_ent_add_component(cecs, entb, "position");
}

void ent_teardown()
{
	cecs_rem_entity(cecs, &enta);
	cecs_rem_entity(cecs, &entb);
}

int tsys_init(struct cecs* cecs)
{
}

int tsys_free(struct cecs* cecs)
{
}

void tsys_run()
{
	//  getting the system
	struct cecs_system* sys = cecs_system(cecs, "test-sys");

	testPosComponent* positions = cecs_component(cecs, "position")->data;
	testUVComponent* uvs = cecs_component(cecs, "uv")->data;

	for(int i = 0; i < cecs->num_entities; ++i){
		// if i is in free_entities, continue
		if(cecs->free_entities.length != 0){
			for(int j = 0; j < cecs->free_entities.length; ++i){
				if(i == cecs->free_entities.data[i])
					continue;
			}
		}

		// Skipping if the entity contains an excluded component
		if((cecs->entities[i].mask & sys->exclusion_mask) != 0)
			continue;


		// Actual data transformation
		positions[i].x += 1.f;
		positions[i].y += 1.5f;
		positions[i].z += 2.f;

		uvs[i].u = 5.f;
		uvs[i].v = 5.f;
	}
}

void sys_setup()
{
	cecs_reg_system(cecs, "test-sys");
	cecs_sys_set_incl(cecs, "test-sys",
			  cecs_component_key(cecs, "position") |
			  cecs_component_key(cecs, "uv"));
	cecs_sys_set_excl(cecs, "test-sys",
			  cecs_component_key(cecs, "string"));
	cecs_sys_set_funcs(cecs, "test-sys", tsys_init, tsys_run, tsys_free);
}

void sys_teardown()
{
	cecs_rem_system(cecs, "test-sys");
}

void stub() { ck_abort_msg("STUB - test not implemented\n"); }

START_TEST(cecs_check_init)
{
	ck_assert_ptr_nonnull(cecs);
	ck_assert_int_eq(cecs->num_components, 0);
	ck_assert_int_eq(cecs->num_entities, 0);
	ck_assert_int_eq(cecs->num_systems, 0);

	ck_assert_ptr_null(cecs->components);
	ck_assert_ptr_null(cecs->entities);
	ck_assert_ptr_null(cecs->systems);
}
END_TEST

START_TEST(cecs_check_add_comp)
{
	ck_assert_int_eq(cecs->num_components, 3);
	ck_assert_ptr_nonnull(cecs->components);

	uint32_t posKey = cecs_component_key(cecs, "position");
	uint32_t strKey = cecs_component_key(cecs, "string");
	uint32_t uvKey = cecs_component_key(cecs, "uv");

	ck_assert_uint_eq(posKey, 0);
	ck_assert_uint_eq(strKey, 1);
	ck_assert_uint_eq(uvKey, 2);
}
END_TEST

START_TEST(cecs_check_add_ent)
{
	ck_assert_int_eq(cecs->num_entities, 2);
	ck_assert_uint_ge(sizeof(cecs->entities) * cecs->num_entities,
					  sizeof(cecs->entities) * 2);
}
END_TEST

START_TEST(cecs_check_add_sys)
{
	struct cecs_system* sys = cecs_system(cecs, "test-sys");
	ck_assert_int_eq(cecs->num_systems, 1);
	ck_assert_ptr_nonnull(cecs->systems);
	ck_assert_int_eq(sys->inclusion_mask, cecs_component_key(cecs, "position") |
										  cecs_component_key(cecs, "uv") );

	ck_assert_uint_eq(sys->exclusion_mask, cecs_component_key(cecs, "string"));
}
END_TEST

START_TEST(cecs_check_system_func)
{
	ck_assert_ptr_nonnull(cecs_system(cecs, "test-sys"));
	ck_assert_ptr_null(cecs_system(NULL, "test-sys"));
	ck_assert_ptr_null(cecs_system(cecs, "not a real system"));
	ck_assert_ptr_null(cecs_system(NULL, NULL));
}
END_TEST

void check_ent_untouched(testPosComponent* pos, testUVComponent* uv)
{
	ck_assert_float_eq(pos->x, 0.f);
	ck_assert_float_eq(pos->y, 0.f);
	ck_assert_float_eq(pos->z, 0.f);
	ck_assert_float_eq(uv->u, 0.f);
	ck_assert_float_eq(uv->v, 0.f);
}

START_TEST(cecs_check_system_run)
{
	struct cecs_system* sys = cecs_system(cecs, "test-sys");
	testPosComponent* positions = cecs_component(cecs, "position")->data;
	testStrComponent* strs = cecs_component(cecs, "string")->data;
	testUVComponent* uvs = cecs_component(cecs, "uv")->data;

	sys->run();

	for(int i = 0; i < cecs->num_entities; ++i){
		// if i is in free_entities, should have been ignored
		if(cecs->free_entities.length != 0){
			for(int j = 0; j < cecs->free_entities.length; ++i){
				if(i == cecs->free_entities.data[i])
					check_ent_untouched(&positions[i],&uvs[i]);
			}
		}

		/*
		 * A component in this ent has been specifically excluded, it
		 * should have been ignored.
		 *
		 * bitwise & will return 0 only if no bit in the ent mask matches
		 * the exclusion mask
		*/
		if((cecs->entities[i].mask & sys->exclusion_mask) != 0) {
			check_ent_untouched(&positions[i],&uvs[i]);
		/*
		 * Entity contains both required components for system to be
		 * interested, values should have been set.
		*/
		} else if ((cecs->entities[i].mask & sys->inclusion_mask) ==
			  sys->inclusion_mask){
			/*
			 * Posiitions were incremented with '+=', but the sys
			 * has only been run once
			*/
			ck_assert_float_eq(positions[i].x, 1.f);
			ck_assert_float_eq(positions[i].y, 1.5f);
			ck_assert_float_eq(positions[i].z, 2.f);

			// UVs were set, with '=' so should be 5.f
			ck_assert_float_eq(uvs[i].u, 5.f);
			ck_assert_float_eq(uvs[i].v, 5.f);
		}

		/*
		 * test sys doesn't care about strings, so they should always
		 * be NULL
		*/
		printf("strs[%d].str is %s\n", i, strs[i].str);
		ck_assert_ptr_null(strs[i].str);
	}
}
END_TEST

START_TEST(cecs_check_system_rem)
{
	cecs_rem_system(cecs, "test-sys");
	ck_assert_ptr_null(cecs_system(cecs, "test-sys"));
}

Suite * cecs_suite(void)
{
	Suite *s;

	s = suite_create("cecs suite");
	TCase * tcinit = tcase_create("cecs init");
	TCase * tccomp = tcase_create("cecs add components");
	TCase * tcent = tcase_create("cecs add entities");
	TCase * tcsys = tcase_create("cecs add systems");

	tcase_add_checked_fixture(tcinit, init_setup, init_teardown);
	tcase_add_test(tcinit, cecs_check_init);

	tcase_add_unchecked_fixture(tccomp, init_setup, init_teardown);
	tcase_add_checked_fixture(tccomp, comp_setup, comp_teardown);
	tcase_add_test(tccomp, cecs_check_add_comp);

	tcase_add_unchecked_fixture(tcent, init_setup, init_teardown);
	tcase_add_unchecked_fixture(tcent, comp_setup, comp_teardown);
	tcase_add_checked_fixture(tcent, ent_setup, ent_teardown);
	tcase_add_test(tcent, cecs_check_add_ent);

	tcase_add_unchecked_fixture(tcsys, init_setup, init_teardown);
	tcase_add_unchecked_fixture(tcsys, comp_setup, comp_teardown);
	tcase_add_unchecked_fixture(tcsys, ent_setup, ent_teardown);
	tcase_add_unchecked_fixture(tcsys, sys_setup, sys_teardown);
	tcase_add_test(tcsys, cecs_check_system_func);
	tcase_add_test(tcsys, cecs_check_add_sys);
	tcase_add_test(tcsys, cecs_check_system_run);
	tcase_add_test(tcsys, cecs_check_system_rem);

	suite_add_tcase(s, tcinit);
	suite_add_tcase(s, tccomp);
	suite_add_tcase(s, tcent);
	suite_add_tcase(s, tcsys);
	return s;
}

