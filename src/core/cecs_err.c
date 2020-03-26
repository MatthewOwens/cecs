#include <stdio.h>
#include "cecs_err.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static enum cecs_errno lasterrno = CECS_NONE;

static char* cecs_errmsg[] = {
	"",
	"invalid value",
	"invalid operation",
	"cecs could not allocate any more memory!",
	"stub function called",
	"cecs is uninitilised"
};

int cecserr(enum cecs_errno err)
{
	return cecserr_msg(err, "");
}

int cecserr_msg(enum cecs_errno err, const char* msg)
{
	if (err != CECS_NONE)
		fprintf(stderr,
		ANSI_COLOR_RED "cecs error: %s %s\n" ANSI_COLOR_RESET,
		cecs_errmsg[err], msg);
	return err;
}

int cecserr_last()
{
	return cecserr(lasterrno);
}

void cecserr_set(enum cecs_errno err)
{
	lasterrno = err;
}
