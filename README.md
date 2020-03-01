<p align="center">
<br/>
  <img src="assets/logo.png" width="160px"/><br/>
  (sɛks)<br/><br/>
  a small ecs, written in c.<br/>
<br/>
<a href="https://github.com/MatthewOwens/cecs/issues"><img alt="Issues" src="https://img.shields.io/github/issues-raw/MatthewOwens/cecs.svg"/></a>
</p>

## Summary
small entity-component-system written in C. The purpose of this project is 3-fold:
* brush up on c (again)
* wrap my head around entity-component-systems
* work using [github flow](https://guides.github.com/introduction/flow/)

## Building
0. ensure that make, gcc, pkg-config, libcheck and libyaml are installed
1. clone the repo
2. run `make components` to generate component c/h file
3. run `make` to build libs & run unit tests

make install is currently unsupported, but is on the roadmap once the windows
build is complete.

## Usage
to use cecs in your project, you can include the following files:
```c
#include "cecs.h"
#include "cecs_entity.h"
#include "cecs_system.h"
```
all cecs functions return a `cecs_errno` enum, defined in `cecs_err.h`
### Components
components are dumb data structures, added to entities and manipulated by systems.

#### Defining components with YAML
components can be defined with YAML for fast iteration. only a small subset of
YAML is actually supported, but for defining simple data structures (as described
below). For example anchors and lists aren't supported, but they aren't really needed.
An example of components defined in YAML can be found in
[components.yml](components.yml)

#### Under the hood

##### Generation
Since we are defining the data structures _themselves_ as well as the default
data that should fill them when we define our component YAML, we have to
generate some C code to define the data structures. This is done in
[comp_gen.c](src/components/comp_gen.c). This is compiled into the `components`
target with `make components`. The `components` recipe also cleans up this
target once it has been run, and generated the `src/components/components.h`
and `src/components/components.c` files.

Note that as these files are generated they should _not_ be checked in to git!

##### Registration
For example:
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
cecs_reg_component(cecs, "position", &posComp, sizeof(posComp));
```
`cecs_reg_component` takes four parameters
* a pointer to cecs
* a name to associate the component with
* a pointer to the original var and the size of the original var.

once a component has been registered the original variable and it's associated
location in memory are no longer required. 


### Entities
Entities represent discreet object types in-game, and are comprised of
one or more components.

#### Defining entities with YAML
Similarly to components, only a subset of YAML is supported in defining
entities.
An example of entities defined in YAML can be found in
[entities.yml](entities.yml)

#### Under the hood
entities are clusters of components that represent an object in the game.
empty entities are registered with:
```c
int cecs_reg_entity(struct cecs *cecs, char* name, int n_comps, char **comps);
```
here the `name`, `n_comps` and `comps` parameters are loaded from YAML.

there is also support for adding & removing components from and entity at
runtime with:
```c
int cecs_ent_add_component(struct cecs *cecs, uint32_t id, char* name);
```
and
```c
int cecs_ent_rem_component(struct cecs *cecs, uint32_t id, char* name);
```

### Systems
documentation required
