#pragma once
#include "cecs.h"

struct cecs;

typedef int (*sys_init_func)(struct cecs* cecs);
typedef void (*sys_run_func)();
typedef void (*sys_free_func)();

struct cecs_system
{
	sys_init_func init;
	sys_run_func run;
	sys_free_func free;
	uint32_t inclusion_mask;
	uint32_t exclusion_mask;
	const char *name;
};

/*
 * registers a function with cecs, return 0 on success
 * incl_mask and excl_mask are component keys bitwise or'd together
*/
int cecs_reg_system(struct cecs* cecs, const char* name, uint32_t incl_mask,
		    uint32_t excl_mask, sys_init_func init, sys_run_func run,
		    sys_free_func free);
int cecs_free_system(struct cecs* cecs, const char* name);

struct cecs_system* cecs_system(struct cecs *cecs, const char* name);
