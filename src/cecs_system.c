#include "cecs_system.h"
#include "cecs_err.h"
#include "cecs.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

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

int cecs_sys_set_incl(struct cecs* cecs, const char* name, uint32_t incl_mask)
{
	struct cecs_system* sys = NULL;
	sys = cecs_system(cecs, name);
	if(sys == NULL)
		return cecse(CECSE_INVALID_VALUE);

	sys->inclusion_mask = incl_mask;
	return cecse(CECSE_NONE);
}

int cecs_sys_set_excl(struct cecs* cecs, const char* name, uint32_t excl_mask)
{
	struct cecs_system* sys = NULL;
	sys = cecs_system(cecs, name);
	if(sys == NULL)
		return cecse(CECSE_INVALID_VALUE);

	sys->exclusion_mask = excl_mask;
	return cecse(CECSE_NONE);
}

int cecs_sys_set_funcs(struct cecs* cecs, const char* name,
			sys_init_func init,
			sys_run_func run,
			sys_free_func free)
{
	struct cecs_system* sys = NULL;
	sys = cecs_system(cecs, name);
	if(sys == NULL)
		return cecse(CECSE_INVALID_VALUE);

	sys->init = init;
	sys->run = run;
	sys->free = free;
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

int cecs_free_system(struct cecs* cecs, const char* name)
{
	//TODO
	return -1;
}
