#include "cecs_component.h"
#include "cecs_err.h"
#include "cecs.h"
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

int cecs_reg_component( struct cecs* cecs, const char* name,
			void *data, size_t size )
{
	static uint32_t skey = 1;

	if(cecs == NULL) {
		return cecse(CECSE_NULL);
	} else if (cecs->state == CECS_STARTED){
		return cecse(CECSE_INVALID_OPERATION);
	}

	void* tmp  = reallocarray(cecs->components, cecs->num_components + 1,
				sizeof(struct cecs_component));
	if (tmp == NULL) { return cecse(CECSE_NOMEM); }

	cecs->components = tmp;
	int i = cecs->num_components;

	// generating a bit-unique key for this component
	skey = skey << 1;
	printf("skey is %d\n" , skey);

	cecs->components[i].data = malloc(size);
	memcpy(cecs->components[i].data, data, size);

	cecs->components[i].size = size;
	cecs->components[i].key = skey;
	cecs->components[i].name = name;

	printf("successfully registered %s component\n", name);

	cecs->num_components += 1;
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
		if(strcmp(name, cecs->components[i].name)){
			ret = cecs->components[i].key;
			break;
		}
	}
	return ret;
}