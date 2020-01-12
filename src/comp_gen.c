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

static char* outfiles[2];

void usage(char* exe)
{
	printf("%s, usage:\n\t%s <input yml file> <output .h/.c name>\n",
	exe, exe);
}

int argsValid(int argc, char* argv[])
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
	int size = strlen(arg);
	char* end[2] = {".c", ".h"};

	for( int i = 0; i < 2; ++i)
	{
		outfiles[i] = malloc(strlen(arg) + 2);
		strcpy(outfiles[i], arg);
		strcat(outfiles[i], end[i]);
	}

	//outfiles[1] = malloc(strlen(arg) + 3);
	//outfiles[1] = strcat(".c", arg);
	printf("YEET\n");
}

int main(int argc, char* argv[])
{
	if(argsValid(argc, argv) == 1) {
		exit(1);
	}

	printf(("args OK\n"));
	set_outfiles(argv[2]);
	printf("outfiles are '%s' and '%s'\n", outfiles[0], outfiles[1]);
}

