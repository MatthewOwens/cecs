#include "cecs.h"
#include "cecs_err.h"
#include <stdlib.h>
#include <malloc.h>
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
	if(cecs == NULL) return cecse(CECSE_NULL); 
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


int cecs_reg_system(struct cecs* cecs, const char* name, uint32_t incl_mask,
		    uint32_t excl_mask, sys_init_func init, sys_run_func run,
		    sys_free_func free)
{
	printf("registering cecs system %s\n", name);
	if(cecs == NULL) {
		return cecse(CECSE_NULL);
	} else if (cecs->state != CECS_UNINITILISED) {
		return cecse(CECSE_INVALID_OPERATION);
	}

	void* tmp = reallocarray(cecs->systems, cecs->num_systems + 1,
				 sizeof(struct cecs_system));
	if(tmp == NULL) { return cecse(CECSE_NOMEM); }

	cecs->systems = tmp;
	int i = cecs->num_systems;

	cecs->systems[i].init = init;
	cecs->systems[i].run = run;
	cecs->systems[i].free = free;

	cecs->systems[i].inclusion_mask = incl_mask;
	cecs->systems[i].exclusion_mask = excl_mask;
	cecs->systems[i].name = name;

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

struct cecs_system* cecs_system(struct cecs *cecs, const char* name)
{
	if(cecs == NULL || name == NULL){
		return NULL;
	}

	for(int i = 0; i < cecs->num_systems; ++i){
		if(strcmp(name, cecs->systems[i].name))
			return &cecs->systems[i];
	}

	return NULL;
}
