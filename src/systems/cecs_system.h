#pragma once
#include "cecs.h"
#include <stdbool.h>
#include "cecs_component_key.h"
#include "arr.h"

struct cecs;

typedef int (*sys_init_func)(struct cecs* cecs);
typedef void (*sys_run_func)();
typedef void (*sys_tidy_func)();

typedef enum {
	sys_unknown = -1,
	sys_init = 0,
	sys_per_frame,
	sys_direct_call
}sys_runtype;

struct cecs_system
{
	sys_init_func init;
	sys_run_func run;
	sys_tidy_func free;
	sys_runtype runtype;
	CECS_COMP_KEY read_keys;
	CECS_COMP_KEY write_keys;
	CECS_COMP_KEY ignore_keys;
	const char *name;
	bool registered;

	/* 
	 * tracking systems that must be executed prior
	 * to this one. Once dependencies have been resolved, names
	 * will be cleared and indices stored instead to reduce memory overhead
	 * and enable quicker parsing.
	*/
	array(int) dependIndices;
	array(char *)dependNames;
};

int cecs_reg_system(struct cecs* cecs, struct cecs_system* sys);
int cecs_rem_system(struct cecs* cecs, const char* name);
int cecs_resolve_sys_deps(struct cecs* cecs);
struct cecs_system* cecs_system(struct cecs *cecs, const char* name);
