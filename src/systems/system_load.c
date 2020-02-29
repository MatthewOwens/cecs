#include "system_load.h"
#include "cecs_system.h"
#include "yaml_helper.h"
#include "cecs_err.h"
#include "cecs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <yaml.h>

#define N_ELEMENTS 5
#define N_FUNCTIONS 3

// preprocessor magic for includes

#define XSTR(x) STR(x)
#define STR(x) #x
#ifdef CECS_SYS_FUNCS
	#pragma message "CECS_SYS_FUNCS set to " XSTR(CECS_SYS_FUNCS)
#else
	//#error "CECS_SYS_FUNCS is undefined!" 
	#warning "CECS_SYS_FUNCS is undefined!" //TODO: switch to error
#endif

#ifdef CECS_USR_FUNCS
	#pragma message "CECS_USR_FUNCS set to " XSTR(CECS_USR_FUNCS)
#else
	#warning "CECS_USR_FUNCS is undefined!" 
#endif

// dynamic library loading
#ifdef _WIN32	//TODO: windows testing
	#include <windows.h>
	#define LIBTYPE HMODULE
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

/*
 * both can be NULL since HMODULE seems to just be a pointer under the hood
 * from the docs:
 *	If the function fails, the return value is NULL
 * https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibraryw
*/

static LIBTYPE sysfuncs = NULL;
static LIBTYPE usrfuncs = NULL;

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

void cecs_free_sys_yaml()
{
	printf("\t%s\n", __FUNCTION__);
	#ifdef CECS_SYS_FUNCS
		DLCLOSE(sysfuncs);
	#endif
	#ifdef CECS_USR_FUNCS
		DLCLOSE(usrfuncs);
	#endif
}

static void load_libs()
{
	printf("\t%s\n", __FUNCTION__);
	printf("\tCECS_SYS_FUNCS = " XSTR(CECS_SYS_FUNCS) "\n");
	//printf("\tCECS_USR_FUNCS = %s\n", CECS_USR_FUNCS);
	#ifdef CECS_SYS_FUNCS
	sysfuncs = DLOPEN(XSTR(CECS_SYS_FUNCS));
	if(sysfuncs == NULL) {
		fprintf(stderr,
		"couldn't load system funcs (%s) DLOPEN returned:\n\t'"
		XSTR(DLEFMT) "'\n", XSTR(CECS_SYS_FUNCS), DLERROR());
		exit(-1);
	} else { printf("CECS_SYS_FUNCS loaded!\n"); }
	#endif

	#ifdef CECS_USR_FUNCS
	usrfuncs = DLOPEN(CECS_USR_FUNCS, RTLD_LAZY);
	if(sysfuncs == NULL) {
		fprintf(stderr,
		"couldn't load user funcs (%s) DLOPEN returned DLEFMT\n",
		CECS_USR_FUNCS, DLERROR());
		exit(-1);
	}
	#endif
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

//TODO: test set_func()
static void set_func(struct cecs_system* system, int iwt, const char* value)
{
	sys_init_func* i = NULL;
	sys_run_func* w = NULL;
	sys_tidy_func* t = NULL;

	void **sel = NULL;
	void **target = NULL;
	char str = "";

	switch(iwt){
	case 0:
		sel = &i;
		str = "init";
		target = &system->init;
		break;
	case 1:
		sel = &w;
		str = "work";
		target = &system->run;
		break;
	case 2:
		sel = &t;
		str = "tidy";
		target = &system->free;
		break;
	}

	*sel = DLFUNC(usrfuncs, value);
	if (*sel == NULL) { 
		*sel = DLFUNC(sysfuncs, value);
	}

	if(sel == NULL) {
		fprintf(stderr,
		"ERROR: " XSTR(DLFUNC) " couldn't match symbol %s\n", value);
	} else {
		*target = *sel;
	}
}

static void parse_scalar(const char* value, enum parse_state* s,
		struct cecs_system* system, struct cecs *cecs)
{
	static char* prevValue = NULL;
	set_scalar_state(value, s);


	switch(*s){
	case elem:
		*s = elem_m;
		break;
	case func:
		*s = func_m;
		break;
	case sys:
	{
		struct cecs_system empty = {0};
		*system = empty;
		system->name = value;
		break;
	}
	case elem_m:
		break;
	case func_m:
		if(strcmp(prevValue, "init") == 0) {
			set_func(system, 0, value);
		}
		else if(strcmp(prevValue, "work") == 0) {
			set_func(system, 1, value);
		}
		else if(strcmp(prevValue, "tidy") == 0) {
			set_func(system, 2, value);
		}
		break;
	}

	prevValue = value;
}

static int parse_event(yaml_parser_t *p, struct cecs* cecs)
{
	yaml_event_t e;
	static enum parse_state state;
	static struct cecs_system system = {0};
	static int mapping_depth = 0;

	if(!yaml_parser_parse(p, &e)){
		fprintf(stderr, "error parsing yml!\n");
		return bad;
	}

	switch (e.type) {
	case YAML_SEQUENCE_START_EVENT:
		break;
	case YAML_SEQUENCE_END_EVENT:
		break;
	case YAML_MAPPING_START_EVENT:
		mapping_depth++;
		if(mapping_depth == 1) {
			state = sys;
		}
		break;
	case YAML_MAPPING_END_EVENT:
		mapping_depth--;
		if(mapping_depth == 1) {
			cecs_reg_system(cecs, &system);
		}
		break;
	case YAML_ALIAS_EVENT:
		fprintf(stderr, " ERROR: Got alias (anchor %s)\n",
		    e.data.alias.anchor );
		// ensuring that state is isolated to a yaml document
		state = none;
		return bad;
		break;
	case YAML_SCALAR_EVENT:
		parse_scalar(e.data.scalar.value, &state, &system, cecs);
		break;
	case YAML_DOCUMENT_END_EVENT:
		// ensuring that state is isolated to a yaml document
		state = none;
		mapping_depth = 0;
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

	load_libs();

	do {
		status = parse_event(&parser, cecs);
	} while(status == good);
	yaml_parser_delete(&parser);

	if(status == bad) {
		return cecse_msg(CECSE_INVALID_VALUE, "bad component yaml");
	}

	return cecse(CECSE_NONE);
}