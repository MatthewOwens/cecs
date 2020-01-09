#include "cecs.h"
#include <cyaml/cyaml.h>
#include "runner.h"
#include <check.h>
#include <stdio.h>

typedef struct {
	float x;
	float y;
	float z;
} s_position;

typedef struct {
	float x;
	float y;
	float z;
}s_velocity;

typedef struct {
	float u;
	float v;
}s_uv;

typedef struct {
	char* str;
}s_name;

static s_position pos = {0};
static s_velocity vel = {0};
static s_uv uv = {0};
static s_name name = {0};

void yaml_init()
{
	/* ********************
	   velocity
	 * ********************/
	// defining _how_ to map yaml fields to structs
	static const cyaml_schema_field_t vel_fields_schema[] = {
		CYAML_FIELD_FLOAT("x", CYAML_FLAG_DEFAULT, s_velocity, x),
		CYAML_FIELD_FLOAT("y", CYAML_FLAG_DEFAULT, s_velocity, y),
		CYAML_FIELD_FLOAT("z", CYAML_FLAG_DEFAULT, s_velocity, z),
		CYAML_FIELD_END	// NULL value to signal end of array
	};
	static const cyaml_schema_value_t s_vel_schema = {
		CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, s_velocity, vel_fields_schema),
	};

	/* ********************
	   position
	 * ********************/
	static const cyaml_schema_field_t pos_fields_schema[] = {
		CYAML_FIELD_FLOAT("x", CYAML_FLAG_DEFAULT, s_position, x),
		CYAML_FIELD_FLOAT("y", CYAML_FLAG_DEFAULT, s_position, y),
		CYAML_FIELD_FLOAT("z", CYAML_FLAG_DEFAULT, s_position, z),
		CYAML_FIELD_END
	};
	static const cyaml_schema_value_t s_pos_schema = {
		CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, s_position, pos_fields_schema),
	};

	/* ********************
	   uv
	 * ********************/
	static const cyaml_schema_field_t uv_fields_schema[] = {
		CYAML_FIELD_FLOAT("u", CYAML_FLAG_DEFAULT, s_uv, u),
		CYAML_FIELD_FLOAT("v", CYAML_FLAG_DEFAULT, s_uv, v),
		CYAML_FIELD_END
	};
	static const cyaml_schema_value_t s_uv_schema = {
		CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, s_uv, uv_fields_schema),
	};

	/* ********************
	   name
	 * ********************/
	static const cyaml_schema_field_t name_fields_schema[] = {
		CYAML_FIELD_STRING_PTR(
				"name", CYAML_FLAG_POINTER, s_name, str, 0, CYAML_UNLIMITED),
		CYAML_FIELD_END
	};

	// mapping the yaml fields to struct with the schema
	static const cyaml_schema_value_t component_schema = {
		CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, s_velocity, vel_fields_schema),
		//CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, s_position, pos_fields_schema),
		//CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, s_uv, uv_fields_schema),
		//CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, s_name, name_fields_schema),
		//CYAML_FIELD_END
	};

	/* ********************
	   actual config loading
	 * ********************/
	static const cyaml_config_t config = {
		.log_level = CYAML_LOG_WARNING, /* Logging errors and warnings only. */
		.log_fn = cyaml_log,            /* Use the default logging function. */
		.mem_fn = cyaml_mem,            /* Use the default memory allocator. */
	};

	/* Load input file. */
	// TODO: can only load one at a time?
	cyaml_err_t err = cyaml_load_file("components.yml", &config,
			&vel_fields_schema, (void **) &vel, NULL);
	if (err != CYAML_OK) {
		fprintf(stderr, "ERROR: %s\n", cyaml_strerror(err));
	}
}

void yaml_teardown()	// TODO: stub
{
}

START_TEST(position_loaded)
{
	ck_assert_float_eq(pos.x, 5.f);
	ck_assert_float_eq(pos.y, 5.f);
	ck_assert_float_eq(pos.z, 5.f);
}
END_TEST

START_TEST(velocity_loaded)
{
	ck_assert_float_eq(vel.x, 3.0);
	ck_assert_float_eq(vel.y, 0.0);
	ck_assert_float_eq(vel.z, 0.0);
}
END_TEST

START_TEST(uv_loaded)
{
	ck_assert_float_eq(uv.u, 1.0);
	ck_assert_float_eq(uv.v, 1.0);
}
END_TEST

START_TEST(name_loaded)
{
	ck_assert_str_eq(name.str, "test");
}
END_TEST

Suite * comp_yaml_suite(void)
{
	Suite * s;
	TCase * load_case = tcase_create("component load");
	s = suite_create("component yaml suite");

	tcase_add_unchecked_fixture(load_case, yaml_init, yaml_teardown);
	tcase_add_test(load_case, position_loaded);
	tcase_add_test(load_case, velocity_loaded);
	tcase_add_test(load_case, uv_loaded);
	tcase_add_test(load_case, name_loaded);
	suite_add_tcase(s, load_case);
	return s;
}
