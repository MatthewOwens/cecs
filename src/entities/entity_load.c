#include "openbsd-reallocarray.h"
#include "entity_load.h"
#include "yaml_helper.h"
#include "cecs_err.h"
#include "cecs_entity.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <yaml.h>
#include <arr.h>

struct entity{
	const char **components;
	int components_count;
};

static array(struct entity) entities = array_init();
static array(char *) names = array_init();

enum event_status{
	good = 0,
	bad,
	complete
};

static void cleanup()
{
	for(int i = 0; i < entities.length; ++i){
		for(int j = 0; j < entities.data[i].components_count; ++j){
			free(entities.data[i].components[j]);
		}
		free(entities.data[i].components);
		free(names.data[i]);
	}
	array_free(entities);
	array_free(names);
}

static void add_comp(struct entity *e, const char* name)
{
	// sure we're reallocing after every push, but since this is just
	// running on init, it's not too big a deal
	void* tmp = obsdreallocarray(e->components, e->components_count + 1,
			sizeof(const char*));
	if(tmp){
		e->components = tmp;
		e->components[e->components_count] = malloc(strlen(name));
		strcpy(e->components[e->components_count], name);
		e->components_count++;
	}
}

static int parse_event(yaml_parser_t *p)
{
	static const struct entity empty_ent = {0};
	static bool in_seq = false;
	static bool passed_first_seq = false;
	static bool name_parse = false;
	yaml_event_t e;

	if(!yaml_parser_parse(p, &e)){
		fprintf(stderr, "error parsing yml!\n");
		// ensuring that state is isolated to a yaml document
		in_seq = false;
		passed_first_seq = false;
		name_parse = false;
		return bad;
	}

	switch (e.type) {
	case YAML_SEQUENCE_START_EVENT:
		if(!passed_first_seq) {
			passed_first_seq = true;
		} else {
			in_seq = true;
			array_push(entities, empty_ent);
		}
		break;
	case YAML_SEQUENCE_END_EVENT:
		in_seq = false;
		break;
	case YAML_MAPPING_START_EVENT:
		/*
		 * since we don't care about the entity name, we can ignore the
		 * mapping entierly.
		 */
		break;
	case YAML_ALIAS_EVENT:
		fprintf(stderr, " ERROR: Got alias (anchor %s)\n",
		    e.data.alias.anchor );
		// ensuring that state is isolated to a yaml document
		in_seq = false;
		passed_first_seq = false;
		name_parse = false;
		return bad;
		break;
	case YAML_SCALAR_EVENT:
		if(in_seq) {
			add_comp(&(entities.data[entities.length-1]),
			e.data.scalar.value);
		}
		else if(!name_parse) {
			if(strcmp((char*)e.data.scalar.value, "name") == 0){
				name_parse = true;
			}
		}
		else if (name_parse) {
			array_push(names, NULL);
			names.data[names.length-1]=strdup(e.data.scalar.value);
			name_parse = false;
		}
		break;
	case YAML_DOCUMENT_END_EVENT:
		// ensuring that state is isolated to a yaml document
		in_seq = false;
		passed_first_seq = false;
		name_parse = false;
		return complete;
	}

	return good;
}

int cecs_load_ent_yaml(struct cecs* cecs, const char* filename)
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

	/* registering entities*/
	for(int i = 0; i < entities.length; ++i){
		cecs_reg_entity(cecs, names.data[i],
			entities.data[i].components_count,
			entities.data[i].components);
	}
	cleanup();
	return cecse(CECSE_NONE);
}

