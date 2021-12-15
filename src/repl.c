#include "repl.h"
#include "interpreter.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// 0 if not a special command
int process_commands(const char* input, bool* running) {
	if(input[0] != ':') return 0;

	if(strcmp(input, ":q\n") == 0) *running = false;
	return 1;
}

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
	bool running = true;

	while(running) {
		printf("%li: %i> ", mem_ptr - memory, *mem_ptr);
		getline(&linebuf, &bufsize, stdin);

		if(!process_commands(linebuf, &running))
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

