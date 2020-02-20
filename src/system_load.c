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
#ifdef CECSENG_SYSFUNCFILE
	#include CECSENG_SYSFUNCFILE
#endif

#ifdef USER_SYSFUNCFILE
	#include USER_SYSFUNCFILE
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

static void cleanup()
{

}


static void parse_scalar(const char* value, bool *elem, bool *func)
{
	if(*elem) {
		// are we looking at functions?
		for(int i = 0; i < N_FUNCTIONS; ++i) {
			if(strcmp(value, functions[i]) == 0){
				*func = true;
				printf("func - ");
				return;
			}
		}
	} else {
		// are we looking at an element?
		for(int i = 0; i < N_ELEMENTS; ++i) {
			if(strcmp(value, elements[i]) == 0){
				*elem = true;
				printf("elem - ");
				return;
			}
		}
	}

	if(!*func){
		*elem = false;
		*func = false;
	}
}

static int parse_event(yaml_parser_t *p)
{
	yaml_event_t e;
	static bool parsing_elem = false;
	static bool parsing_func = false;

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
	case YAML_MAPPING_START_EVENT:
		break;
	case YAML_ALIAS_EVENT:
		fprintf(stderr, " ERROR: Got alias (anchor %s)\n",
		    e.data.alias.anchor );
		// ensuring that state is isolated to a yaml document
		parsing_elem = false;
		parsing_func = false;
		return bad;
		break;
	case YAML_SCALAR_EVENT:
		parse_scalar(e.data.scalar.value,
		&parsing_elem, &parsing_func);
		printf("\t%s\n", e.data.scalar.value);
		break;
	case YAML_DOCUMENT_END_EVENT:
		// ensuring that state is isolated to a yaml document
		parsing_elem = false;
		parsing_func = false;
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
