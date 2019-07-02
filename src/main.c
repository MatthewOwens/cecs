#include <stdio.h>
#include "jsmn.h"
#include "string.h"

void print_toks(jsmntok_t *t, const char *str, int max_toks)
{
	if(!t){
		fprintf(stderr, "%s: token pointer passed is null!", __func__);
		return;
	}


	for(int i = 0; i < max_toks; ++i){
		if(t[i].type == JSMN_UNDEFINED)
			return;
		printf("%.*s\n", t[i].end - t[i].start, str + t[i].start);
	}
}

int main(int argc, char **argv)
{
	const char str[] = "{ \"name\": \"Jack\", \"age\" : 27 }";
	const char str2[] = "{ \"object\": { \"int\" : 99, \"bool\":false}}";
	int tokens = -1;

	jsmn_parser p;
	jsmntok_t t[128];

	jsmn_init(&p);
	tokens = jsmn_parse(&p, str2, strlen(str2), t, 128);

	if(tokens < 0){
		printf("jsmn error: %d\n", tokens);
	} else {
		printf("jsmn parsed %d dokens from %s\n", tokens, str2);
		print_toks(t, str2, tokens);
	}
	return 0;
}

