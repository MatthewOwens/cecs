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

void cecse_set(enum cecs_errno err);
int cecse(enum cecs_errno err);
int cecse_last();
int cecse_msg(enum cecs_errno err, const char* msg);
