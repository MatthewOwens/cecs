#include "cecs.h"
#include "cecs_err.h"
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
