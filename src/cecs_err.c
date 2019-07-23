#include <stdio.h>
#include "cecs_err.h"

static enum cecs_errno lasterrno = CECSE_NONE;

static char* cecs_errmsg[] = {
	"",
	"invalid value",
	"invalid operation",
	"cecs could not allocate any more memory!",
	"cecs is uninitilised"
};

int cecse(enum cecs_errno err)
{
	if (err != CECSE_NONE)
		fprintf(stderr, "cecs error: %s", cecs_errmsg[err]);
	return err;
}

int cecse_last()
{
	return cecse(lasterrno);
}

void cecse_set(enum cecs_errno err)
{
	lasterrno = err;
}
