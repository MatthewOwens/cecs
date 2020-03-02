#include "cecs.h"
#include "cecs_err.h"
#include "cecs_component.h"
#include <stdlib.h>
#include <stdio.h>

struct cecs* cecs_init()
{
	struct cecs* cecs = NULL;
	cecs = malloc(sizeof(struct cecs));
	cecs->state = CECS_UNINITILISED;

	cecs->components = NULL;
	cecs->num_components = 0;

	cecs->systems = NULL;
	cecs->num_systems = 0; 

	cecs->entities = NULL;
	cecs->num_entities = 0;

	cecs->next_key = 0;

	/*
	 * can't use array_init here since arrays have already been
	 * declared, see arr.h for more.
	 *
	*/
	cecs->free_entities.length = 0;
	cecs->free_entities.capacity = 0;
	cecs->free_entities.data = NULL;

	cecs->registered_entities.length = 0;
	cecs->registered_entities.capacity = 0;
	cecs->registered_entities.data = NULL;

	cecs->registered_entity_names.length = 0;
	cecs->registered_entity_names.capacity = 0;
	cecs->registered_entity_names.data = NULL;

	cecserr(CECSE_NONE);
	return cecs;
}

int cecs_free(struct cecs* cecs)
{
	if(cecs == NULL) return cecserr(CECSE_NULL); 

	if(cecs->components != NULL){
		for(int i = 0; i < cecs->num_components; ++i){
			if(cecs->components[i].data != NULL){
				free(cecs->components[i].data);
			}
		}
		free(cecs->components);
	}

	if(cecs->systems != NULL) {
		printf("freeing systems\n");
		free(cecs->systems);
	}

	if(cecs->entities != NULL)		free(cecs->entities);
	array_free(cecs->free_entities);

	for(int i = 0; i < cecs->registered_entity_names.length; ++i)
		free(cecs->registered_entity_names.data[i]);
	array_free(cecs->registered_entity_names);
	array_free(cecs->registered_entities);

	free(cecs);
	cecs = NULL;
	return cecserr(CECSE_NONE);
}

int cecs_start(struct cecs* cecs)
{
	if(cecs == NULL){
		return cecserr(CECSE_NULL);
	} else if(cecs->state != CECS_UNINITILISED) {
		return cecserr(CECSE_INVALID_OPERATION);
	}

	cecs->state = CECS_STARTED;
	return cecserr(CECSE_NONE);
}

