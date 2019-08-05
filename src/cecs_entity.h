#pragma once
#include "cecs.h"

struct cecs;

int cecs_add_entitiy(struct cecs *cecs, int *ent);
int cecs_rem_entitiy(struct cecs *cecs, int ent);
int cecs_ent_add_component(struct cecs *cecs, int ent, char* name);
int cecs_ent_rem_component(struct cecs *cecs, int ent, char* name);
