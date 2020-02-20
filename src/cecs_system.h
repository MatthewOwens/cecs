#pragma once
#include "cecs.h"
#include <stdbool.h>

struct cecs;

typedef int (*sys_init_func)(struct cecs* cecs);
typedef void (*sys_run_func)();
typedef void (*sys_tidy_func)();

struct cecs_system
{
	sys_init_func init;
	sys_run_func run;
	sys_tidy_func free;
	uint32_t inclusion_mask;
	uint32_t exclusion_mask;
	const char *name;
	bool registered;
};

int cecs_reg_system(struct cecs* cecs, const char* name);
int cecs_sys_set_incl(struct cecs* cecs, const char* name, uint32_t incl_mask);
int cecs_sys_set_excl(struct cecs* cecs, const char* name, uint32_t excl_mask);
int cecs_sys_set_funcs(struct cecs* cecs, const char* name,
			sys_init_func init,
			sys_run_func run,
			sys_tidy_func free);
int cecs_rem_system(struct cecs* cecs, const char* name);

struct cecs_system* cecs_system(struct cecs *cecs, const char* name);
