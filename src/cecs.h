#pragma once
#include <stdint.h>
#include <stddef.h>
#include "cecs_entity.h"
#include "cecs_component.h"
#include "cecs_system.h"

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

	uint32_t *entities;
	int num_entities;

	int *inactive_entities;
	int num_inactive_entities;
};

struct cecs* cecs_init();
int cecs_free(struct cecs* cecs);
int cecs_start(struct cecs* cecs);
