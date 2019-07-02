#pragma once

typedef int (*fnPtr)();
typedef int (*fnPtrF1)(float);

typedef struct {
	fnPtr init;
	fnPtrF1 update;
	fnPtrF1 render;
	fnPtr destroy;
	fnPtr on_pause;
	fnPtr on_resume;
} State;
