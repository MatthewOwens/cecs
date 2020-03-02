#pragma once

enum cecs_errno
{
	CECSE_NONE = 0,
	CECSE_INVALID_VALUE,
	CECSE_INVALID_OPERATION,
	CECSE_NOMEM,
	CECSE_STUB,
	CECSE_NULL
};

void cecserr_set(enum cecs_errno err);
int cecserr(enum cecs_errno err);
int cecserr_last();
int cecserr_msg(enum cecs_errno err, const char* msg);
