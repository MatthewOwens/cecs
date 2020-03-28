#pragma once
#include "cecs.h"
#include "cecs_component_key.h"

struct cecs;

struct cecs_component
{
	void *data;
	size_t size;
	CECS_COMP_KEY key;
	const char *name;
};

// registers data struct with cecs, returns 0 on success
int cecs_reg_component( struct cecs* cecs, const char* name,
			void *data, size_t size );

CECS_COMP_KEY cecs_component_key(struct cecs* cecs, const char* name);
struct cecs_component* cecs_component(struct cecs* cecs, const char* name);
