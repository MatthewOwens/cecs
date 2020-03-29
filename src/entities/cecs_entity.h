#pragma once
#include "cecs.h"
#include "cecs_component_key.h"

struct cecs;

struct cecs_entity
{
	uint32_t id;
	CECS_COMP_KEY mask;
};

int cecs_reg_entity(struct cecs *cecs, char* name, int n_comps, char **comps);
int cecs_add_entity(struct cecs *cecs, char* name, struct cecs_entity** out);
int cecs_rem_entity(struct cecs *cecs, struct cecs_entity** ent);
int cecs_ent_add_component(struct cecs *cecs, uint32_t id, char* name);
int cecs_ent_rem_component(struct cecs *cecs, uint32_t id, char* name);
