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
	"runtype",
	"reads",
	"writes",
	"ignores",
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
	func,
	func_m,
	rt,
	rt_m,
	r,
	r_m,
	w,
	w_m,
	i,
	i_m
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

	if(strcmp(value, elements[0]) == 0) { *s = rt; }
	if(strcmp(value, elements[1]) == 0) { *s = r; }
	if(strcmp(value, elements[2]) == 0) { *s = w; }
	if(strcmp(value, elements[3]) == 0) { *s = i; }
	if(strcmp(value, elements[4]) == 0) { *s = func; }

	//for(int i = 0; i < N_FUNCTIONS; ++i){
	//	if(strcmp(value, functions[i]) == 0) {
	//		*s = func;
	//		return;
	//	}
	//}
}

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

static void set_runtype(const char* value, struct cecs_system *system)
{
	const char* valid_values[3] = {
		"init",
		"per-frame",
		"direct-call"
	};

	for(int i = 0; i < 3; ++i){
		if(strcmp(value, valid_values[i]) == 0) {
			system->runtype = i;
			return;
		}
	}

	system->runtype = unknown;
}

static void append_read_key(const char* value, struct cecs_system* system,
			struct cecs *cecs){
	const uint32_t bad_key = -1;
	uint32_t key = cecs_component_key(cecs, value);

	if(key != bad_key) {
		array_push(system->read_keys, key);
	}
}

static void append_write_key(const char* value, struct cecs_system* system,
			struct cecs *cecs){
	const uint32_t bad_key = -1;
	uint32_t key = cecs_component_key(cecs, value);

	if(key != bad_key) {
		array_push(system->write_keys, key);
	}
}

static void append_ignore_key(const char* value, struct cecs_system* system,
			struct cecs *cecs){
	const uint32_t bad_key = -1;
	uint32_t key = cecs_component_key(cecs, value);

	if(key != bad_key) {
		array_push(system->ignore_keys, key);
	}
}

static void parse_scalar(const char* value, enum parse_state* s,
		struct cecs_system* system, struct cecs *cecs)
{
	static char* prevValue = NULL;
	set_scalar_state(value, s);


	switch(*s){
	case rt:	// runtype
		*s = rt_m;
		break;
	case r:		// reads
		*s = r_m;
		break;
	case w:		// writes
		*s = w_m;
		break;
	case i:		// ignores
		*s = i_m;
		break;
	case func:	// functions
		*s = func_m;
		break;
	case sys:
	{
		struct cecs_system empty = {0};
		*system = empty;
		system->name = value;
		break;
	}
	//TODO: parse reads, writes, ignores & runtype
	case rt_m:
		set_runtype(value, system);
		break;
	case r_m:
		append_read_key(value, system, cecs);
		break;
	case w_m:
		append_write_key(value, system, cecs);
		break;
	case i_m:
		append_ignore_key(value, system, cecs);
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
			printf("registered system\n");
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
