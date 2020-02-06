#include "cecs.h"
#include "cecs_err.h"
#include "cecs_entity.h"
#include "openbsd-reallocarray.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// returns null if there is none available, ptr to inactive entity if available
struct cecs_entity* get_inactive_entity(struct cecs* cecs)
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

struct cecs_entity* ent_from_id(struct cecs* cecs, uint32_t id)
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
	void* tmp = NULL;
	const char* err = "can't extend component array!\n";
	int i;
	int target_size = cecs->num_entities * 2;
	int old_size = cecs->num_entities;

	if(target_size == 0)
		target_size = 1;

	for(i = 0; i < cecs->num_components; ++i){
		// double size of component arrays
		tmp = obsdreallocarray(cecs->components[i].data,
					target_size,
					cecs->components[i].size);

		if(tmp == NULL)
			return cecse_msg(CECSE_NOMEM, err);
		cecs->components[i].data = tmp;

	}

	// extend inactives array to accomodate new entries
	for(i = old_size; i < cecs->num_entities; ++i){
		array_push(cecs->free_entities, &cecs->entities[i]);
	}
	return cecse(CECSE_NONE);
}

int need_components_extended(struct cecs* cecs)
{
	if(cecs == NULL){
		cecse_msg(CECSE_NULL, "need_components_extended");
		return false;
	}
	return (cecs->free_entities.length == 0);
}

int cecs_add_entity(struct cecs* cecs, struct cecs_entity** ent)
{
	void* tmp = NULL;
	if(cecs == NULL) return cecse(CECSE_NULL);
	struct cecs_entity* inactive = get_inactive_entity(cecs);

	// if there are no inactive entities
	if(inactive == NULL) {
		if(need_components_extended(cecs)) extend_components(cecs);

		cecs->num_entities++;
		tmp = obsdreallocarray(cecs->entities, cecs->num_entities,
				   sizeof(struct cecs_entity));

		if(tmp == NULL) return cecse(CECSE_NOMEM);
		cecs->entities = tmp;
		*ent = &cecs->entities[cecs->num_entities - 1];
		// setting the identity id to it's position in our array
		(*ent)->id = cecs->num_entities - 1;
	} else {
		*ent = inactive;
		(*ent)->id = cecs->num_entities - 1;
		array_pop(cecs->free_entities);
	}

	// ensuring that no keys are associated with the new entitiy
	(*ent)->mask = 0;
	return cecse(CECSE_NONE);
}

int cecs_rem_entity(struct cecs* cecs, struct cecs_entity** ent)
{
	if(cecs == NULL) return cecse(CECSE_NULL);
	if(ent == NULL){
		fprintf(stderr, "can't free null entitiy");
		return cecse(CECSE_INVALID_VALUE);
	}

	for(int i = 0; i < cecs->free_entities.length; ++i){
		// entity is already inactive, can report success
		if(ent == cecs->free_entities.data[i])
			return cecse(CECSE_NONE);
	}

	// flagging the entity for removal
	array_push(cecs->free_entities, *ent);
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
	if(key == -1) return cecse(CECSE_INVALID_VALUE);

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
	if(key == -1) return cecse(CECSE_INVALID_VALUE);

	// removing the key to the entity using bitwise xor
	ent->mask = ent->mask ^ key;
	return cecse(CECSE_NONE);
}

//TODO: test
int cecs_reg_entity(struct cecs *cecs, char* name, int n_comps, ...)
{
	if (cecs == NULL) { return cecse_msg(CECSE_NULL, __FUNCTION__); }
	if (name == NULL){
		return cecse_msg(CECSE_INVALID_VALUE, __FUNCTION__);
	}

	va_list args;
	uint32_t key = -1;
	struct cecs_entity ent;
	ent.mask = 0;
	ent.id = -1;

	/*
 	 * building our entity mask, we can return from here if reg. fails
 	 * since we don't allocate anything on the heap in this loop
 	 */
	va_start(args, n_comps);
	for(int i = 0; i < n_comps; ++i) {
		arg = va_arg(args, char*);
		key = cecs_component_key(cecs, arg);
		if(key == -1) {
			return cecse_msg(CECSE_INVALID_VALUE,
			"can't register entity, component doesn't exist");
		}

		ent.mask | key;
	}
	va_end(args);
	
	/*
 	 * using strdup since the pointer passed in to here will be freed
 	 * when the yaml loader finishes
 	 */
	char *tmp = strdup(name);
	array_push(cecs->registered_entities, ent);
	array_push(cecs->registered_entities, tmp);
}

int cecs_add_entity_v(struct cecs *cecs, struct cecs_entity **ent,
		int comp_count, ...)
{
	va_list args;
	int ret = -1;
	char* arg = NULL;

	// validating args
	va_start(args, comp_count);
	for(int i =0; i < comp_count; ++i) {
		arg = va_arg(args, char*);

		if(cecs_component_key(cecs, arg) == -1) {
			ent = NULL;
			return cecse_msg(CECSE_INVALID_VALUE,
			"can't add component to ent, doesn't exist\n!");
		}
	}
	va_end(args);

	// args are valid, add the ent
	cecs_add_entity(cecs, ent);

	// add the components
	va_start(args, comp_count);
	for(int i = 0; i < comp_count; ++i){
		cecs_ent_add_component(cecs, (*ent)->id, va_arg(args, char*));
	}
	va_end(args);

	return cecse(CECSE_NONE);
}
