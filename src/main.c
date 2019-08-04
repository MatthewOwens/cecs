#include <stdio.h>
#include "string.h"
#include "components/components.h"
#include "systems/gravity.h"
#include "cecs.h"

int main(int argc, char **argv)
{
	printf("initilising cecs...");
	struct cecs* cecs = cecs_init();
	if(cecs == NULL){
		printf("FAILED!\n");
		return 1;
	}
	printf("success!\n");
	ComponentPosition pos = {0};
	ComponentVelocity vel = {0};

	printf("registering components\n");
	cecs_reg_component(cecs, "position", &pos, sizeof(ComponentPosition));
	cecs_reg_component(cecs, "velocity", &vel, sizeof(ComponentVelocity));

	cecs_reg_system(cecs, "gravity", cecs_component_key(cecs, "position") |
			cecs_component_key(cecs, "veloctiy"), 0, gravity_init,
			gravity_run, gravity_free);

	printf("position key is %d\n", cecs_component_key(cecs, "position"));
	printf("velocity key is %d\n", cecs_component_key(cecs, "velocity"));

	cecs_start(cecs);
	cecs_free(cecs);
	return 0;
}
