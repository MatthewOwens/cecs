#pragma once
#include <stdint.h>
#include <stddef.h>


struct cecs_component
{
	void *data;
	size_t size;
	uint32_t key;
	const char *name;
};

enum cecs_state
{
	CECS_UNINITILISED = 0,
	CECS_STARTED
};

struct cecs{
	enum cecs_state state;

	struct cecs_component *components;
	int num_components;

	struct cecs_system *systems;
	int num_systems;
};

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

struct cecs* cecs_init();
int cecs_free(struct cecs* cecs);
int cecs_start(struct cecs* cecs);

// registers data struct with cecs, returns 0 on success
int cecs_reg_component( struct cecs* cecs, const char* name,
			void *data, size_t size );

/*
 * registers a function with cecs, return 0 on success
 * incl_mask and excl_mask are component keys bitwise or'd together
*/
int cecs_reg_system(struct cecs* cecs, const char* name, uint32_t incl_mask,
		    uint32_t excl_mask, sys_init_func init, sys_run_func run,
		    sys_free_func free);

uint32_t cecs_component_key(struct cecs* cecs, const char* name);
struct cecs_system* cecs_system(struct cecs *cecs, const char* name);
