#include "gravity.h"
#include "cecs.h"
#include "cecs_err.h"

static float force;
static struct cecs* cecs_ptr = NULL;

int gravity_init(struct cecs* cecs)
{
	if(cecs_ptr == NULL)
		cecs_ptr = cecs;
	force = 9.8f;
	return cecse(CECSE_NONE);
}

void gravity_run()
{
	struct cecs_system* self = cecs_system(cecs_ptr, "gravity");
}

void gravity_free()
{
}
