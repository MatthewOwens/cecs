#pragma once
#include "state.h"

typedef struct {
	State **stack;
	int capacity;
	int top;
} StateManager;

int StateManager_init(StateManager *sm);
int StateManager_free(StateManager *sm);
int StateManager_push(StateManager *sm, State *state);
int StateManager_pop(StateManager *sm);
State *StateManager_top(StateManager *sm);
int StateManager_update(StateManager *sm, float deltatime);
int StateManager_draw(StateManager *sm, float deltatime);
int StateManager_scale(StateManager *sm);
