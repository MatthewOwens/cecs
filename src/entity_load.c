#include "entity_load.h"
#include <cyaml/cyaml.h>
#include "cecs_err.h"

struct entity{
	const char *name;
	const char **components;
	unsigned int components_count;
};

struct loaded_entities{
	struct entity* entity;
	unsigned int entity_count;
};

struct loaded_entities *in_ents = NULL;

static const cyaml_config_t config = {
	.log_level = CYAML_LOG_WARNING, /* Logging errors and warnings only. */
	.log_fn = cyaml_log,            /* Use the default logging function. */
	.mem_fn = cyaml_mem,            /* Use the default memory allocator. */
	.flags = CYAML_CFG_IGNORE_UNKNOWN_KEYS
};

/* Schema for string pointer values (used in sequences of strings). */
static const cyaml_schema_value_t component_entry = {
	CYAML_VALUE_STRING(CYAML_FLAG_POINTER, char, 0, CYAML_UNLIMITED),
};

/* Individual entity mapping */
static const cyaml_schema_field_t entity_field_schema[] = {
	CYAML_FIELD_STRING_PTR("name", CYAML_FLAG_POINTER, struct entity,
		name, 0, CYAML_UNLIMITED),
	CYAML_FIELD_SEQUENCE_COUNT("components", CYAML_FLAG_POINTER,
		struct entity, components, components_count, &component_entry,
		0, CYAML_UNLIMITED),
	CYAML_FIELD_END
};


int cecs_load_ent_yaml( struct cecs* cecs, const char* filename)
{
	if(cecs == NULL) {
		cecse_msg(CECSE_NULL, __FUNCTION__);
	}

	//TODO in_ents is null after load?
	cyaml_err_t err = cyaml_load_file(filename, &config, &entities_field_schema,
			(void **) &in_ents, NULL);
	printf("%p\n", in_ents);
	return cecse_msg(CECSE_STUB, __FUNCTION__);
}
