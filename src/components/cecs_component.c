#include "cecs.h"
#include <stdlib.h>
#include <string.h>
#include "inttypes.h"
#include "cecs_err.h"
#include "cecs_component.h"
#include "openbsd-reallocarray.h"

int cecs_reg_component( struct cecs* cecs, const char* name,
			void *data, size_t size )
{
	if(cecs == NULL) {
		return cecserr(CECS_NULL);
	} else if (cecs->state == CECS_STARTED){
		return cecserr(CECS_INVALID_OPERATION);
	}

	// do we have room in the key?
	if(cecs->num_components == sizeof(CECS_COMP_KEY)) {
		return cecserr(CECS_NO_KEY);
	}

	void* tmp  = obsdreallocarray(cecs->components, cecs->num_components+1,
				sizeof(struct cecs_component));
	if (tmp == NULL) { return cecserr(CECS_NOMEM); }

	cecs->components = tmp;
	int i = cecs->num_components;


	cecs->components[i].data = malloc(size);
	memcpy(cecs->components[i].data, data, size);

	cecs->components[i].size = size;
	cecs->components[i].key = cecs->next_comp_key;
	cecs->components[i].name = name;

	cecs->num_components += 1;

	// generating a unique key for the next component
	cecs->next_comp_key <<= 1;
	return cecserr(CECS_NOERR);
}

CECS_COMP_KEY cecs_component_key(struct cecs *cecs, const char* name)
{
	const CECS_COMP_KEY null_mask = 0;
	CECS_COMP_KEY ret = null_mask;


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

struct cecs_component* cecs_component(struct cecs* cecs, const char* name)
{
	const char* nVal = "cecs_component - null name param!\n";
	const char* bVal = "cecs_component - requested compo doesn't exist\n";

	if(cecs == NULL){
		cecserr(CECS_NULL);
		return NULL;
	} else if (name == NULL){
		cecserr_msg(CECS_INVALID_VALUE, nVal);
		return NULL;
	}

	for(int i = 0; i < cecs->num_components; ++i){
		if(strcmp(name, cecs->components[i].name ) == 0){
			return &cecs->components[i];
		}
	}

	return NULL;
}
