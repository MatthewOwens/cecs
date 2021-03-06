#pragma once
#include <stdint.h>
#include <stddef.h>
#include "cecs_entity.h"
#include "cecs_component.h"
#include "cecs_component_key.h"
#include "cecs_system.h"
#include "arr.h"

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
	
	array(struct cecs_entity) registered_entities;
	array(char*) registered_entity_names;

	struct cecs_entity *entities;
	int num_entities;

	CECS_COMP_KEY next_comp_key;

	array(struct cecs_entity*) free_entities;
};

struct cecs* cecs_init();
int cecs_free(struct cecs* cecs);
int cecs_start(struct cecs* cecs);
