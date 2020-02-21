#include "cecs_system.h"
#include "cecs_err.h"
#include "cecs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "openbsd-reallocarray.h"

int cecs_reg_system(struct cecs* cecs, struct cecs_system* sys)
{
	if(sys == NULL) {
		return cecse_msg(CECSE_INVALID_VALUE,
			"tried to register system, but sys is NULL\n");
	}
	printf("registering cecs system %s\n", sys->name);
	printf("checking if sys with that name already exists...\n");
	if(cecs == NULL) {
		return cecse(CECSE_NULL);
	} else if (cecs->state != CECS_UNINITILISED) {
		return cecse(CECSE_INVALID_OPERATION);
	} else if (cecs_system(cecs, sys->name) != NULL) {
		fprintf(stderr, "\tsystem with name %s already exists!\n",
				sys->name);
		return cecse(CECSE_INVALID_VALUE);
	}

	struct cecs_system* sysptr = NULL;
	for(int i = 0; i < cecs->num_systems; ++i){
		if(&cecs->systems[i] == NULL){
			sysptr = &cecs->systems[i];
			break;
		}
	}

	if(sysptr == NULL){
		if(cecs->num_systems == 0){
			void* tmp = obsdreallocarray(cecs->systems,
			1, sizeof(struct cecs_system));
		}
		} else {
			void* tmp = obsdreallocarray(cecs->systems,
			cecs->num_systems*2, sizeof(struct cecs_system));
		}

		if(tmp == NULL) { return cecse(CECSE_NOMEM); }

		cecs->systems = tmp;
		sysptr = &cecs->systems[cecs->num_systems];
	}

	sys->registered = true;	// just making sure
	memcpy(sysptr, sys, sizeof(struct cecs_system));

	cecs->num_systems *= 2;
	return cecse(CECSE_NONE);
}

struct cecs_system* cecs_system(struct cecs *cecs, const char* name)
{
	if(cecs == NULL || name == NULL){
		if(cecs == NULL)
			printf("cecs is null\n");
		else
			printf("name is null\n");
		return NULL;
	}

	for(int i = 0; i < cecs->num_systems; ++i){
		if(strcmp(name, cecs->systems[i].name) != 0){
			printf("continuing...");
			continue;
		}
		if(cecs->systems[i].registered == true)
			return &cecs->systems[i];
		else
		{
			printf("%s.registered is %d\n", cecs->systems[i].name,
					cecs->systems[i].registered);
			return NULL;
		}
	}
	printf("no system name matched %s\n", name);
	return NULL;
}

int cecs_rem_system(struct cecs* cecs, const char* name)
{
	if(cecs == NULL || name == NULL){
		cecse_msg(CECSE_INVALID_VALUE,
			"null param in cecs_rem_system");
	}
	struct cecs_system* sys = NULL;
	sys = cecs_system(cecs, name);
	if(sys == NULL){
		char buf[80];
		const char* x = "- cecs_rem_system called on system ";
		const char* y = " but system doesn't exist!";
		strcpy(buf, x);
		strcpy(buf+strlen(x), name);
		strcpy(buf+strlen(x)+strlen(name), y);
		return cecse_msg(CECSE_INVALID_VALUE, buf);
	}

	sys->registered = false;
	printf("removed system %s\n", sys->name);
	sys->free(sys);
	sys->free = NULL;
	sys->run = NULL;
	sys->init = NULL;
	return cecse(CECSE_NONE);
}
