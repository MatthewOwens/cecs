/*
 * The component generator is it's own destinct program and is run before the
 * rest of the engine. The goal is to take a user-provided component yaml file
 * and output a valid c header defining structs based on the yaml input.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>
#include <errno.h>
#include "yaml_helper.h"

static char* outfiles[2];

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
	printf("%s, usage:\n\t%s <input yml file> <out dir> <output .h/.c name>\n",
	exe, exe);
}

int args_valid(int argc, char* argv[])
{
	if(argc != 4) {
		usage(argv[0]);
		printf("got args:\n");
		for(int i = 0; i < argc; ++i){
			printf("\t%d:%s\n", i, argv[i]);
		}
		return 1;
	}

	return 0;
}

void set_outfiles(char* dir, char *name)
{
	char* end[2] = {".c", ".h"};

	for( int i = 0; i < 2; ++i)
	{
		outfiles[i] = malloc(strlen(dir) + strlen(name) + 3);
		if(strlen(dir) != 0) {
			strcpy(outfiles[i], dir);
			strcat(outfiles[i], "/"); //TODO: windows compat
		}
		strcat(outfiles[i], name);
		strcat(outfiles[i], end[i]);
	}
}

/* possible types are string, float or int */
int determine_type(const char* value)
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

int parse_event(yaml_parser_t *p, yaml_event_t *e, FILE *c, FILE *h, FILE *t)
{

	/*
	 * File seems to start off with a mapping event and switch later
	 * After we've parsed the first event we'll switch this to an empty
	 * event for the rest of the program
 	*/
	static yaml_event_type_t start_event = YAML_MAPPING_START_EVENT;
	static yaml_event_t prev_event;
	static char* reg_start = "cecs_reg_component(cecs, ";
	static char *key = NULL;
	static char *value = NULL;

	if(!yaml_parser_parse(p, e)){
		fprintf(stderr, "error parsing yml!\n");
		return 0;
	}

	/*********************
	 * printing the event *
	 ********************/
	print_yaml_event(e);
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
			fprintf(h, "};\n");
			fprintf(c, "\t};\n");
		}
	}

	if(e->type != YAML_SCALAR_EVENT){
		goto cleanup;
	}

	// if we've started a new struct
	if(prev_event.type == start_event) {
		fprintf(h, "struct %s_cmpnt {\n",
		e->data.scalar.value);
		start_event = YAML_MAPPING_END_EVENT;

		fprintf(c, "\tconst struct %s_cmpnt %s = {\n",
			e->data.scalar.value, e->data.scalar.value);

		fprintf(t, "\t%s\"%s\", &%s, sizeof(%s));\n",
			reg_start, e->data.scalar.value, e->data.scalar.value,
			e->data.scalar.value);
		printf("\t%s\"%s\", &%s, sizeof(%s));\n",
			reg_start, e->data.scalar.value, e->data.scalar.value,
			e->data.scalar.value);
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
			fprintf(h, "\t %s %s;// = %s;\n", types[t],
				key, v);
			fprintf(c, "\t\t%s,\n", v);
			free(v);
		} else {
			fprintf(h, "\t %s %s;// = %s;\n", types[t],
				key, value);
			fprintf(c, "\t\t%s,\n", value);
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

void build_files(char *in, char* out_dir, char *out_name)
{
	yaml_parser_t parser;
	yaml_event_t event;
	FILE *infile = fopen(in, "rb");
	FILE *cfile = fopen(outfiles[0], "w+");
	FILE *hfile = fopen(outfiles[1], "w+");
	FILE *tmp = tmpfile();
	const char* func_proto = "void reg_loaded_components(struct cecs* cecs)";
	int done = 0;
	char ch;

	if(tmp == NULL) {
		fprintf(stderr, "Failed to open tmpfile!\n");
		exit(-1);
	}

	yaml_parser_initialize(&parser);
	yaml_parser_set_input_file(&parser, infile);

	// building the start of our .h/.c files, since it's consistent
	fprintf(hfile, "#include \"cecs.h\"\n");
	fprintf(hfile, "%s;\n\n", func_proto);

	fprintf(cfile, "#include \"%s.h\"\n", out_name);
	fprintf(cfile, "#include \"cecs.h\"\n\n", out_name);
	fprintf(cfile, "%s\n{\n", func_proto);

	do {
		done = parse_event(&parser, &event, cfile, hfile, tmp);
	} while(!done);
	yaml_parser_delete(&parser);

	// concat
	rewind(tmp);
	while((ch = fgetc(tmp)) != EOF)
	{
		putchar(ch);
		fputc(ch,cfile);
	}

	// still need to finish off our .c file
	fprintf(cfile, "};");

	fclose(infile);
	fclose(cfile);
	fclose(hfile);
	fclose(tmp);
}

int main(int argc, char* argv[])
{
	if(args_valid(argc, argv) == 1) {
		exit(1);
	}

	printf(("args OK\n"));
	set_outfiles(argv[2], argv[3]);
	printf("outfiles are '%s' and '%s'\n", outfiles[0], outfiles[1]);

	build_files(argv[1], argv[2], argv[3]);
}

