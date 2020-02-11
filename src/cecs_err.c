#include <stdio.h>
#include "cecs_err.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static enum cecs_errno lasterrno = CECSE_NONE;

static char* cecs_errmsg[] = {
	"",
	"invalid value",
	"invalid operation",
	"cecs could not allocate any more memory!",
	"stub function called",
	"cecs is uninitilised"
};

int cecse(enum cecs_errno err)
{
	return cecse_msg(err, "");
}

int cecse_msg(enum cecs_errno err, const char* msg)
{
	if (err != CECSE_NONE)
		fprintf(stderr,
		ANSI_COLOR_RED "cecs error: %s %s\n" ANSI_COLOR_RESET,
		cecs_errmsg[err], msg);
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
