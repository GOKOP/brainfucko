#include "repl.h"
#include "interpreter.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void repl(int mem_size) {
	uchar* memory = malloc(mem_size * sizeof(uchar));
	if(!memory) {
		printf("Can't allocate program memory\n");
		exit(1);
	}

	uchar* mem_ptr = memory;

	char* linebuf = NULL;
	size_t bufsize = 0;
	char* err = NULL;

	while(true) {
		printf("%li: %i> ", mem_ptr - memory, *mem_ptr);
		getline(&linebuf, &bufsize, stdin);
		if(strcmp(linebuf, ":q\n") == 0) break;
		process_program(linebuf, memory, &mem_ptr, mem_size, &err, true);

		if(err) {
			printf("%s\n", err);
			free(err);
			err = NULL;
		}
	}
	free(linebuf);
	free(memory);
}

