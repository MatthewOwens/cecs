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
	testPosComponent pos;
	testStrComponent str;
} testCombComponent;

static testPosComponent posComp = {0};
static testStrComponent strComp = {0};
static testCombComponent combComp = {0};

void init_setup()
{
	cecs = cecs_init();
	printf("init_setup\n");
}

void init_teardown()
{
	cecs_free(cecs);
	printf("init_teardown\n");
}

void comp_setup()
{
	cecs_reg_component(cecs, "position", &posComp, sizeof(posComp));
	cecs_reg_component(cecs, "string", &strComp, sizeof(strComp));
	cecs_reg_component(cecs, "combo", &combComp, sizeof(combComp));
}

void comp_teardown()
{
}

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
	uint32_t comboKey = cecs_component_key(cecs, "combo");

	printf("postKey: %zu, strKey: %zu, combo: %zu\n", posKey, strKey, comboKey);

	ck_assert_uint_eq(posKey, 2);
	ck_assert_uint_eq(strKey, 4);
}
END_TEST

START_TEST(cecs_check_add_ent)
{
}
END_TEST

Suite * cecs_suite(void)
{
	Suite *s;

	s = suite_create("cecs suite");
	TCase * tcinit = tcase_create("cecs init");
	TCase * tccomp = tcase_create("cecs components");

	tcase_add_checked_fixture(tcinit, init_setup, init_teardown);
	tcase_add_test(tcinit, cecs_check_init);

	tcase_add_checked_fixture(tccomp, init_setup, init_teardown);
	tcase_add_checked_fixture(tccomp, comp_setup, comp_teardown);
	tcase_add_test(tccomp, cecs_check_add_comp);
	suite_add_tcase(s, tcinit);
	suite_add_tcase(s, tccomp);
	return s;
}

