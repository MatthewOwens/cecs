#include "cecs.h"
#include "cecs_err.h"
#include "cecs_entity.h"
#include "openbsd-reallocarray.h"

#include <stdlib.h>

// returns null if there is none available, ptr to inactive entity if available
struct cecs_entitiy* get_inactive_entity(struct cecs* cecs)
{
	int len = cecs->free_entities.length;
	if(len == 0){
		return NULL;
	} else {
		return cecs->free_entities.data[len-1];
	}
}

int is_inactive(struct cecs* cecs, uint32_t entID)
{
	for(uint32_t i = 0; i < cecs->free_entities.length; ++i){
		if(cecs->entities[i].id == cecs->free_entities.data[i]->id)
			return 0;
	}
	return 1;
}

struct cecs_ent* ent_from_id(struct cecs* cecs, uint32_t id)
{
	// finding the entitiy with that id
	for(int i = 0; i < cecs->num_entities; ++i) {
		if(cecs->entities[i].id == id){
			return &cecs->entities[i];
		}
	}
	return NULL;

}

int extend_components(struct cecs* cecs)
{
	int i;
	for(i = 0; i < cecs->num_components; ++i){
		// TODO: increment each component data ptr

		// double size of component arrays
		// set num_free_entities to num_entities - 1
		// extend inactives array to accomodate new entries
		// populate new inactives
	}
	return cecse(CECSE_NONE);
}

int cecs_add_entity(struct cecs* cecs, struct cecs_entity* ent)
{
	void* tmp = NULL;
	if(cecs == NULL) return cecse(CECSE_NULL);
	if(ent == NULL) return cecse_msg(CECSE_INVALID_VALUE, "cecs_add_entity");
	ent = get_inactive_entity(cecs);

	// if there are no inactive entities
	if(ent == NULL) {
		//TODO: resize component arrays accordingly!
		cecs->num_entities++;
		tmp = obsdreallocarray(cecs->entities, cecs->num_entities,
				   sizeof(struct cecs_entity));

		if(tmp == NULL) return cecse(CECSE_NOMEM);
		cecs->entities = tmp;
		ent = &cecs->entities[cecs->num_entities - 1];
		// setting the identity id to it's position in our array
		ent->id = cecs->num_entities - 1;
	} else {
		ent->mask = 0;
		array_pop(cecs->free_entities);
	}

	// ensuring that no keys are associated with the new entitiy
	ent->mask = 0;
	return cecse(CECSE_NONE);
}

int cecs_rem_entity(struct cecs* cecs, struct cecs_entity* ent)
{
	if(cecs == NULL) return cecse(CECSE_NULL);
	if(ent == NULL){
		fprintf(stderr, "can't free null entitiy");
	}

	for(int i = 0; i < cecs->free_entities.length; ++i){
		// entity is already inactive, can report success
		if(ent == &cecs->free_entities.data[i])
			return cecse(CECSE_NONE);
	}

	// flagging the entity for removal
	array_push(cecs->free_entities, ent);
	return cecse(CECSE_NONE);
}

int cecs_ent_add_component(struct cecs *cecs, uint32_t id, char* name)
{
	struct cecs_entity* ent = NULL;
	if(cecs == NULL)	 return cecse(CECSE_NULL);
	ent = ent_from_id(cecs, id);
	if(ent == NULL) return cecse(CECSE_INVALID_VALUE);
	if(is_inactive(cecs, id) == 0) return cecse(CECSE_INVALID_VALUE);

	uint32_t key = cecs_component_key(cecs, name);
	if(key == 0) return cecse(CECSE_INVALID_VALUE);

	// adding the key to the entity
	ent->mask = ent->mask | key;
	return cecse(CECSE_NONE);
}

int cecs_ent_rem_component(struct cecs *cecs, uint32_t id, char* name)
{
	struct cecs_entity* ent = NULL;
	if(cecs == NULL) return cecse(CECSE_NULL);
	ent = ent_from_id(cecs, id);
	if(ent == NULL)		 return cecse(CECSE_INVALID_VALUE);
	if(is_inactive(cecs, id) == 0) return cecse(CECSE_INVALID_VALUE);

	uint32_t key = cecs_component_key(cecs, name);
	if(key == 0) return cecse(CECSE_INVALID_VALUE);

	// removing the key to the entity using bitwise xor
	ent->mask = ent->mask ^ key;
	return cecse(CECSE_NONE);
}

int cecs_ent_get_mask(struct cecs* cecs, int ent, uint32_t* mask)
{
	if(cecs == NULL || mask == NULL || ent < 0 || ent >= cecs->num_entities)
		return cecse(CECSE_INVALID_VALUE);

	return(cecs->entities[ent].mask);
}
