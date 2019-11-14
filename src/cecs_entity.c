#include "cecs.h"
#include "cecs_err.h"
#include "cecs_entity.h"
#include "openbsd-reallocarray.h"

#include <stdlib.h>

// returns -1 if there is none available, index of inactive entity if available
int get_inactive_entity(struct cecs* cecs)
{
	int len = cecs->inactive_entities.length;
	if(len == 0){
		return -1;
	} else {
		return cecs->inactive_entities.data[len-1];
	}
}

int extend_components(struct cecs* cecs)
{
	int i;
	for(i = 0; i < cecs->num_components; ++i){
		// TODO: increment each component data ptr

		// double size of component arrays
		// set num_inactive_entities to num_entities - 1
		// extend inactives array to accomodate new entries
		// populate new inactives
	}
	return cecse(CECSE_NONE);
}

int cecs_add_entity(struct cecs* cecs, int *ent)
{
	void* tmp = NULL;
	if(cecs == NULL) return cecse(CECSE_NULL);
	if(ent == NULL) return cecse(CECSE_INVALID_VALUE);
	int index = get_inactive_entity(cecs);

	// if there are no inactive entities
	if(index == -1) {
		//TODO: resize component arrays accordingly!
		cecs->num_entities++;
		tmp = obsdreallocarray(cecs->entities, cecs->num_entities,
				   sizeof(uint32_t));

		if(tmp == NULL) return cecse(CECSE_NOMEM);
		cecs->entities = tmp;
		index = cecs->num_entities - 1;

	} else {
		array_pop(cecs->inactive_entities);
	}

	// ensuring that no keys are associated with the new entitiy
	cecs->entities[index] = 0;
	*ent = index;
	return cecse(CECSE_NONE);
}

int cecs_rem_entity(struct cecs* cecs, int ent)
{
	if(cecs == NULL) return cecse(CECSE_NULL);
	if(ent < 0 || ent > cecs->num_entities - 1)
		return cecse(CECSE_INVALID_OPERATION);

	for(int i = 0; i < cecs->inactive_entities.length; ++i){
		// entity is already inactive, can report success
		if(ent == cecs->inactive_entities.data[i])
			return cecse(CECSE_NONE);
	}

	// flagging the entity for removal
	array_push(cecs->inactive_entities, ent);
	return cecse(CECSE_NONE);
}

int cecs_ent_add_component(struct cecs *cecs, int ent, char* name)
{
	if(cecs == NULL) return cecse(CECSE_NULL);
	if(ent < 0 || ent > cecs->num_entities - 1)
		return cecse(CECSE_INVALID_OPERATION);

	uint32_t key = cecs_component_key(cecs, name);
	if(key == 0) return cecse(CECSE_INVALID_VALUE);

	// adding the key to the entity
	cecs->entities[ent] = cecs->entities[ent] | key;
	return cecse(CECSE_NONE);
}

int cecs_ent_rem_component(struct cecs *cecs, int ent, char* name)
{
	if(cecs == NULL) return cecse(CECSE_NULL);
	if(ent < 0 || ent > cecs->num_entities - 1)
		return cecse(CECSE_INVALID_OPERATION);

	uint32_t key = cecs_component_key(cecs, name);
	if(key == 0) return cecse(CECSE_INVALID_VALUE);

	// removing the key to the entity using bitwise xor
	cecs->entities[ent] = cecs->entities[ent] ^ key;
	return cecse(CECSE_NONE);
}
