#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

#define READ_CHUNK 100

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
	char* prog = load_program(argv[1], &bytes_read);
	if(prog == NULL) printf("dupa\n");
	else {
		printf("read bytes: %lu\n", bytes_read);
		for(size_t i=0; i<bytes_read; ++i) {
			putchar(prog[i]);
		}
		free(prog);
	}
}
