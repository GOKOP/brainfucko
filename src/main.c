#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "interpreter.h"
#include "repl.h"

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
	char* err = NULL;

	int ret = process_program(program, memory, &mem_ptr, MEMSIZE, &err, false);

	if(err) {
		printf("%s\n", err);
		free(err);
		err = NULL;
	}

	if(ret > 0) exit(ret);
}

void print_help(const char* bin_name) {
	printf("Usage: %s <filename>\n", bin_name);
	printf("       %s --help\n", bin_name);
	printf("       %s --repl\n", bin_name);
	printf("Exit codes:\n");
	printf("0 - success\n");
	printf("1 - error in the interpreter\n");
	printf("2 - brainfuck syntax error\n");
	printf("3 - brainfuck execution error\n");
}

int main(int argc, char** argv) {
	if(argc != 2) {
		print_help(argv[0]);
		exit(1);
	}

	if(strcmp(argv[1], "--help") == 0) print_help(argv[0]);
	else if(strcmp(argv[1], "--repl") == 0) repl(MEMSIZE);
	else run_from_file(argv[1]);
	return 0;
}
