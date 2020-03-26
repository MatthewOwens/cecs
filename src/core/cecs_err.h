#pragma once

enum cecs_errno
{
	CECS_NONE = 0,
	CECS_INVALID_VALUE,
	CECS_INVALID_OPERATION,
	CECS_NOMEM,
	CECS_STUB,
	CECS_NULL
};

void cecserr_set(enum cecs_errno err);
int cecserr(enum cecs_errno err);
int cecserr_last();
int cecserr_msg(enum cecs_errno err, const char* msg);
