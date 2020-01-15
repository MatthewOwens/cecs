/*
 * The component generator is it's own destinct program and is run before the
 * rest of the engine. The goal is to take a user-provided component yaml file
 * and output a valid c header defining structs based on the yaml input.
 * 
 * TODO: generate cecs comp. registration function
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>
#include <errno.h>

static char* outfiles[2];
static char* yaml_events[YAML_MAPPING_END_EVENT + 1]=
{
	"YAML_NO_EVENT",
	"YAML_STREAM_START_EVENT",
	"YAML_STREAM_END_EVENT",
	"YAML_DOCUMENT_START_EVENT",
	"YAML_DOCUMENT_END_EVENT",
	"YAML_ALIAS_EVENT",
	"YAML_SCALAR_EVENT",
	"YAML_SEQUENCE_START_EVENT",
	"YAML_SEQUENCE_END_EVENT",
	"YAML_MAPPING_START_EVENT",
	"YAML_MAPPING_END_EVENT"
};

static char* types[3] = 
{
	"int",
	"float",
	"const char*"
};

enum {
	INT=0,
	FLOAT,
	STRING
};

enum {
	C=0,
	H=1
};
 
void usage(char* exe)
{
	printf("%s, usage:\n\t%s <input yml file> <output .h/.c name>\n",
	exe, exe);
}

int args_valid(int argc, char* argv[])
{
	if(argc != 3) {
		usage(argv[0]);
		printf("got args:\n");
		for(int i = 0; i < argc; ++i){
			printf("\t%d:%s\n", i, argv[i]);
		}
		return 1;
	}

	return 0;
}

void set_outfiles(char *arg)
{
	char* end[2] = {".c", ".h"};

	for( int i = 0; i < 2; ++i)
	{
		outfiles[i] = malloc(strlen(arg) + 2);
		strcpy(outfiles[i], arg);
		strcat(outfiles[i], end[i]);
	}
}

/* possible types are string, float or int */
int determine_type(const char* value)	//TODO: stub
{
	char* p = value;
	errno = 0;
	unsigned long ival = 0;
	float fval = 0.f;

	ival = strtoul(value, &p, 10);
	if(errno == 0 && value != p && *p == 0)
		return INT;
	fval = strtof(value, &p);
	if(errno == 0 && value != p && *p == 0)
		return FLOAT;
	fval = strtof(value, &p);
	return STRING;
}

int parse_event(yaml_parser_t *p, yaml_event_t *e, FILE *c, FILE *h)
{

	/*
	 * File seems to start off with a mapping event and switch later
	 * After we've parsed the first event we'll switch this to an empty
	 * event for the rest of the program
 	*/
	static yaml_event_type_t start_event = YAML_MAPPING_START_EVENT;
	static yaml_event_t prev_event;
	static char *key = NULL;
	static char *value = NULL;

	if(!yaml_parser_parse(p, e)){
		fprintf(stderr, "error parsing yml!\n");
		return 0;
	}

	/* TODO: process the event */
	printf("event: %s", yaml_events[e->type]);
	if(e->type == YAML_SCALAR_EVENT){
		printf("\t| %s\n", e->data.scalar.value);
	} else if (e->type == YAML_MAPPING_START_EVENT){
		printf("\t| %s\n", e->data.mapping_start.tag);
	}
	printf("\n");

	/*********************
	 * parsing the event *
	 ********************/

	// new struct is being defined
	if(e->type == start_event){
		// if not at the first struct definition, close the brace
		// doc ends with 2 end mappings, so we exclude the second
		if(start_event != YAML_MAPPING_START_EVENT && 
		prev_event.type != YAML_MAPPING_END_EVENT){
			fprintf(h, "}\n");
		}
	}

	if(e->type != YAML_SCALAR_EVENT){
		goto cleanup;
	}

	// if we've started a new struct
	if(prev_event.type == start_event) {
		fprintf(h, "struct %s_component {\n",
		e->data.scalar.value);
		start_event = YAML_MAPPING_END_EVENT;
		goto cleanup;
	}

	if (key == NULL) {
		key = e->data.scalar.value;
	} else {
		value = e->data.scalar.value;
		int t = determine_type(value);
		if(t == STRING) { // for strings we need to wrap it
			char* v = malloc(strlen(value) + 3);
			sprintf(v, "\"%s\"", value);
			fprintf(h, "\t %s %s = %s;\n", types[t],
				key, v);
			free(v);
		} else {
			fprintf(h, "\t %s %s = %s;\n", types[t],
				key, value);
		}
		key = NULL;
		value = NULL;
	}

	if(e->type == YAML_MAPPING_END_EVENT) {
		// we've hit a new line, clear our values
		key = NULL;
		value = NULL;
	}

	cleanup:
	prev_event = *e;
	return (e->type == YAML_STREAM_END_EVENT);
}

void build_files(char *in)
{
	yaml_parser_t parser;
	yaml_event_t event;
	FILE *infile = fopen(in, "rb");
	FILE *cfile = fopen(outfiles[0], "w+");
	FILE *hfile = fopen(outfiles[1], "w+");
	int done = 0;

	yaml_parser_initialize(&parser);
	yaml_parser_set_input_file(&parser, infile);

	do {
		done = parse_event(&parser, &event, cfile, hfile);
	} while(!done);

	yaml_parser_delete(&parser);

	fclose(infile);
	fclose(cfile);
	fclose(hfile);
}

int main(int argc, char* argv[])
{
	if(args_valid(argc, argv) == 1) {
		exit(1);
	}

	printf(("args OK\n"));
	set_outfiles(argv[2]);
	printf("outfiles are '%s' and '%s'\n", outfiles[0], outfiles[1]);

	build_files(argv[1]);
}

