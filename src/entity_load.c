#include "entity_load.h"
#include <cyaml/cyaml.h>
#include "cecs_err.h"

struct in_ent{
	char *name;
	char **components;
};

static struct in_ent ent = {0};

/* Schema for string pointer values (used in sequences of strings). */
static const cyaml_schema_value_t copmonent_entry = {
	CYAML_VALUE_STRING(CYAML_FLAG_POINTER, char, 0, CYAML_UNLIMITED),
};

/* Individual entity mapping */
//TODO:component sequence correct?
static const cyaml_schema_field_t top_level_mapping_schema[] = {
	CYAML_FIELD_STRING_PTR("name", CYAML_FLAG_POINTER, struct in_ent,
		name, 0, CYAML_UNLIMITED),
	CYAML_FIELD_SEQUENCE("components", CYAML_FLAG_POINTER, struct in_ent,
		components, &component_entry, 0, CYAML_UNLIMITED),
	CYAML_FIELD_END
};

int cecs_load_ent_yaml( struct cecs* cecs, const char* filename)
{
	if(cecs == NULL) {
		cecse_msg(CECSE_NULL, __FUNCTION__);
	}
	return cecse_msg(CECSE_STUB, __FUNCTION__);
}
