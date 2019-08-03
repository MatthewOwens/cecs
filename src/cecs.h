#pragma once
#include <stdint.h>
#include <stddef.h>

typedef void (*cecs_sys_func)();

struct cecs_component
{
	void *data;
	size_t size;
	uint32_t key;
};

struct cecs_system
{
	cecs_sys_func function;
	uint32_t inclusion_mask;
	uint32_t exclusion_mask;
};

enum cecs_state
{
	CECS_UNINITILISED = 0,
	CECS_INITILISED,
	CECS_STARTED
};

struct cecs{
	enum cecs_state state;

	struct cecs_component *components;
	int num_components;

	struct cecs_system *systems;
	int num_systems;
};

struct cecs* cecs_init();
int cecs_free(struct cecs* cecs);
int cecs_start(struct cecs* cecs);

// registers data struct with cecs, returns 0 on success
int cecs_reg_component(struct cecs* cecs, void *data, size_t size);

/*
 * registers a function with cecs, return 0 on success
 * incl_mask and excl_mask are the relevant component keys bitwise or'd together
*/
int cecs_reg_system(struct cecs* cecs, cecs_sys_func func,
		    uint32_t incl_mask, uint32_t excl_mask);
