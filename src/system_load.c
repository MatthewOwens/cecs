#include "system_load.h"
#include "yaml_helper.h"
#include "cecs_err.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <yaml.h>

enum token_status{
	good = 0,
	bad,
	complete
};

static int parse_token(yaml_parser_t *p, yaml_token_t *t)
{
	yaml_parser_scan(p, t);
	switch(t->type)
	{
	/* Stream start/end */
	case YAML_STREAM_START_TOKEN: printf("STREAM START\n");	break;
	case YAML_STREAM_END_TOKEN:   printf("STREAM END\n");	break;
	/* Token types (read before actual t) */
	case YAML_KEY_TOKEN:   printf("(Key token)   \n");	break;
	case YAML_VALUE_TOKEN: printf("(Value token) \n");	break;
	/* Block delimeters */
	case YAML_BLOCK_SEQUENCE_START_TOKEN: printf("{\n");	break;
	case YAML_BLOCK_ENTRY_TOKEN:          printf("- ");	break;
	case YAML_BLOCK_END_TOKEN:            printf("}\n");	break;
	/* Data */
	case YAML_BLOCK_MAPPING_START_TOKEN:  printf("[Block mapping] {\n");	break;
	case YAML_SCALAR_TOKEN:  printf("scalar %s \n", t->data.scalar.value);	break;
	/* Others */
	default:
	printf("Got token of type %d\n", t->type);
	}
	if(t->type == YAML_STREAM_END_TOKEN){
		yaml_token_delete(t);
		return complete;
	}
	yaml_token_delete(t);
	return good;
}

int cecs_load_sys_yaml( struct cecs* cecs, const char* filename)
{

	FILE *fh = fopen("systems.yml", "r");
	int status = -1;
	yaml_parser_t parser;
	yaml_token_t  token;

	/* Initialize parser */
	if(!yaml_parser_initialize(&parser))
	fputs("Failed to initialize parser!\n", stderr);
	if(fh == NULL)
	fputs("Failed to open file!\n", stderr);

	/* Set input file */
	yaml_parser_set_input_file(&parser, fh);

	/* BEGIN new code */
	do {
		status = parse_token(&parser, &token);
	} while(status == good);

	/* Cleanup */
	yaml_parser_delete(&parser);
	fclose(fh);

	return cecse(CECSE_STUB);
}
