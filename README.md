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
### Entities
### Systems
