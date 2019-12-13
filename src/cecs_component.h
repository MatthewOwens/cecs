#pragma once
#include "cecs.h"

struct cecs;

struct cecs_component
{
	void *data;
	size_t size;
	uint32_t key;
	const char *name;
};

// registers data struct with cecs, returns 0 on success
int cecs_reg_component( struct cecs* cecs, const char* name,
			void *data, size_t size );

uint32_t cecs_component_key(struct cecs* cecs, const char* name);
struct cecs_component* cecs_component(struct cecs* cecs, const char* name);
