#pragma once
#include "cecs.h"

struct cecs;

struct cecs_entity
{
	uint32_t id;
	uint32_t mask;
};

int cecs_add_entity(struct cecs *cecs, struct cecs_entity* ent);
int cecs_rem_entity(struct cecs *cecs, struct cecs_entity* ent);
int cecs_ent_add_component(struct cecs *cecs, uint32_t id, char* name);
int cecs_ent_rem_component(struct cecs *cecs, uint32_t id, char* name);

// vargs should all be of type const char*
int cecs_add_entity_v(struct cecs *cecs, struct cecs_entity* ent,
		int comp_count, ...);
