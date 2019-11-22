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

	/*
	 * can't use array_init here since free_entities has already been
	 * declared, see arr.h for more.
	 *
	*/
	cecs->free_entities.length = 0;
	cecs->free_entities.capacity = 0;
	cecs->free_entities.data = NULL;

	cecse(CECSE_NONE);
	return cecs;
}

int cecs_free(struct cecs* cecs)
{
	if(cecs == NULL) return cecse(CECSE_NULL); 

	if(cecs->components != NULL){
		for(int i = 0; i < cecs->num_components; ++i){
			if(cecs->components[i].data != NULL){
				printf("freeing component %d\n", i);
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

	free(cecs);
	return cecse(CECSE_NONE);
}

int cecs_start(struct cecs* cecs)
{
	if(cecs == NULL){
		return cecse(CECSE_NULL);
	} else if(cecs->state != CECS_UNINITILISED) {
		return cecse(CECSE_INVALID_OPERATION);
	}

	cecs->state = CECS_STARTED;
	return cecse(CECSE_NONE);
}

