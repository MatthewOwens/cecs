#include "statemanager.h"
#include <stdlib.h>

int StateManager_init(StateManager *sm){
	sm->capacity = 3;
	sm->stack = malloc(sm->capacity * sizeof(State*));
	sm->top = -1;
	return 0;
}
int StateManager_free(StateManager *sm)
{
	do{
		StateManager_pop(sm);
	} while (sm->top > -1);
	free(sm->stack);
	return 0;
}
int StateManager_scale(StateManager *sm)
{
	sm->capacity *= 2;
	State **tmp = realloc(sm->stack,
			sm->capacity * sizeof(State*));
	if(tmp)
		sm->stack = tmp;
	return sm->capacity;
}
int StateManager_push(StateManager *sm, State *state)
{
	if(sm->top+1 == sm->capacity){
		StateManager_scale(sm);
	}
	sm->top++;
	sm->stack[sm->top] = state;
	if(state->init != NULL)
		state->init();
	return sm->top;

}
int StateManager_pop(StateManager *sm)
{
	if(sm->top == -1)
		return 0;
	State* top = StateManager_top(sm);
	if(top->destroy != NULL)
		top->destroy();
	sm->stack[sm->top] = NULL;
	sm->top--;
	return sm->top;
}
State *StateManager_top(StateManager *sm)
{
	return sm->stack[sm->top];
}
int StateManager_update(StateManager *sm, float deltatime)
{
	State *state = StateManager_top(sm);
	if(state->update != NULL)
		return state->update(deltatime);
	return 1;
}

int StateManager_draw(StateManager *sm, float deltatime)
{
	State *state = StateManager_top(sm);
	if(state->render != NULL)
		return state->render(deltatime);
	return 1;
}
