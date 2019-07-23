#include "cecs.h"
#include "cecs_err.h"
#include "stdlib.h"

struct cecs* cecs_init()
{
	struct cecs* cecs = NULL;
	cecs = malloc(sizeof(struct cecs));
	cecs->state = CECS_UNINITILISED;
	cecs->components = NULL;
	cecs->num_components = 0;
	cecs->components = NULL;

	cecse(CECSE_NONE);
	return cecs;
}

int cecs_reg_component(struct cecs* cecs, void *data, size_t size)
{
	static uint32_t skey = 0x0;

	if(cecs == NULL) {
		return cecse(CECSE_UNINITILISED);
	} else if (cecs->state == CECS_INITILISED){
		return cecse(CECSE_INVALID_OPERATION);
	}

	void* tmp  = reallocarray(cecs->components, cecs->num_components + 1,
				sizeof(struct cecs_component));

	if(tmp == NULL){
		return cecse(CECSE_NOMEM);
	} else {
		cecs->components = tmp;

		// generating a bit-unique key for this component
		skey = skey << 1;

		cecs->components[cecs->num_components].data = data;
		cecs->components[cecs->num_components].size = size;
		cecs->components[cecs->num_components].key = skey;

		cecs->num_components += 1;
	}
	return cecse(CECSE_NONE);
}
