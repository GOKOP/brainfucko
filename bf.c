#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

#define READ_CHUNK 100

typedef unsigned char uchar;

// size contains the size of the written buffer, without bytes allocated but not written
char* load_program(char* fname, size_t* size) {
	FILE* in = fopen(fname, "r");
	if(!in) return NULL;

	*size = READ_CHUNK;
	char* program = malloc((*size) * sizeof(char));
	size_t offset = 0;
	int bytes_read = 0;

	while((bytes_read = fread(program + offset, 1, READ_CHUNK, in)) == READ_CHUNK) {
		*size += READ_CHUNK;
		offset += READ_CHUNK;
		program = realloc(program, (*size) * sizeof(char));
	}
	*size -= (READ_CHUNK - bytes_read);

	return program;
}

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	size_t bytes_read;
	char* program = load_program(argv[1], &bytes_read);
	if(program == NULL) {
		printf("Could not read %s\n", argv[1]);
		exit(1);
	}

	uchar* memory = malloc(30000 * sizeof(uchar));
	memset(memory, 0, 30000);

	char* prog_ptr = program;
	uchar* mem_ptr = memory;

	while(prog_ptr - program < bytes_read) {
		switch(*prog_ptr) {
			case '>': ++mem_ptr; break;
			case '<': --mem_ptr; break;
			case '+': ++(*mem_ptr); break;
			case '-': --(*mem_ptr); break;
			case '.': putchar(*mem_ptr); break;
			case ',': *mem_ptr = getchar(); break;
		}
		++prog_ptr;
	}
}
