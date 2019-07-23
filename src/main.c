#include <stdio.h>
#include "string.h"
#include "components/components.h"
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

	vel.x = 5.f;
	printf("position: (%f,%f)\n", pos.x, pos.y);
	printf("velocity: (%f,%f)\n", vel.x, vel.y);

	printf("registering components\n");
	cecs_reg_component(cecs, &pos, sizeof(ComponentPosition));
	cecs_reg_component(cecs, &vel, sizeof(ComponentVelocity));

	return 0;
}

