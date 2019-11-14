#include "cecs_system.h"
#include "cecs_err.h"
#include "cecs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "openbsd-reallocarray.h"

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

	void* tmp = obsdreallocarray(cecs->systems, cecs->num_systems + 1,
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
