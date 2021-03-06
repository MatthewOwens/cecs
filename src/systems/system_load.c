#include "arr.h"
#include <yaml.h>
#include "cecs.h"
#include <yaml.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cecs_err.h"
#include "system_load.h"
#include "cecs_system.h"
#include "yaml_helper.h"
#include "cecs_component.h"
#include "cecs_component_key.h"

#define N_ELEMENTS 7
#define N_FUNCTIONS 3
#define SYS_NAME_MAX 32

// preprocessor magic for includes

#define XSTR(x) STR(x)
#define STR(x) #x
#ifdef CECS_SYS_FUNCS
	#pragma message "CECS_SYS_FUNCS set to " XSTR(CECS_SYS_FUNCS)
#else
	#error "CECS_SYS_FUNCS is undefined!" 
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

static char *elements[N_ELEMENTS] = {
	"runtype",
	"reads",
	"writes",
	"ignores",
	"functions",
	"adds",
	"consumes"
};

static char *functions[N_FUNCTIONS] = {
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
	add,
	add_m,
	con,
	con_m,
	rt,
	rt_m,
	r,
	r_m,
	w,
	w_m,
	i,
	i_m
};

void cecs_free_sys_libs()
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
	if(strcmp(value, elements[5]) == 0) { *s = add; }
	if(strcmp(value, elements[6]) == 0) { *s = con; }
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

	system->runtype = sys_unknown;
}

static void append_key(CECS_COMP_KEY comp_key, CECS_COMP_KEY* sys_keys)
{
	*sys_keys |= comp_key;
}

static void parse_scalar(const char* value, enum parse_state* s,
		struct cecs_system* system, struct cecs *cecs)
{
	static char* prevValue = NULL;
	int dummy = 1;		// dummy variable for component tagging
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
	case add:	// adds
		*s = add_m;
		break;
	case con:	// consumes
		*s = con_m;
		break;
	case sys:
	{
		struct cecs_system empty = {0};
		*system = empty;
		system->name = value;
		break;
	}
	case add_m:
	{
		// If the component hasn't been added, add tag component
		if(cecs_component_key(cecs, value) == 0) {
			cecs_reg_component(cecs, value, &dummy, sizeof(dummy));
		}
		array_push(system->addKeys, cecs_component_key(cecs, value));
		printf("%s adds %s on completion\n", system->name,
			value);
		printf("\tkey is %d\n", cecs_component_key(cecs, value));
		break;
	}
	case con_m:
	{
		// If the component hasn't been added, add tag component
		if(cecs_component_key(cecs, value) == 0) {
			cecs_reg_component(cecs, value, &dummy, sizeof(dummy));
		}
		array_push(system->consumeKeys,
			cecs_component_key(cecs, value));
		printf("%s consumes %s on completion\n", system->name,
			value);
		printf("\tkey is %d\n", cecs_component_key(cecs, value));
		break;
	}
	case rt_m:
		set_runtype(value, system);
		break;
	case r_m:
		append_key(cecs_component_key(cecs, value),
				&system->read_keys);
		break;
	case w_m:
		append_key(cecs_component_key(cecs, value),
				&system->write_keys);
		break;
	case i_m:
		append_key(cecs_component_key(cecs, value),
				&system->ignore_keys);
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
		cecserr_msg(CECS_NULL, __FUNCTION__);
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
		return cecserr_msg(CECS_INVALID_VALUE, "bad system yaml");
	}

	/*
	 * since systems must be the last thing we load for cecs, we can say
	 * that cecs has been intilised if we get here
	 */
	cecs->state = CECS_INITILISED;

	return cecserr(CECS_NOERR);
}
