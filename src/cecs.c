#include "cecs.h"
#include "cecs_err.h"
#include "stdlib.h"
#include <string.h>
#include <stdio.h>

struct cecs* cecs_init()
{
	struct cecs* cecs = NULL;
	cecs = malloc(sizeof(struct cecs));
	cecs->state = CECS_UNINITILISED;
	cecs->components = NULL;
	cecs->num_components = 0;
	cecs->systems = NULL;
	cecs->num_systems = 0;

	cecse(CECSE_NONE);
	return cecs;
}

int cecs_free(struct cecs* cecs)
{
	if(cecs == NULL)
		return cecse(CECSE_NULL);

	if(cecs->components != NULL){
		for(int i = 0; i < cecs->num_components; ++i){
			if(cecs->components[i].data != NULL){
				printf("freeing component %d\n", i);
				free(cecs->components[i].data);
			}
		}
		free(cecs->components);
	}

	if(cecs->systems != NULL) {
		printf("freeing systems\n");
		free(cecs->systems);
	}

	free(cecs);
	return cecse(CECSE_NONE);
}

int cecs_reg_component(struct cecs* cecs, void *data, size_t size)
{
	static uint32_t skey = 0x0;

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

	cecs->components[i].data = malloc(size);
	memcpy(cecs->components[i].data, data, size);
	cecs->components[i].size = size;
	cecs->components[i].key = skey;

	cecs->num_components += 1;
	return cecse(CECSE_NONE);
}

int cecs_reg_system(struct cecs* cecs, cecs_sys_func func,
		    uint32_t incl_mask, uint32_t excl_mask)
{
	printf("registering cecs system %p\n", func);
	if(cecs == NULL) {
		return cecse(CECSE_NULL);
	} else if (cecs->state != CECS_UNINITILISED || func == NULL) {
		return cecse(CECSE_INVALID_OPERATION);
	}

	void* tmp = reallocarray(cecs->systems, cecs->num_systems + 1,
				 sizeof(struct cecs_system));
	if(tmp == NULL) { return cecse(CECSE_NOMEM); }

	cecs->systems = tmp;
	int i = cecs->num_systems;

	cecs->systems[i].function = func;
	cecs->systems[i].inclusion_mask = incl_mask;
	cecs->systems[i].exclusion_mask = excl_mask;

	cecs->num_systems += 1;
	return cecse(CECSE_NONE);
}

int cecs_start(struct cecs* cecs)
{
	if(cecs == NULL){
		return cecse(CECSE_NULL);
	} else if(cecs->state != CECS_UNINITILISED) {
		return cecse(CECSE_INVALID_OPERATION);
	}

	cecs->state = CECS_STARTED;
	return cecse(CECSE_NONE);
}
