#include "cecs_component.h"
#include "cecs_err.h"
#include "cecs.h"
#include <stdlib.h>
#include <string.h>
#include "inttypes.h"
#include "openbsd-reallocarray.h"

// model for one set of components, updated in cecs_reg_component
struct cecs_component* model = NULL;
const uint32_t comp_size = sizeof(struct cecs_component);
int registered_count = 0;

int cecs_extend_components(struct cecs* cecs)
{
	if(cecs == NULL) { return cecse_msg(CECSE_NULL, "extend components"); }
	void* tmp = obsdreallocarray(cecs->components,
			(cecs->num_components+1) * comp_size);
	if(tmp == NULL) { return cecse(CECSE_NOMEM); }

	// ensuring that values in existing components aren't overwritten
	for(int i = 0; i < cecs->num_components; ++i){
		tmp[i] = cecs->components[i];
	}

	// adding a new component set based on our model
	for(int i = 0; i < registered_count; ++i){
		tmp[cecs->num_components + i] = model[i];
	}
}

/*
 * TODO: update to edit model pointer, accessable components shouldn't be
 * added outside of entity creation
 */
int cecs_reg_component( struct cecs* cecs, const char* name,
			void *data, size_t size )
{
	static uint32_t skey = 1;

	if(cecs == NULL) {
		return cecse(CECSE_NULL);
	} else if (cecs->state == CECS_STARTED){
		return cecse(CECSE_INVALID_OPERATION);
	}

	void* tmp  = obsdreallocarray(cecs->components, cecs->num_components + 1,
				sizeof(struct cecs_component));
	if (tmp == NULL) { return cecse(CECSE_NOMEM); }

	cecs->components = tmp;
	int i = cecs->num_components;

	// generating a bit-unique key for this component
	skey = skey << 1;

	cecs->components[i].data = malloc(size);
	memcpy(cecs->components[i].data, data, size);

	cecs->components[i].size = size;
	cecs->components[i].key = skey;
	cecs->components[i].name = name;

	cecs->num_components += 1;
	registered_count++;
	return cecse(CECSE_NONE);
}

uint32_t cecs_component_key(struct cecs *cecs, const char* name)
{
	const uint32_t null_mask = 0;
	uint32_t ret = null_mask;


	if(cecs == NULL || name == NULL) {
		return null_mask;
	}

	for(int i = 0; i < cecs->num_components; ++i){
		if(strcmp(name, cecs->components[i].name) == 0){
			ret = cecs->components[i].key;
			break;
		}
	}
	return ret;
}
