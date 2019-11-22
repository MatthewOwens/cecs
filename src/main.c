#include <stdio.h>
#include "string.h"
#include "components/components.h"
#include "systems/gravity.h"
#include "cecs.h"
#include "arr.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
	ComponentPosition pos = {0};
	ComponentVelocity vel = {0};
	int enta;
	int entb;

	printf("initilising cecs...");
	struct cecs* cecs = cecs_init();
	if(cecs == NULL){
		printf("FAILED!\n");
		return 1;
	}
	printf("success!\n");

	printf("registering components\n");
	cecs_reg_component(cecs, "position", &pos, sizeof(ComponentPosition));
	cecs_reg_component(cecs, "velocity", &vel, sizeof(ComponentVelocity));

	cecs_reg_system(cecs, "gravity");
	cecs_sys_set_incl(cecs, "gravity",
			cecs_component_key(cecs, "position") |
			cecs_component_key(cecs, "velocity"));

	printf("position key is %d\n", cecs_component_key(cecs, "position"));
	printf("velocity key is %d\n", cecs_component_key(cecs, "velocity"));

	// building the entities
	cecs_add_entity(cecs, &enta);
	cecs_ent_add_component(cecs, enta, "position");

	cecs_add_entity(cecs, &entb);
	cecs_ent_add_component(cecs, enta, "position");
	cecs_ent_add_component(cecs, enta, "velocity");

	cecs_start(cecs);
	cecs_free(cecs);
	return 0;
}
