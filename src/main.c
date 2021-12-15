#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "interpreter.h"

#define READ_CHUNK 100
#define MEMSIZE 30000

char* load_program(char* fname) {
	FILE* in = fopen(fname, "r");
	if(!in) return NULL;

	int size = READ_CHUNK;
	char* program = malloc(size * sizeof(char));
	if(!program) return NULL;
	int offset = 0;
	int bytes_read = 0;

	while((bytes_read = fread(program + offset, 1, READ_CHUNK, in)) == READ_CHUNK) {
		size += READ_CHUNK;
		offset += READ_CHUNK;
		char* new_block = realloc(program, size * sizeof(char));
		if(!new_block) {
			free(program);
			return NULL;
		}
		program = new_block;
	}
	size -= (READ_CHUNK - bytes_read);
	program[size] = 0;

	return program;
}

void run_from_file(char* fname) {
	char* program = load_program(fname);
	if(program == NULL) {
		printf("Could not load the program from %s\n", fname);
		exit(1);
	}

	uchar* memory = malloc(MEMSIZE * sizeof(uchar));
	if(!memory) {
		printf("Can't allocate program memory\n");
		exit(1);
	}
	memset(memory, 0, MEMSIZE);

	uchar* mem_ptr = memory;
	int ret = process_program(program, memory, &mem_ptr, MEMSIZE, false);
	if(ret > 0) exit(ret);
}

void repl() {
	uchar* memory = malloc(MEMSIZE * sizeof(uchar));
	if(!memory) {
		printf("Can't allocate program memory\n");
		exit(1);
	}

	uchar* mem_ptr = memory;

	char* linebuf = NULL;
	size_t bufsize = 0;

	while(true) {
		printf("%li: %i> ", mem_ptr - memory, *mem_ptr);
		getline(&linebuf, &bufsize, stdin);
		if(strcmp(linebuf, ":q\n") == 0) break;
		process_program(linebuf, memory, &mem_ptr, MEMSIZE, true);
	}
	free(linebuf);
	free(memory);
}

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	if(strcmp(argv[1], "--repl") == 0) repl();
	else run_from_file(argv[1]);
	return 0;
}
