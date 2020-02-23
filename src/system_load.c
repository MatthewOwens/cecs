#include "system_load.h"
#include "yaml_helper.h"
#include "cecs_err.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <yaml.h>

#define N_ELEMENTS 5
#define N_FUNCTIONS 3

// preprocessor magic for includes
#ifdef CECS_SYS_FUNCS
	#pragma message "CECS_SYS_FUNCS set to " STRING(CECS_SYS_FUNCS)
#else
	//#error "CECS_SYS_FUNCS is undefined!" 
	#warning "CECS_SYS_FUNCS is undefined!" //TODO: switch to error
#endif

#ifdef CECS_USR_FUNCS
	#pragma message "CECS_USR_FUNCS set to " STRING(CECS_USR_FUNCS)
#else
	#warning "CECS_USR_FUNCS is undefined!" 
#endif

// dynamic library loading
#ifdef _WIN32	//TODO: windows testing
	#include <windows.h>
	#define LIBTYPE HWINSTANCE
	#define DLOPEN(libname) LoadLibraryW(L ## libname)
	#define DLFUNC(lib, fn) GetProcAddress((lib), (fn))
	#define DLCLOSE(lib) FreeLibrary((lib))
	#define DLERROR GetLastError
	#define DLEFMT %d
#else	// linux/macos
	#include <dlfcn.h>
	#define LIBTYPE void*
	#define DLOPEN(libname) dlopen((libname), RTLD_LAZY)
	#define DLFUNC(lib, fn) dlsym((lib), (fn))
	#define DLCLOSE(lib) dlclose((lib))
	#define DLERROR dlerror
	#define DLEFMT %s
#endif

static char *elements[5] = {
	"reads",
	"writes",
	"ignores",
	"runtype",
	"functions"
};

static char *functions[3] = {
	"init",
	"work",
	"tidy"
};

enum event_status{
	good = 0,
	bad,
	complete
};

enum parse_state{
	none = -1,
	sys = 0,
	elem,
	func,
	elem_m,
	func_m
};

static void cleanup()
{

}

static void set_scalar_state(const char* value, enum parse_state* s)
{
	/*
	 * are we parsing a function or element?
	 * systems are handled by YAML_MAPPING_END_EVENT
	 */

	for(int i = 0; i < N_ELEMENTS; ++i){
		if(strcmp(value, elements[i]) == 0) {
			*s = elem;
			return;
		}
	}

	for(int i = 0; i < N_FUNCTIONS; ++i){
		if(strcmp(value, functions[i]) == 0) {
			*s = func;
			return;
		}
	}
}


static void parse_scalar(const char* value, enum parse_state* s)
{
	set_scalar_state(value, s);

	switch(*s){
	case elem:
		printf("elem - ");
		*s = elem_m;
		break;
	case func:
		printf("func - ");
		*s = func_m;
		break;
	case sys:
		printf("sys - ");
		break;
	case elem_m:
		printf("   e - ");
		break;
	case func_m:
		printf("   f - ");
		break;
	}
}

static int parse_event(yaml_parser_t *p)
{
	yaml_event_t e;
	static enum parse_state state;

	if(!yaml_parser_parse(p, &e)){
		fprintf(stderr, "error parsing yml!\n");
		return bad;
	}

	//print_yaml_event(&e);

	switch (e.type) {
	case YAML_SEQUENCE_START_EVENT:
		break;
	case YAML_SEQUENCE_END_EVENT:
		break;
	case YAML_MAPPING_END_EVENT:
		state = sys;
		break;
	case YAML_ALIAS_EVENT:
		fprintf(stderr, " ERROR: Got alias (anchor %s)\n",
		    e.data.alias.anchor );
		// ensuring that state is isolated to a yaml document
		state = none;
		return bad;
		break;
	case YAML_SCALAR_EVENT:
		parse_scalar(e.data.scalar.value, &state);
		printf("\t%s\n", e.data.scalar.value);
		break;
	case YAML_DOCUMENT_END_EVENT:
		// ensuring that state is isolated to a yaml document
		state = none;
		return complete;
	}


	return good;
}

int cecs_load_sys_yaml( struct cecs* cecs, const char* filename)
{
	if(cecs == NULL) {
		cecse_msg(CECSE_NULL, __FUNCTION__);
	}

	yaml_parser_t parser;
	FILE *infile = fopen(filename, "rb");
	static yaml_event_type_t start_event = YAML_MAPPING_START_EVENT;
	int status = 0;

	yaml_parser_initialize(&parser);
	yaml_parser_set_input_file(&parser, infile);

	do {
		status = parse_event(&parser);
	} while(status == good);
	yaml_parser_delete(&parser);

	if(status == bad) {
		cleanup();
		return cecse_msg(CECSE_INVALID_VALUE, "bad component yaml");
	}

	cleanup();
	return cecse(CECSE_NONE);
}
