#include <stdio.h>
#include "statemanager.h"

int first_init()
{
	printf("init 1\n");
	return 0;
}

int second_init()
{
	printf("init 2\n");
	return 0;
}

int generic_destroy()
{
	printf("kill\n");
	return 0;
}

int main(int argc, char **argv)
{
	State state_one = {0};
	State state_two = {0};
	StateManager sm;

	StateManager_init(&sm);

	state_one.init = first_init;
	state_one.destroy = generic_destroy;
	state_two.init = second_init;

	StateManager_push(&sm,&state_one);
	StateManager_push(&sm,&state_two);
	StateManager_pop(&sm);
	StateManager_pop(&sm);

	printf("hello\n");
	return 0;
}
