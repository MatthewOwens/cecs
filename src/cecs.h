#pragma once
#include <stdint.h>
#include <stddef.h>

struct cecs_component{
	void *data;
	size_t size;
	uint32_t key;
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
};

struct cecs* cecs_init();

// Registers data struct with cecs, returns 0 on success
int cecs_reg_component(struct cecs* cecs, void *data, size_t size);
