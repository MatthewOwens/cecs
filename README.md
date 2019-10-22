# cecs

## Summary
small entity-component-system written in C. The purpose of this project is 3-fold:
* brush up on c (again)
* wrap my head around entity-component-systems
* work using [github flow](https://guides.github.com/introduction/flow/)

## Building
1. clone the repo
2. run `make`
3. run `make check` to run unit tests

make install is currently unsupported, as this project is in no fit state
to be installed.

## Usage
to use cecs in your project, you can include the following files:
```c
#include "cecs.h"
#include "cecs_component.h"
#include "cecs_entitiy.h"
#include "cecs_system.h"
```
### Components
components are dumb data structures, added to entities and manipulated by systems. For example:
```c
typedef struct {
	float x;
	float y;
	float z;
} PosComponent;
```
would be registered in cecs like this:
```c
static PosComponent posComp = {0};
cecs_reg_component(cecs, "pos", &posComp, sizeof(posComp));
```
`cecs_reg_component` takes four params, a pointer to cecs, a name to associate
the component with, a pointer to the original var and the size of the original var.

once a component has been registered the original variable and it's associated
location in memory are no longer required. 
### Entities
### Systems
